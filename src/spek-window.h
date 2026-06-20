#pragma once

#include <wx/arrstr.h>
#include <wx/wx.h>
#include <wx/splitter.h>

class SpekInfoPanel;
class SpekSpectrogram;

class SpekWindow : public wxFrame
{
public:
    SpekWindow(int width, int height, const wxString& path, const wxString& pngpath);
    void open(const wxString& path);

private:
    void on_open(wxCommandEvent& event);
    void on_export(wxCommandEvent& event);
    void on_exit(wxCommandEvent& event);
    void on_preferences(wxCommandEvent& event);
    void on_help(wxCommandEvent& event);
    void on_about(wxCommandEvent& event);
    void on_notify(wxCommandEvent& event);
    void on_visit(wxCommandEvent& event);
    void on_close(wxCommandEvent& event);
    void on_show_info_panel(wxCommandEvent& event);
    void on_fft_size(wxCommandEvent& event);
    void on_window_function(wxCommandEvent& event);
    void on_palette(wxCommandEvent& event);
    void on_recent_file(wxCommandEvent& event);
    void on_clear_recent_files(wxCommandEvent& event);
    void populate_recent_files_menu();
    void update_info_panel_visibility();
    void update_info_panel_info();

    SpekSpectrogram *spectrogram;
    SpekInfoPanel *info_panel;
    wxSplitterWindow *splitter;
    wxMenuItem *menu_file_export;
    wxMenu *menu_file_recent;
    wxMenuItem *menu_view_info;
    int info_sash_position;
    wxString path;
    wxString pngpath;
    wxString cur_dir;
    wxString description;
    wxArrayString recent_files;

    DECLARE_EVENT_TABLE()
};
