#include <cmath>

#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/filename.h>

#include "spek-audio.h"
#include "spek-events.h"
#include "spek-fft.h"
#include "spek-info-panel.h"
#include "spek-platform.h"
#include "spek-preferences.h"
#include "spek-ruler.h"
#include "spek-utils.h"

#include "spek-spectrogram.h"

BEGIN_EVENT_TABLE(SpekSpectrogram, wxWindow)
    EVT_CHAR(SpekSpectrogram::on_char)
    EVT_PAINT(SpekSpectrogram::on_paint)
    EVT_SIZE(SpekSpectrogram::on_size)
    EVT_MOUSEWHEEL(SpekSpectrogram::on_mouse_wheel)
    EVT_LEFT_DOWN(SpekSpectrogram::on_mouse_left_down)
    EVT_MOTION(SpekSpectrogram::on_mouse_motion)
    EVT_LEFT_UP(SpekSpectrogram::on_mouse_left_up)
    EVT_LEAVE_WINDOW(SpekSpectrogram::on_mouse_leave)
    SPEK_EVT_HAVE_SAMPLE(SpekSpectrogram::on_have_sample)
END_EVENT_TABLE()

// Constants
const int SpekSpectrogram::MIN_RANGE = -140;
const int SpekSpectrogram::MAX_RANGE = 0;
const int SpekSpectrogram::URANGE = 0;
const int SpekSpectrogram::LRANGE = -120;
const int SpekSpectrogram::FFT_BITS = 11;
const int SpekSpectrogram::MIN_FFT_BITS = 8;
const int SpekSpectrogram::MAX_FFT_BITS = 14;
const int SpekSpectrogram::LPAD = 60 * spek_platform_dpi_scale();
const int SpekSpectrogram::TPAD = 60 * spek_platform_dpi_scale();
const int SpekSpectrogram::RPAD = 92 * spek_platform_dpi_scale();
const int SpekSpectrogram::BPAD = 40 * spek_platform_dpi_scale();
const int SpekSpectrogram::GAP = 10 * spek_platform_dpi_scale();
const int SpekSpectrogram::RULER = 10 * spek_platform_dpi_scale();
const int SpekSpectrogram::MAX_IMAGE_SAMPLES = 8192;

// Forward declarations.
static wxString trim(wxDC& dc, const wxString& s, int length, bool trim_end);
static int bits_to_bands(int bits);
static wxString format_freq(double freq);
static wxString format_time(double time);

SpekSpectrogram::SpekSpectrogram(wxWindow *parent) :
    wxWindow(
        parent, -1, wxDefaultPosition, wxDefaultSize,
        wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS
    ),
    audio(new Audio()), // TODO: refactor
    fft(new FFT()),
    pipeline(NULL),
    streams(0),
    stream(0),
    channels(0),
    channel(0),
    window_function(WINDOW_DEFAULT),
    duration(0.0),
    sample_rate(0),
    bits_per_sample(0),
    fft_size(0),
    palette(PALETTE_DEFAULT),
    palette_image(),
    image(1, 1),
    image_data(),
    status_bar(NULL),
    prev_width(-1),
    fft_bits(FFT_BITS),
    urange(URANGE),
    lrange(LRANGE),
    viewport_x(0.0),
    viewport_y(0.0),
    viewport_width(1.0),
    viewport_height(1.0),
    axes_linked(true),
    dragging(false),
    drag_start(0, 0),
    drag_last(0, 0),
    drag_viewport_x(0.0),
    drag_viewport_y(0.0),
    image_samples(0)
{
    this->create_palette();

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetFocus();
}

SpekSpectrogram::~SpekSpectrogram()
{
    this->stop();
}

void SpekSpectrogram::open(const wxString& path, const wxString& pngpath)
{
    this->path = path;
    this->pngpath = pngpath;
    this->stream = 0;
    this->channel = 0;
    // Force the pipeline to restart even if the viewport/image size hasn't changed.
    this->image_samples = 0;
    this->reset_view();
}

