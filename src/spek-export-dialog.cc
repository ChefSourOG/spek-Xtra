#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "spek-export-dialog.h"

#define ID_PRESET_STANDARD (wxID_HIGHEST + 50)
#define ID_PRESET_HIGH (wxID_HIGHEST + 51)
#define ID_PRESET_MAXIMUM (wxID_HIGHEST + 52)
#define ID_PRESET_CUSTOM (wxID_HIGHEST + 53)
#define ID_BROWSE (wxID_HIGHEST + 54)

BEGIN_EVENT_TABLE(SpekExportDialog, wxDialog)
    EVT_RADIOBUTTON(ID_PRESET_STANDARD, SpekExportDialog::on_preset)
    EVT_RADIOBUTTON(ID_PRESET_HIGH, SpekExportDialog::on_preset)
    EVT_RADIOBUTTON(ID_PRESET_MAXIMUM, SpekExportDialog::on_preset)
    EVT_RADIOBUTTON(ID_PRESET_CUSTOM, SpekExportDialog::on_preset)
    EVT_TEXT(wxID_ANY, SpekExportDialog::on_custom_changed)
    EVT_BUTTON(ID_BROWSE, SpekExportDialog::on_browse)
END_EVENT_TABLE()

SpekExportDialog::SpekExportDialog(wxWindow *parent, const wxString& default_name) :
    wxDialog(parent, -1, _("Export Image")), default_name(default_name)
{
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer *inner = new wxBoxSizer(wxVERTICAL);
    sizer->Add(inner, 1, wxALL | wxEXPAND, 12);

    // Split the default path into directory and file name.
    wxString default_dir = wxGetHomeDir();
    wxString default_file = default_name;
    if (wxFileName(default_name).IsAbsolute()) {
        wxFileName fn(default_name);
        default_dir = fn.GetPath();
        default_file = fn.GetFullName();
    } else if (!default_name.IsEmpty()) {
        wxFileName fn(default_name);
        default_file = fn.GetFullName();
    }

    // Save location.
    wxSizer *dir_sizer = new wxBoxSizer(wxHORIZONTAL);
    inner->Add(new wxStaticText(this, -1, _("Save to:")));
    this->dir_ctrl = new wxTextCtrl(this, -1, default_dir);
    dir_sizer->Add(this->dir_ctrl, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    wxButton *browse_btn = new wxButton(this, ID_BROWSE, _("Browse..."));
    dir_sizer->Add(browse_btn, 0, wxALIGN_CENTER_VERTICAL);
    inner->Add(dir_sizer, 0, wxEXPAND | wxTOP, 4);

    // File name.
    wxSizer *name_sizer = new wxBoxSizer(wxHORIZONTAL);
    inner->Add(new wxStaticText(this, -1, _("File name:")), 0, wxTOP, 8);
    this->name_ctrl = new wxTextCtrl(this, -1, default_file);
    name_sizer->Add(this->name_ctrl, 1, wxALIGN_CENTER_VERTICAL);
    inner->Add(name_sizer, 0, wxEXPAND | wxTOP, 4);

    // Resolution presets.
    inner->Add(new wxStaticText(this, -1, _("Resolution:")), 0, wxTOP, 12);
    this->preset_standard = new wxRadioButton(this, ID_PRESET_STANDARD, _("Standard (1100x600)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    this->preset_high = new wxRadioButton(this, ID_PRESET_HIGH, _("High (1650x900)"));
    this->preset_maximum = new wxRadioButton(this, ID_PRESET_MAXIMUM, _("Maximum (2200x1200)"));
    this->preset_custom = new wxRadioButton(this, ID_PRESET_CUSTOM, _("Custom"));
    inner->Add(this->preset_standard, 0, wxTOP, 4);
    inner->Add(this->preset_high, 0, wxTOP, 4);
    inner->Add(this->preset_maximum, 0, wxTOP, 4);
    inner->Add(this->preset_custom, 0, wxTOP, 4);

    wxSizer *size_sizer = new wxBoxSizer(wxHORIZONTAL);
    size_sizer->Add(new wxStaticText(this, -1, _("Width:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    this->width_ctrl = new wxTextCtrl(this, -1, wxT("1100"), wxDefaultPosition, wxSize(60, -1));
    size_sizer->Add(this->width_ctrl, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    size_sizer->Add(new wxStaticText(this, -1, _("Height:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    this->height_ctrl = new wxTextCtrl(this, -1, wxT("600"), wxDefaultPosition, wxSize(60, -1));
    size_sizer->Add(this->height_ctrl, 0, wxALIGN_CENTER_VERTICAL);
    inner->Add(size_sizer, 0, wxLEFT | wxTOP, 24);

    // Format.
    wxSizer *format_sizer = new wxBoxSizer(wxHORIZONTAL);
    format_sizer->Add(new wxStaticText(this, -1, _("Format:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    this->format_choice = new wxChoice(this, -1);
    this->format_choice->Append(_("PNG"));
    this->format_choice->Append(_("JPEG"));
    this->format_choice->Append(_("BMP"));
    this->format_choice->SetSelection(0);
    format_sizer->Add(this->format_choice, 0, wxALIGN_CENTER_VERTICAL);
    inner->Add(format_sizer, 0, wxTOP, 12);

    // Aspect ratio.
    this->aspect_check = new wxCheckBox(this, -1, _("Keep aspect ratio"));
    this->aspect_check->SetValue(true);
    inner->Add(this->aspect_check, 0, wxTOP, 8);

    sizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_RIGHT | wxBOTTOM | wxRIGHT, 12);
    sizer->SetSizeHints(this);
    SetSizer(sizer);

    this->preset_standard->SetValue(true);
    this->update_controls();
}

void SpekExportDialog::on_preset(wxCommandEvent&)
{
    this->update_controls();
}

void SpekExportDialog::on_custom_changed(wxCommandEvent& event)
{
    if (!this->preset_custom->GetValue()) {
        return;
    }
    if (!this->aspect_check->GetValue()) {
        return;
    }

    long width = 1100, height = 600;
    this->width_ctrl->GetValue().ToLong(&width);
    this->height_ctrl->GetValue().ToLong(&height);

    // Maintain the standard 1100:600 (11:6) aspect ratio.
    wxObject *obj = event.GetEventObject();
    if (obj == this->width_ctrl && width > 0) {
        height = (int)(width * 600.0 / 1100.0 + 0.5);
        this->height_ctrl->SetValue(wxString::Format(wxT("%d"), height));
    } else if (obj == this->height_ctrl && height > 0) {
        width = (int)(height * 1100.0 / 600.0 + 0.5);
        this->width_ctrl->SetValue(wxString::Format(wxT("%d"), width));
    }
}

void SpekExportDialog::on_browse(wxCommandEvent&)
{
    wxString wildcard = _("PNG images") + wxString(wxT("|*.png|")) +
                        _("JPEG images") + wxString(wxT("|*.jpg;*.jpeg|")) +
                        _("BMP images") + wxString(wxT("|*.bmp"));

    wxFileName current(this->dir_ctrl->GetValue(), this->name_ctrl->GetValue());
    wxFileDialog dlg(
        this,
        _("Save Image"),
        current.GetPath(),
        current.GetFullName(),
        wildcard,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT
    );
    if (dlg.ShowModal() == wxID_OK) {
        wxFileName fn(dlg.GetPath());
        this->dir_ctrl->SetValue(fn.GetPath());
        this->name_ctrl->SetValue(fn.GetFullName());
    }
}

void SpekExportDialog::update_controls()
{
    bool custom = this->preset_custom->GetValue();
    this->width_ctrl->Enable(custom);
    this->height_ctrl->Enable(custom);
    this->aspect_check->Enable(custom);

    if (!custom) {
        int width = 1100, height = 600;
        if (this->preset_high->GetValue()) {
            width = 1650;
            height = 900;
        } else if (this->preset_maximum->GetValue()) {
            width = 2200;
            height = 1200;
        }
        this->width_ctrl->SetValue(wxString::Format(wxT("%d"), width));
        this->height_ctrl->SetValue(wxString::Format(wxT("%d"), height));
    }
}

SpekExportOptions SpekExportDialog::get_options() const
{
    SpekExportOptions options;
    options.width = 1100;
    options.height = 600;
    long width = options.width;
    long height = options.height;
    this->width_ctrl->GetValue().ToLong(&width);
    this->height_ctrl->GetValue().ToLong(&height);
    options.width = width;
    options.height = height;

    switch (this->format_choice->GetSelection()) {
    case 1:
        options.format = EXPORT_FORMAT_JPEG;
        break;
    case 2:
        options.format = EXPORT_FORMAT_BMP;
        break;
    case 0:
    default:
        options.format = EXPORT_FORMAT_PNG;
        break;
    }
    return options;
}

wxString SpekExportDialog::get_path() const
{
    wxFileName fn(this->dir_ctrl->GetValue(), this->name_ctrl->GetValue());
    return fn.GetFullPath();
}
