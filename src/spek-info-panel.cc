#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "spek-info-panel.h"

SpekInfoPanel::SpekInfoPanel(wxWindow *parent) :
    wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN)
{
    wxBoxSizer *outer_sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText *title = new wxStaticText(this, -1, _("Audio Info"));
    wxFont title_font = title->GetFont();
    title_font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(title_font);
    outer_sizer->Add(title, 0, wxALL, 8);

    wxFlexGridSizer *grid = new wxFlexGridSizer(2, 4, 4);
    grid->AddGrowableCol(1);

    wxSize value_min_size(120, -1);
    this->file_type_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->sample_rate_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->bit_depth_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->channels_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->duration_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->nyquist_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->file_size_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->total_samples_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->analysis_frames_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->fft_size_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);
    this->freq_resolution_value = new wxStaticText(this, -1, wxEmptyString, wxDefaultPosition, value_min_size);

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

    outer_sizer->Add(grid, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
    SetSizer(outer_sizer);

    SetMinSize(wxSize(260, -1));

    // Set initial placeholder values; no parent layout yet because the panel
    // is still being constructed.
    this->file_type_value->SetLabel(_("—"));
    this->sample_rate_value->SetLabel(_("—"));
    this->bit_depth_value->SetLabel(_("—"));
    this->channels_value->SetLabel(_("—"));
    this->duration_value->SetLabel(_("—"));
    this->nyquist_value->SetLabel(_("—"));
    this->file_size_value->SetLabel(_("—"));
    this->total_samples_value->SetLabel(_("—"));
    this->analysis_frames_value->SetLabel(_("—"));
    this->fft_size_value->SetLabel(_("—"));
    this->freq_resolution_value->SetLabel(_("—"));
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

void SpekInfoPanel::set_info(const SpekAudioInfo& info)
{
    wxString file_type = info.file_type;
    if (!file_type.IsEmpty()) {
        file_type.MakeUpper();
    }
    this->file_type_value->SetLabel(file_type.IsEmpty() ? _("Unknown") : file_type);
    this->sample_rate_value->SetLabel(wxString::Format(_("%d Hz"), info.sample_rate));
    this->bit_depth_value->SetLabel(info.bit_depth > 0
        ? wxString::Format(_("%d bits"), info.bit_depth)
        : _("N/A"));
    this->channels_value->SetLabel(wxString::Format("%d", info.channels));
    this->duration_value->SetLabel(format_duration(info.duration));
    this->nyquist_value->SetLabel(wxString::Format(_("%d Hz"), info.nyquist));
    this->file_size_value->SetLabel(wxString::Format(_("%.2f MB"), info.file_size_mb));
    this->total_samples_value->SetLabel(wxString::Format("%lld", (long long)info.total_samples));
    this->analysis_frames_value->SetLabel(wxString::Format("%lld", (long long)info.analysis_frames));
    this->fft_size_value->SetLabel(wxString::Format("%d", info.fft_size));
    this->freq_resolution_value->SetLabel(wxString::Format(_("%.2f Hz"), info.freq_resolution));

    Layout();
    GetParent()->Layout();
}

void SpekInfoPanel::clear()
{
    this->file_type_value->SetLabel(_("—"));
    this->sample_rate_value->SetLabel(_("—"));
    this->bit_depth_value->SetLabel(_("—"));
    this->channels_value->SetLabel(_("—"));
    this->duration_value->SetLabel(_("—"));
    this->nyquist_value->SetLabel(_("—"));
    this->file_size_value->SetLabel(_("—"));
    this->total_samples_value->SetLabel(_("—"));
    this->analysis_frames_value->SetLabel(_("—"));
    this->fft_size_value->SetLabel(_("—"));
    this->freq_resolution_value->SetLabel(_("—"));

    Layout();
    GetParent()->Layout();
}