void SpekSpectrogram::get_info(SpekAudioInfo& info) const
{
    info.file_type = this->format_name;
    info.sample_rate = this->sample_rate;
    info.bit_depth = this->bits_per_sample;
    info.channels = this->channels;
    info.duration = this->duration;
    info.nyquist = this->sample_rate / 2;

    wxFileName file_name(this->path);
    wxULongLong size = file_name.GetSize();
    info.file_size_mb = size != wxInvalidSize
        ? size.ToDouble() / (1024.0 * 1024.0)
        : 0.0;

    info.total_samples = (int64_t)(this->duration * this->sample_rate + 0.5);
    info.fft_size = this->fft_size;
    info.analysis_frames = this->fft_size > 0
        ? (info.total_samples / this->fft_size)
        : 0;
    info.freq_resolution = this->fft_size > 0
        ? (double)this->sample_rate / this->fft_size
        : 0.0;
}

void SpekSpectrogram::save(const wxString& path)
{
    wxSize size = GetClientSize();
    wxBitmap bitmap(size.GetWidth(), size.GetHeight());
    wxMemoryDC dc(bitmap);
    render(dc, size.GetWidth(), size.GetHeight());
    bitmap.SaveFile(path, wxBITMAP_TYPE_PNG);
}

// Forward declaration used by render_export and start.
static void pipeline_cb(int bands, int sample, float *values, void *cb_data);

wxBitmap SpekSpectrogram::render_export(int width, int height)
{
    if (this->path.IsEmpty()) {
        return wxBitmap(width, height);
    }

    // Stop the current analysis and preserve the in-window image and viewport.
    this->stop();
    wxImage saved_image = this->image;
    std::vector<float> saved_image_data = this->image_data;
    double saved_viewport_x = this->viewport_x;
    double saved_viewport_y = this->viewport_y;
    double saved_viewport_width = this->viewport_width;
    double saved_viewport_height = this->viewport_height;
    int saved_image_samples = this->image_samples;

    // Render the full file at the requested resolution.
    this->viewport_x = 0.0;
    this->viewport_y = 0.0;
    this->viewport_width = 1.0;
    this->viewport_height = 1.0;
    int samples = width - LPAD - RPAD;
    this->image_samples = samples > 0 ? samples : 1;
    if (samples > 0) {
        int bands = bits_to_bands(this->fft_bits);
        this->image.Create(samples, bands);
        this->image_data.assign(samples * bands, this->lrange);
        this->pipeline = spek_pipeline_open(
            this->audio->open(std::string(this->path.utf8_str()), this->stream),
            this->fft->create(this->fft_bits),
            this->stream,
            this->channel,
            this->window_function,
            samples,
            pipeline_cb,
            this
        );
        spek_pipeline_start(this->pipeline);

        // Process events until the export pipeline finishes.
        while (this->pipeline) {
            wxApp::GetInstance()->ProcessPendingEvents();
        }
    } else {
        this->image.Create(1, 1);
        this->image_data.assign(1, this->lrange);
    }

    wxBitmap bitmap(width, height);
    wxMemoryDC dc(bitmap);
    render(dc, width, height);
    dc.SelectObject(wxNullBitmap);

    // Restore the in-window image, viewport, and restart the normal analysis.
    this->image = saved_image;
    this->image_data = saved_image_data;
    this->viewport_x = saved_viewport_x;
    this->viewport_y = saved_viewport_y;
    this->viewport_width = saved_viewport_width;
    this->viewport_height = saved_viewport_height;
    this->image_samples = saved_image_samples;
    this->start();

    return bitmap;
}

