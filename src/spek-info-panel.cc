#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "spek-info-panel.h"

namespace {

wxStaticText *create_value_ctrl(wxWindow *parent)
{
    wxSize value_min_size(120, -1);
    return new wxStaticText(parent, -1, wxEmptyString, wxDefaultPosition, value_min_size);
}

wxStaticText *create_title(wxWindow *parent, const wxString& text)
{
    wxStaticText *title = new wxStaticText(parent, -1, text);
    wxFont title_font = title->GetFont();
    title_font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(title_font);
    return title;
}

} // namespace

SpekInfoPanel::SpekInfoPanel(wxWindow *parent) :
    wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN)
{
    wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);

    outer_sizer->Add(create_title(this, _("Audio Info")), 0, wxALL, 8);

    wxFlexGridSizer *grid = new wxFlexGridSizer(2, 4, 4);
    grid->AddGrowableCol(1);

    this->file_type_value = create_value_ctrl(this);
    this->sample_rate_value = create_value_ctrl(this);
    this->bit_depth_value = create_value_ctrl(this);
    this->channels_value = create_value_ctrl(this);
    this->duration_value = create_value_ctrl(this);
    this->nyquist_value = create_value_ctrl(this);
    this->file_size_value = create_value_ctrl(this);
    this->total_samples_value = create_value_ctrl(this);
    this->analysis_frames_value = create_value_ctrl(this);
    this->fft_size_value = create_value_ctrl(this);
    this->freq_resolution_value = create_value_ctrl(this);

    add_row(grid, _("File Type:"), this->file_type_value);
    add_row(grid, _("Sample Rate:"), this->sample_rate_value);
    add_row(grid, _("Bit Depth:"), this->bit_depth_value);
    add_row(grid, _("Channels:"), this->channels_value);
    add_row(grid, _("Duration:"), this->duration_value);
    add_row(grid, _("Nyquist Freq.:"), this->nyquist_value);
    add_row(grid, _("File Size:"), this->file_size_value);
    add_row(grid, _("Total Samples:"), this->total_samples_value);
    add_row(grid, _("Analysis Frames:"), this->analysis_frames_value);
    add_row(grid, _("FFT Size:"), this->fft_size_value);
    add_row(grid, _("Freq. Resolution:"), this->freq_resolution_value);

    outer_sizer->Add(grid, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    this->secondary_title = create_title(this, _("Secondary"));
    this->secondary_title->Hide();
    outer_sizer->Add(this->secondary_title, 0, wxLEFT | wxRIGHT | wxTOP, 8);

    wxFlexGridSizer *grid2 = new wxFlexGridSizer(2, 4, 4);
    grid2->AddGrowableCol(1);

    this->file_type_value2 = create_value_ctrl(this);
    this->sample_rate_value2 = create_value_ctrl(this);
    this->bit_depth_value2 = create_value_ctrl(this);
    this->channels_value2 = create_value_ctrl(this);
    this->duration_value2 = create_value_ctrl(this);
    this->nyquist_value2 = create_value_ctrl(this);
    this->file_size_value2 = create_value_ctrl(this);
    this->total_samples_value2 = create_value_ctrl(this);
    this->analysis_frames_value2 = create_value_ctrl(this);
    this->fft_size_value2 = create_value_ctrl(this);
    this->freq_resolution_value2 = create_value_ctrl(this);

    add_row(grid2, _("File Type:"), this->file_type_value2);
    add_row(grid2, _("Sample Rate:"), this->sample_rate_value2);
    add_row(grid2, _("Bit Depth:"), this->bit_depth_value2);
    add_row(grid2, _("Channels:"), this->channels_value2);
    add_row(grid2, _("Duration:"), this->duration_value2);
    add_row(grid2, _("Nyquist Freq.:"), this->nyquist_value2);
    add_row(grid2, _("File Size:"), this->file_size_value2);
    add_row(grid2, _("Total Samples:"), this->total_samples_value2);
    add_row(grid2, _("Analysis Frames:"), this->analysis_frames_value2);
    add_row(grid2, _("FFT Size:"), this->fft_size_value2);
    add_row(grid2, _("Freq. Resolution:"), this->freq_resolution_value2);

    outer_sizer->Add(grid2, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    SetSizer(outer_sizer);

    SetMinSize(wxSize(260, -1));

    clear();
    clear_secondary();
}

void SpekInfoPanel::add_row(wxFlexGridSizer *sizer, const wxString& label, wxStaticText *value)
{
    wxStaticText *label_ctrl = new wxStaticText(this, -1, label);
    wxFont label_font = label_ctrl->GetFont();
    label_font.SetWeight(wxFONTWEIGHT_BOLD);
    label_ctrl->SetFont(label_font);
    sizer->Add(label_ctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    sizer->Add(value, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxLEFT, 4);
}

wxString SpekInfoPanel::format_duration(double duration) const
{
    int total_ms = (int)(duration * 1000.0);
    int ms = total_ms % 1000;
    int total_seconds = total_ms / 1000;
    int seconds = total_seconds % 60;
    int minutes = total_seconds / 60;
    return wxString::Format("%d:%02d.%03d", minutes, seconds, ms);
}

void SpekInfoPanel::fill_values(
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
    const SpekAudioInfo& info)
{
    wxString file_type = info.file_type;
    if (!file_type.IsEmpty()) {
        file_type.MakeUpper();
    }
    file_type_value->SetLabel(file_type.IsEmpty() ? _("Unknown") : file_type);
    sample_rate_value->SetLabel(wxString::Format(_("%d Hz"), info.sample_rate));
    bit_depth_value->SetLabel(info.bit_depth > 0
        ? wxString::Format(_("%d bits"), info.bit_depth)
        : _("N/A"));
    channels_value->SetLabel(wxString::Format("%d", info.channels));
    duration_value->SetLabel(format_duration(info.duration));
    nyquist_value->SetLabel(wxString::Format(_("%d Hz"), info.nyquist));
    file_size_value->SetLabel(wxString::Format(_("%.2f MB"), info.file_size_mb));
    total_samples_value->SetLabel(wxString::Format("%lld", (long long)info.total_samples));
    analysis_frames_value->SetLabel(wxString::Format("%lld", (long long)info.analysis_frames));
    fft_size_value->SetLabel(wxString::Format("%d", info.fft_size));
    freq_resolution_value->SetLabel(wxString::Format(_("%.2f Hz"), info.freq_resolution));
}

void SpekInfoPanel::set_info(const SpekAudioInfo& info)
{
    fill_values(
        this->file_type_value,
        this->sample_rate_value,
        this->bit_depth_value,
        this->channels_value,
        this->duration_value,
        this->nyquist_value,
        this->file_size_value,
        this->total_samples_value,
        this->analysis_frames_value,
        this->fft_size_value,
        this->freq_resolution_value,
        info);

    Layout();
    GetParent()->Layout();
}

void SpekInfoPanel::clear()
{
    this->file_type_value->SetLabel(wxEmptyString);
    this->sample_rate_value->SetLabel(wxEmptyString);
    this->bit_depth_value->SetLabel(wxEmptyString);
    this->channels_value->SetLabel(wxEmptyString);
    this->duration_value->SetLabel(wxEmptyString);
    this->nyquist_value->SetLabel(wxEmptyString);
    this->file_size_value->SetLabel(wxEmptyString);
    this->total_samples_value->SetLabel(wxEmptyString);
    this->analysis_frames_value->SetLabel(wxEmptyString);
    this->fft_size_value->SetLabel(wxEmptyString);
    this->freq_resolution_value->SetLabel(wxEmptyString);

    Layout();
    GetParent()->Layout();
}

void SpekInfoPanel::set_secondary_info(const SpekAudioInfo& info)
{
    fill_values(
        this->file_type_value2,
        this->sample_rate_value2,
        this->bit_depth_value2,
        this->channels_value2,
        this->duration_value2,
        this->nyquist_value2,
        this->file_size_value2,
        this->total_samples_value2,
        this->analysis_frames_value2,
        this->fft_size_value2,
        this->freq_resolution_value2,
        info);

    this->secondary_title->Show();
    this->file_type_value2->Show();
    this->sample_rate_value2->Show();
    this->bit_depth_value2->Show();
    this->channels_value2->Show();
    this->duration_value2->Show();
    this->nyquist_value2->Show();
    this->file_size_value2->Show();
    this->total_samples_value2->Show();
    this->analysis_frames_value2->Show();
    this->fft_size_value2->Show();
    this->freq_resolution_value2->Show();

    Layout();
    GetParent()->Layout();
}

void SpekInfoPanel::clear_secondary()
{
    this->file_type_value2->SetLabel(wxEmptyString);
    this->sample_rate_value2->SetLabel(wxEmptyString);
    this->bit_depth_value2->SetLabel(wxEmptyString);
    this->channels_value2->SetLabel(wxEmptyString);
    this->duration_value2->SetLabel(wxEmptyString);
    this->nyquist_value2->SetLabel(wxEmptyString);
    this->file_size_value2->SetLabel(wxEmptyString);
    this->total_samples_value2->SetLabel(wxEmptyString);
    this->analysis_frames_value2->SetLabel(wxEmptyString);
    this->fft_size_value2->SetLabel(wxEmptyString);
    this->freq_resolution_value2->SetLabel(wxEmptyString);

    this->secondary_title->Hide();
    this->file_type_value2->Hide();
    this->sample_rate_value2->Hide();
    this->bit_depth_value2->Hide();
    this->channels_value2->Hide();
    this->duration_value2->Hide();
    this->nyquist_value2->Hide();
    this->file_size_value2->Hide();
    this->total_samples_value2->Hide();
    this->analysis_frames_value2->Hide();
    this->fft_size_value2->Hide();
    this->freq_resolution_value2->Hide();

    Layout();
    GetParent()->Layout();
}
