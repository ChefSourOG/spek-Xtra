#pragma once

#include <wx/wx.h>

enum export_format {
    EXPORT_FORMAT_PNG,
    EXPORT_FORMAT_JPEG,
    EXPORT_FORMAT_BMP,
};

struct SpekExportOptions
{
    int width;
    int height;
    enum export_format format;
};

class SpekExportDialog : public wxDialog
{
public:
    SpekExportDialog(wxWindow *parent, const wxString& default_name);

    SpekExportOptions get_options() const;
    wxString get_path() const;

private:
    void on_preset(wxCommandEvent& event);
    void on_custom_changed(wxCommandEvent& event);
    void on_browse(wxCommandEvent& event);
    void update_controls();

    wxTextCtrl *dir_ctrl;
    wxTextCtrl *name_ctrl;
    wxRadioButton *preset_standard;
    wxRadioButton *preset_high;
    wxRadioButton *preset_maximum;
    wxRadioButton *preset_custom;
    wxTextCtrl *width_ctrl;
    wxTextCtrl *height_ctrl;
    wxChoice *format_choice;
    wxCheckBox *aspect_check;

    wxString default_name;

    DECLARE_EVENT_TABLE()
};