void SpekSpectrogram::on_char(wxKeyEvent& evt)
{
    int key_code = evt.GetKeyCode();

#ifdef OS_OSX
    int key_modifier = evt.GetModifiers();
    // Using Command-W to close the window in macOS
    if (key_modifier == wxMOD_CONTROL && key_code == 'W') {
        this->GetParent()->Close(true);
        return;
    }
#endif
    switch (key_code) {
    case 'c':
        if (this->channels) {
            this->channel = (this->channel + 1) % this->channels;
        }
        break;
    case 'C':
        if (this->channels) {
            this->channel = (this->channel - 1 + this->channels) % this->channels;
        }
        break;
    case 'f':
        this->window_function = (enum window_function) ((this->window_function + 1) % WINDOW_COUNT);
        break;
    case 'F':
        this->window_function =
            (enum window_function) ((this->window_function - 1 + WINDOW_COUNT) % WINDOW_COUNT);
        break;
    case 'l':
        this->lrange = spek_min(this->lrange + 1, this->urange - 1);
        break;
    case 'L':
        this->lrange = spek_max(this->lrange - 1, MIN_RANGE);
        break;
    case 'p':
        this->palette = (enum palette) ((this->palette + 1) % PALETTE_COUNT);
        this->create_palette();
        break;
    case 'P':
        this->palette = (enum palette) ((this->palette - 1 + PALETTE_COUNT) % PALETTE_COUNT);
        this->create_palette();
        break;
    case 's':
        if (this->streams) {
            if (this->streams == 1) {
                evt.Skip();
                return;
            }
            this->stream = (this->stream + 1) % this->streams;
        }
        break;
    case 'S':
        if (this->streams) {
            if (this->streams == 1) {
                evt.Skip();
                return;
            }
            this->stream = (this->stream - 1 + this->streams) % this->streams;
        }
        break;
    case 'u':
        this->urange = spek_min(this->urange + 1, MAX_RANGE);
        break;
    case 'U':
        this->urange = spek_max(this->urange - 1, this->lrange + 1);
        break;
    case 'w':
        this->fft_bits = spek_min(this->fft_bits + 1, MAX_FFT_BITS);
        this->create_palette();
        break;
    case 'W':
        this->fft_bits = spek_max(this->fft_bits - 1, MIN_FFT_BITS);
        this->create_palette();
        break;
    default:
        evt.Skip();
        return;
    }

    start();
    Refresh();
}

void SpekSpectrogram::on_paint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();
    render(dc, size.GetWidth(), size.GetHeight());
}

void SpekSpectrogram::on_size(wxSizeEvent&)
{
    wxSize size = GetClientSize();
    bool width_changed = this->prev_width != size.GetWidth();
    this->prev_width = size.GetWidth();

    if (width_changed) {
        this->restart_if_needed();
    }
}

void SpekSpectrogram::on_have_sample(wxEvent& event)
{
    SpekHaveSampleEvent *ev = dynamic_cast<SpekHaveSampleEvent *>(&event);
    if (ev == nullptr) {
        return;
    }

    int bands = ev->get_bands();
    int sample = ev->get_sample();
    const float *values = ev->get_values();

    if (sample == -1) {
        this->stop();

        if (!this->pngpath.IsEmpty()) {
            this->save(this->pngpath);
            this->GetParent()->Close(true);
        }

        return;
    }

    // TODO: check image size, quit if wrong.
    double range = this->urange - this->lrange;
    for (int y = 0; y < bands; y++) {
        int row = bands - y - 1;
        this->image_data[sample * bands + row] = values[y];
        double value = fmin(this->urange, fmax(this->lrange, values[y]));
        double level = (value - this->lrange) / range;
        uint32_t color = spek_palette(this->palette, level);
        this->image.SetRGB(
            sample,
            row,
            color >> 16,
            (color >> 8) & 0xFF,
            color & 0xFF
        );
    }

    // TODO: refresh only one pixel column
    this->Refresh();
}

static wxString time_formatter(int unit)
{
    // TODO: i18n
    return wxString::Format("%d:%02d", unit / 60, unit % 60);
}

static wxString freq_formatter(int unit)
{
    return wxString::Format(_("%d kHz"), unit / 1000);
}

static wxString density_formatter(int unit)
{
    return wxString::Format(_("%d dB"), -unit);
}

static wxString format_freq(double freq)
{
    if (freq < 1000.0) {
        return wxString::Format(_("%.0f Hz"), freq);
    }
    return wxString::Format(_("%.1f kHz"), freq / 1000.0);
}

