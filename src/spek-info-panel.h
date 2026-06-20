#pragma once

#include <wx/wx.h>

#include <cstdint>

struct SpekAudioInfo
{
    wxString file_type;
    int sample_rate;
    int bit_depth;
    int channels;
    double duration;
    int nyquist;
    double file_size_mb;
    int64_t total_samples;
    int64_t analysis_frames;
    int fft_size;
    double freq_resolution;
};

class SpekInfoPanel : public wxPanel
{
public:
    SpekInfoPanel(wxWindow *parent);
    void set_info(const SpekAudioInfo& info);
    void clear();

private:
    void add_row(wxFlexGridSizer *sizer, const wxString& label, wxStaticText *value);
    wxString format_duration(double duration) const;

    wxStaticText *file_type_value;
    wxStaticText *sample_rate_value;
    wxStaticText *bit_depth_value;
    wxStaticText *channels_value;
    wxStaticText *duration_value;
    wxStaticText *nyquist_value;
    wxStaticText *file_size_value;
    wxStaticText *total_samples_value;
    wxStaticText *analysis_frames_value;
    wxStaticText *fft_size_value;
    wxStaticText *freq_resolution_value;
};
