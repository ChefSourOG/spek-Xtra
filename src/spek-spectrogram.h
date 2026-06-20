#pragma once

#include <memory>

#include <wx/wx.h>

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

private:
    void on_char(wxKeyEvent& evt);
    void on_paint(wxPaintEvent& evt);
    void on_size(wxSizeEvent& evt);
    void on_have_sample(wxEvent& evt);
    void render(wxDC& dc, int width, int height);

    void start();
    void stop();

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
    int prev_width;
    int fft_bits;
    int urange;
    int lrange;

    DECLARE_EVENT_TABLE()
};