static wxString format_time(double time)
{
    int total_ms = (int)(time * 1000.0 + 0.5);
    int ms = total_ms % 1000;
    int secs = (total_ms / 1000) % 60;
    int mins = total_ms / 60000;
    return wxString::Format("%d:%02d.%03d", mins, secs, ms);
}

void SpekSpectrogram::render(wxDC& dc, int width, int height)
{
    int w = width;
    int h = height;

    // Initialise.
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetTextForeground(wxColour(255, 255, 255));
    wxFont normal_font = wxFont(
        (int)round(9 * spek_platform_font_scale()),
        wxFONTFAMILY_SWISS,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL
    );
    wxFont normal_bold_font = wxFont(normal_font);
    normal_bold_font.SetWeight(wxFONTWEIGHT_BOLD);
    wxFont large_font = wxFont(normal_font);
    large_font.SetPointSize((int)round(10 * spek_platform_font_scale()));
    large_font.SetWeight(wxFONTWEIGHT_BOLD);
    wxFont small_font = wxFont(normal_font);
    small_font.SetPointSize((int)round(8 * spek_platform_font_scale()));
    dc.SetFont(normal_font);
    int normal_height = dc.GetTextExtent("dummy").GetHeight();
    dc.SetFont(large_font);
    int large_height = dc.GetTextExtent("dummy").GetHeight();
    dc.SetFont(small_font);
    int small_height = dc.GetTextExtent("dummy").GetHeight();

    // Clean the background.
    dc.Clear();

    // Spek version
    dc.SetFont(normal_bold_font);
    wxString package_name(PACKAGE_NAME);
    dc.DrawText(
        package_name,
        w - RPAD + GAP,
        TPAD - 2 * GAP - normal_height - normal_height
    );
    int package_name_width = dc.GetTextExtent(package_name + " ").GetWidth();
    dc.SetFont(small_font);
    dc.DrawText(
        PACKAGE_VERSION,
        w - RPAD + GAP + package_name_width,
        TPAD - 2 * GAP - normal_height - small_height
    );

    int plot_w = w - LPAD - RPAD;
    int plot_h = h - TPAD - BPAD;
    int img_w = this->image.GetWidth();
    int img_h = this->image.GetHeight();

    if (img_w > 1 && img_h > 1 && plot_w > 0 && plot_h > 0) {
        // Draw the visible portion of the spectrogram.
        double src_xd = this->viewport_x * (img_w - 1);
        double src_yd = (1.0 - this->viewport_y - this->viewport_height) * (img_h - 1);
        double src_wd = this->viewport_width * (img_w - 1);
        double src_hd = this->viewport_height * (img_h - 1);

        int src_x = (int)round(src_xd);
        int src_y = (int)round(src_yd);
        int src_w = (int)round(src_wd);
        int src_h = (int)round(src_hd);

        if (src_x < 0) src_x = 0;
        if (src_y < 0) src_y = 0;
        if (src_x >= img_w) src_x = img_w - 1;
        if (src_y >= img_h) src_y = img_h - 1;
        if (src_x + src_w > img_w) src_w = img_w - src_x;
        if (src_y + src_h > img_h) src_h = img_h - src_y;
        if (src_w < 1) src_w = 1;
        if (src_h < 1) src_h = 1;

        wxImage sub = this->image.GetSubImage(wxRect(src_x, src_y, src_w, src_h));
        wxBitmap bmp(sub.Scale(plot_w, plot_h));
        dc.DrawBitmap(bmp, LPAD, TPAD);

        // File name.
        // Checking prefs can probably be solved differently.
        SpekPreferences& prefs = SpekPreferences::get();
        wxString file_name = this->path;

        if (prefs.get_hide_full_path()) {
            wxFileName file_path(this->path);
            file_name = file_path.GetFullName();
        }

        dc.SetFont(large_font);
        dc.DrawText(
            trim(dc, file_name, plot_w, false),
            LPAD,
            TPAD - 2 * GAP - normal_height - large_height
        );

        // File properties.
        dc.SetFont(normal_font);
        dc.DrawText(
            trim(dc, this->desc, plot_w, true),
            LPAD,
            TPAD - GAP - normal_height
        );

        // Prepare to draw the rulers.
        dc.SetFont(small_font);

        if (this->duration > 0.0) {
            // Time ruler.
            int time_factors[] = {1, 2, 5, 10, 20, 30, 1*60, 2*60, 5*60, 10*60, 20*60, 30*60, 0};
            double time_min = this->viewport_x * this->duration;
            double time_max = (this->viewport_x + this->viewport_width) * this->duration;
            SpekRuler time_ruler(
                LPAD,
                h - BPAD,
                SpekRuler::BOTTOM,
                // TODO: i18n
                "00:00",
                time_factors,
                (int)time_min,
                (int)time_max,
                1.5,
                plot_w / (time_max - time_min),
                0.0,
                time_formatter
                );
            time_ruler.draw(dc);
        }

        if (this->sample_rate > 0) {
            // Frequency ruler.
            double freq = this->sample_rate / 2.0;
            double freq_min = this->viewport_y * freq;
            double freq_max = (this->viewport_y + this->viewport_height) * freq;
            int freq_factors[] = {1000, 2000, 5000, 10000, 20000, 0};
            SpekRuler freq_ruler(
                LPAD,
                TPAD,
                SpekRuler::LEFT,
                // TRANSLATORS: keep "00" unchanged, it's used to calc the text width
                _("00 kHz"),
                freq_factors,
                (int)freq_min,
                (int)freq_max,
                3.0,
                plot_h / (freq_max - freq_min),
                0.0,
                freq_formatter
                );
            freq_ruler.draw(dc);
        }
    }

    // Border around the spectrogram.
    dc.DrawRectangle(LPAD, TPAD, w - LPAD - RPAD, h - TPAD - BPAD);

    // The palette.
    if (h - TPAD - BPAD > 0) {
        wxBitmap bmp(this->palette_image.Scale(RULER, h - TPAD - BPAD + 1));
        dc.DrawBitmap(bmp, w - RPAD + GAP, TPAD);

        // Prepare to draw the ruler.
        dc.SetFont(small_font);

        // Spectral density.
        int density_factors[] = {1, 2, 5, 10, 20, 50, 0};
        SpekRuler density_ruler(
            w - RPAD + GAP + RULER,
            TPAD,
            SpekRuler::RIGHT,
            // TRANSLATORS: keep "-00" unchanged, it's used to calc the text width
            _("-00 dB"),
            density_factors,
            -this->urange,
            -this->lrange,
            3.0,
            (h - TPAD - BPAD) / (double)(this->lrange - this->urange),
            h - TPAD - BPAD,
            density_formatter
        );
        density_ruler.draw(dc);
    }
}

