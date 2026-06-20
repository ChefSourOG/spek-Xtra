#pragma once

#include <wx/wx.h>

#include <cstdint>

struct SpekAudioInfo
{
    wxString file_type;
    int sample_rate = 0;
    int bit_depth = 0;
    int channels = 0;
    double duration = 0.0;
    int nyquist = 0;
    double file_size_mb = 0.0;
    int64_t total_samples = 0;
    int64_t analysis_frames = 0;
    int fft_size = 0;
    double freq_resolution = 0.0;
};

class SpekInfoPanel : public wxPanel
{
public:
    SpekInfoPanel(wxWindow *parent);
    void set_info(const SpekAudioInfo& info);
    void clear();
    void set_secondary_info(const SpekAudioInfo& info);
    void clear_secondary();

private:
    void add_row(wxFlexGridSizer *sizer, const wxString& label, wxStaticText *value);
    wxString format_duration(double duration) const;
    void fill_values(
        wxStaticText *file_type_value,
        wxStaticText *sample_rate_value,
        wxStaticText *bit_depth_value,
        wxStaticText *channels_value,
        wxStaticText *duration_value,
        wxStaticText *nyquist_value,
        wxStaticText *file_size_value,
        wxStaticText *total_samples_value,
        wxStaticText *analysis_frames_value,
        wxStaticText *fft_size_value,
        wxStaticText *freq_resolution_value,
        const SpekAudioInfo& info);

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

    wxStaticText *secondary_title;
    wxStaticText *file_type_value2;
    wxStaticText *sample_rate_value2;
    wxStaticText *bit_depth_value2;
    wxStaticText *channels_value2;
    wxStaticText *duration_value2;
    wxStaticText *nyquist_value2;
    wxStaticText *file_size_value2;
    wxStaticText *total_samples_value2;
    wxStaticText *analysis_frames_value2;
    wxStaticText *fft_size_value2;
    wxStaticText *freq_resolution_value2;
};
