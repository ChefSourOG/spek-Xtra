#pragma once

#include <memory>
#include <vector>

#include <wx/wx.h>
#include <wx/statusbr.h>

#include "spek-info-panel.h"
#include "spek-palette.h"
#include "spek-pipeline.h"

class Audio;
class FFT;
class SpekHaveSampleEvent;
struct spek_pipeline;

class SpekSpectrogram : public wxWindow
{
public:
    SpekSpectrogram(wxWindow *parent);
    ~SpekSpectrogram();
    void open(const wxString& path, const wxString& pngpath);
    void save(const wxString& path);
    wxBitmap render_export(int width, int height);
    void get_info(SpekAudioInfo& info) const;
    void set_fft_bits(int bits);
    int get_fft_bits() const { return this->fft_bits; }
    void set_window_function(enum window_function f);
    enum window_function get_window_function() const { return this->window_function; }
    void set_palette(enum palette p);
    enum palette get_palette() const { return this->palette; }

    void reset_view();
    void set_zoom(double factor, double center_t, double center_f, bool zoom_x, bool zoom_y);
    void set_pan(int dx, int dy);
    void set_axes_linked(bool linked);
    bool get_axes_linked() const { return this->axes_linked; }
    void set_status_bar(wxStatusBar *status_bar) { this->status_bar = status_bar; }

private:
    void on_char(wxKeyEvent& evt);
    void on_paint(wxPaintEvent& evt);
    void on_size(wxSizeEvent& evt);
    void on_have_sample(wxEvent& evt);
    void on_mouse_wheel(wxMouseEvent& evt);
    void on_mouse_left_down(wxMouseEvent& evt);
    void on_mouse_motion(wxMouseEvent& evt);
    void on_mouse_left_up(wxMouseEvent& evt);
    void on_mouse_leave(wxMouseEvent& evt);
    void render(wxDC& dc, int width, int height);
    void update_readout(int mx, int my);

    void start();
    void stop();
    int calc_image_samples() const;
    void restart_if_needed();
    void constrain_viewport();

    void create_palette();

    static const int MIN_RANGE;
    static const int MAX_RANGE;
    static const int URANGE;
    static const int LRANGE;
    static const int FFT_BITS;
    static const int MIN_FFT_BITS;
    static const int MAX_FFT_BITS;
    static const int LPAD;
    static const int TPAD;
    static const int RPAD;
    static const int BPAD;
    static const int GAP;
    static const int RULER;
    static const int MAX_IMAGE_SAMPLES;

    std::unique_ptr<Audio> audio;
    std::unique_ptr<FFT> fft;
    spek_pipeline *pipeline;
    int streams;
    int stream;
    int channels;
    int channel;
    enum window_function window_function;
    wxString path;
    wxString pngpath;
    wxString desc;
    wxString format_name;
    double duration;
    int sample_rate;
    int bits_per_sample;
    int fft_size;
    enum palette palette;
    wxImage palette_image;
    wxImage image;
    std::vector<float> image_data;
    wxStatusBar *status_bar;
    int prev_width;
    int fft_bits;
    int urange;
    int lrange;

    // Viewport in normalized time/frequency domain [0, 1].
    // x=time (0=start, 1=end), y=frequency (0=DC/bottom, 1=Nyquist/top).
    double viewport_x, viewport_y, viewport_width, viewport_height;
    bool axes_linked;
    bool dragging;
    wxPoint drag_start;
    wxPoint drag_last;
    double drag_viewport_x, drag_viewport_y;
    int image_samples;

    DECLARE_EVENT_TABLE()
};