static void pipeline_cb(int bands, int sample, float *values, void *cb_data)
{
    SpekHaveSampleEvent event(bands, sample, values, false);
    SpekSpectrogram *s = (SpekSpectrogram *)cb_data;
    wxPostEvent(s, event);
}

void SpekSpectrogram::start()
{
    if (this->path.IsEmpty()) {
        return;
    }

    this->stop();

    // The number of samples is based on the window width and the current time
    // zoom so the visible time range is rendered at adequate resolution.
    // The number of bands is fixed, FFT results are very different for
    // different values but we need some consistency.
    int samples = this->calc_image_samples();
    this->image_samples = samples;
    if (samples > 0) {
        int bands = bits_to_bands(this->fft_bits);
        this->image.Create(samples, bands);
        this->image_data.assign(samples * bands, this->lrange);
        this->pipeline = spek_pipeline_open(
            this->audio->open(std::string(this->path.utf8_str()), this->stream),
            this->fft->create(this->fft_bits),
            this->stream,
            this->channel,
            this->window_function,
            samples,
            pipeline_cb,
            this
        );
        spek_pipeline_start(this->pipeline);
        // TODO: extract conversion into a utility function.
        this->desc = wxString::FromUTF8(spek_pipeline_desc(this->pipeline).c_str());
        this->format_name = wxString::FromUTF8(spek_pipeline_format_name(this->pipeline).c_str());
        this->streams = spek_pipeline_streams(this->pipeline);
        this->channels = spek_pipeline_channels(this->pipeline);
        this->duration = spek_pipeline_duration(this->pipeline);
        this->sample_rate = spek_pipeline_sample_rate(this->pipeline);
        this->bits_per_sample = spek_pipeline_bits_per_sample(this->pipeline);
        this->fft_size = 1 << this->fft_bits;
    } else {
        this->image.Create(1, 1);
        this->image_data.assign(1, this->lrange);
    }
}

void SpekSpectrogram::stop()
{
    if (this->pipeline) {
        spek_pipeline_close(this->pipeline);
        this->pipeline = NULL;

        // Make sure all have_sample events are processed before returning.
        wxApp::GetInstance()->ProcessPendingEvents();
    }
}

void SpekSpectrogram::set_fft_bits(int bits)
{
    if (bits < MIN_FFT_BITS) {
        bits = MIN_FFT_BITS;
    } else if (bits > MAX_FFT_BITS) {
        bits = MAX_FFT_BITS;
    }
    if (bits != this->fft_bits) {
        this->fft_bits = bits;
        this->create_palette();
        this->start();
    }
}

void SpekSpectrogram::set_window_function(enum window_function f)
{
    if (f >= 0 && f < WINDOW_COUNT && f != this->window_function) {
        this->window_function = f;
        this->start();
    }
}

void SpekSpectrogram::set_palette(enum palette p)
{
    if (p >= 0 && p < PALETTE_COUNT && p != this->palette) {
        this->palette = p;
        this->create_palette();
        this->start();
    }
}

void SpekSpectrogram::reset_view()
{
    this->viewport_x = 0.0;
    this->viewport_y = 0.0;
    this->viewport_width = 1.0;
    this->viewport_height = 1.0;
    this->restart_if_needed();
    this->Refresh();
}

void SpekSpectrogram::set_zoom(double factor, double center_t, double center_f, bool zoom_x, bool zoom_y)
{
    if (factor <= 0.0 || (!zoom_x && !zoom_y)) {
        return;
    }

    if (zoom_x) {
        double new_width = this->viewport_width / factor;
        double rel = 0.5;
        if (this->viewport_width > 0.0) {
            rel = (center_t - this->viewport_x) / this->viewport_width;
        }
        this->viewport_x = center_t - rel * new_width;
        this->viewport_width = new_width;
    }

    if (zoom_y) {
        double new_height = this->viewport_height / factor;
        double rel = 0.5;
        if (this->viewport_height > 0.0) {
            rel = (center_f - this->viewport_y) / this->viewport_height;
        }
        this->viewport_y = center_f - rel * new_height;
        this->viewport_height = new_height;
    }

    this->constrain_viewport();
    this->restart_if_needed();
    this->Refresh();
}

void SpekSpectrogram::set_pan(int dx, int dy)
{
    wxSize size = GetClientSize();
    int plot_w = size.GetWidth() - LPAD - RPAD;
    int plot_h = size.GetHeight() - TPAD - BPAD;

    if (plot_w > 0 && dx != 0) {
        this->viewport_x -= (double)dx / plot_w * this->viewport_width;
    }
    if (plot_h > 0 && dy != 0) {
        this->viewport_y -= (double)dy / plot_h * this->viewport_height;
    }

    this->constrain_viewport();
    this->Refresh();
}

void SpekSpectrogram::set_axes_linked(bool linked)
{
    this->axes_linked = linked;
}

int SpekSpectrogram::calc_image_samples() const
{
    wxSize size = GetClientSize();
    int base = size.GetWidth() - LPAD - RPAD;
    if (base < 1) {
        base = 1;
    }
    if (this->viewport_width <= 0.0 || this->viewport_width >= 1.0) {
        return base;
    }
    double needed = base / this->viewport_width;
    if (needed > MAX_IMAGE_SAMPLES) {
        needed = MAX_IMAGE_SAMPLES;
    }
    return (int)needed;
}

void SpekSpectrogram::restart_if_needed()
{
    if (this->path.IsEmpty()) {
        return;
    }
    int needed = this->calc_image_samples();
    if (needed != this->image_samples) {
        this->start();
    }
}

void SpekSpectrogram::constrain_viewport()
{
    const double MIN_VIEWPORT = 1e-6;

    if (this->viewport_width < MIN_VIEWPORT) {
        this->viewport_width = MIN_VIEWPORT;
    }
    if (this->viewport_height < MIN_VIEWPORT) {
        this->viewport_height = MIN_VIEWPORT;
    }
    if (this->viewport_width > 1.0) {
        this->viewport_width = 1.0;
    }
    if (this->viewport_height > 1.0) {
        this->viewport_height = 1.0;
    }
    if (this->viewport_x < 0.0) {
        this->viewport_x = 0.0;
    }
    if (this->viewport_y < 0.0) {
        this->viewport_y = 0.0;
    }
    if (this->viewport_x + this->viewport_width > 1.0) {
        this->viewport_x = 1.0 - this->viewport_width;
    }
    if (this->viewport_y + this->viewport_height > 1.0) {
        this->viewport_y = 1.0 - this->viewport_height;
    }
}

void SpekSpectrogram::on_mouse_wheel(wxMouseEvent& evt)
{
    wxSize size = GetClientSize();
    int plot_w = size.GetWidth() - LPAD - RPAD;
    int plot_h = size.GetHeight() - TPAD - BPAD;
    int mx = evt.GetX() - LPAD;
    int my = evt.GetY() - TPAD;

    if (plot_w <= 0 || plot_h <= 0 || mx < 0 || mx >= plot_w || my < 0 || my >= plot_h) {
        evt.Skip();
        return;
    }

    double rotation = evt.GetWheelRotation();
    if (rotation == 0.0) {
        return;
    }

    double factor = (rotation > 0.0) ? 1.1 : 1.0 / 1.1;
    double center_t = this->viewport_x + (double)mx / plot_w * this->viewport_width;
    double center_f = this->viewport_y + (double)my / plot_h * this->viewport_height;

    bool zoom_x, zoom_y;
    if (evt.ControlDown()) {
        zoom_x = true;
        zoom_y = false;
    } else if (evt.ShiftDown()) {
        zoom_x = false;
        zoom_y = true;
    } else if (this->axes_linked) {
        zoom_x = true;
        zoom_y = true;
    } else {
        zoom_x = true;
        zoom_y = false;
    }

    this->set_zoom(factor, center_t, center_f, zoom_x, zoom_y);
}

void SpekSpectrogram::on_mouse_left_down(wxMouseEvent& evt)
{
    wxSize size = GetClientSize();
    int plot_w = size.GetWidth() - LPAD - RPAD;
    int plot_h = size.GetHeight() - TPAD - BPAD;
    int mx = evt.GetX() - LPAD;
    int my = evt.GetY() - TPAD;

    if (plot_w <= 0 || plot_h <= 0 || mx < 0 || mx >= plot_w || my < 0 || my >= plot_h) {
        evt.Skip();
        return;
    }

    this->dragging = true;
    this->drag_start = evt.GetPosition();
    this->drag_last = evt.GetPosition();
    this->drag_viewport_x = this->viewport_x;
    this->drag_viewport_y = this->viewport_y;
    this->CaptureMouse();
}

void SpekSpectrogram::on_mouse_motion(wxMouseEvent& evt)
{
    int mx = evt.GetX() - LPAD;
    int my = evt.GetY() - TPAD;
    this->update_readout(mx, my);

    if (!this->dragging) {
        evt.Skip();
        return;
    }

    if (!evt.LeftIsDown()) {
        this->dragging = false;
        if (this->HasCapture()) {
            this->ReleaseMouse();
        }
        return;
    }

    int dx = evt.GetX() - this->drag_last.x;
    int dy = evt.GetY() - this->drag_last.y;
    this->drag_last = evt.GetPosition();

    this->set_pan(dx, dy);
}

void SpekSpectrogram::on_mouse_leave(wxMouseEvent& evt)
{
    if (this->status_bar) {
        this->status_bar->SetStatusText(wxEmptyString);
    }
    evt.Skip();
}

void SpekSpectrogram::update_readout(int mx, int my)
{
    if (!this->status_bar) {
        return;
    }

    if (this->path.IsEmpty()) {
        this->status_bar->SetStatusText(wxEmptyString);
        return;
    }

    wxSize size = GetClientSize();
    int plot_w = size.GetWidth() - LPAD - RPAD;
    int plot_h = size.GetHeight() - TPAD - BPAD;
    int img_w = this->image.GetWidth();
    int img_h = this->image.GetHeight();

    if (plot_w <= 0 || plot_h <= 0 || img_w <= 1 || img_h <= 1 ||
        mx < 0 || mx >= plot_w || my < 0 || my >= plot_h) {
        this->status_bar->SetStatusText(wxEmptyString);
        return;
    }

    double time_norm = this->viewport_x + (double)mx / plot_w * this->viewport_width;
    double time_sec = time_norm * this->duration;

    double freq_norm = this->viewport_y + (1.0 - (double)my / plot_h) * this->viewport_height;
    double freq_hz = freq_norm * this->sample_rate / 2.0;

    double img_col_d = this->viewport_x * (img_w - 1) +
        (double)mx / plot_w * this->viewport_width * (img_w - 1);
    double img_row_d = (1.0 - this->viewport_y - this->viewport_height) * (img_h - 1) +
        (double)my / plot_h * this->viewport_height * (img_h - 1);
    int img_col = (int)round(fmax(0.0, fmin((double)(img_w - 1), img_col_d)));
    int img_row = (int)round(fmax(0.0, fmin((double)(img_h - 1), img_row_d)));

    float db = this->image_data[img_col * img_h + img_row];

    wxString freq_str = format_freq(freq_hz);
    wxString time_str = format_time(time_sec);
    wxString db_str = wxString::Format(_("%.0f dB"), db);
    this->status_bar->SetStatusText(
        wxString::Format(_("%s, %s, %s"), freq_str, time_str, db_str)
    );
}

void SpekSpectrogram::on_mouse_left_up(wxMouseEvent& evt)
{
    if (this->dragging) {
        this->dragging = false;
        if (this->HasCapture()) {
            this->ReleaseMouse();
        }
    } else {
        evt.Skip();
    }
}

void SpekSpectrogram::create_palette()
{
    this->palette_image.Create(RULER, bits_to_bands(this->fft_bits));
    for (int y = 0; y < bits_to_bands(this->fft_bits); y++) {
        uint32_t color = spek_palette(this->palette, y / (double)bits_to_bands(this->fft_bits));
        this->palette_image.SetRGB(
            wxRect(0, bits_to_bands(this->fft_bits) - y - 1, RULER, 1),
            color >> 16,
            (color >> 8) & 0xFF,
            color & 0xFF
        );
    }
}

// Trim `s` so that it fits into `length`.
static wxString trim(wxDC& dc, const wxString& s, int length, bool trim_end)
{
    if (length <= 0) {
        return wxEmptyString;
    }

    // Check if the entire string fits.
    wxSize size = dc.GetTextExtent(s);
    if (size.GetWidth() <= length) {
        return s;
    }

    // Binary search FTW!
    wxString fix("...");
    int i = 0;
    int k = s.length();
    while (k - i > 1) {
        int j = (i + k) / 2;
        size = dc.GetTextExtent(trim_end ? s.substr(0, j) + fix : fix + s.substr(j));
        if (trim_end != (size.GetWidth() > length)) {
            i = j;
        } else {
            k = j;
        }
    }

    return trim_end ? s.substr(0, i) + fix : fix + s.substr(k);
}

// TODO: test
static int bits_to_bands(int bits) {
    return (1 << (bits - 1)) + 1;
}
