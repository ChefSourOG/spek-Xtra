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
    void add_files_to_queue(const wxArrayString& paths, bool load_first);

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
    void on_show_queue(wxCommandEvent& event);
    void on_compare_mode(wxCommandEvent& event);
    void on_open_secondary(wxCommandEvent& event);
    void on_fit_window(wxCommandEvent& event);
    void on_link_axes(wxCommandEvent& event);
    void on_fft_size(wxCommandEvent& event);
    void on_window_function(wxCommandEvent& event);
    void on_palette(wxCommandEvent& event);
    void on_recent_file(wxCommandEvent& event);
    void on_clear_recent_files(wxCommandEvent& event);
    void on_queue_select(wxCommandEvent& event);
    void on_queue_remove(wxCommandEvent& event);
    void on_queue_clear(wxCommandEvent& event);
    void on_queue_context_menu(wxContextMenuEvent& event);
    void on_queue_open_primary(wxCommandEvent& event);
    void on_queue_open_secondary(wxCommandEvent& event);
    void on_info_sash_changed(wxSplitterEvent& event);
    void on_size(wxSizeEvent& event);
    void populate_recent_files_menu();
    void update_info_panel_visibility();
    void update_info_panel_info();
    void update_queue_visibility();
    void load_file(const wxString& path);
    void load_secondary_file(const wxString& path);
    void set_compare_mode(bool compare);
    void load_queue_item(int index);
    void refresh_queue_list();
    void save_queue();
    void remove_queue_item(int index);
    void clear_queue();

    SpekSpectrogram *spectrogram;
    SpekSpectrogram *spectrogram2;
    SpekInfoPanel *info_panel;
    wxSplitterWindow *splitter;
    wxSplitterWindow *compare_splitter;
    wxSplitterWindow *main_splitter;
    wxPanel *info_bar;
    wxPanel *queue_panel;
    wxPanel *right_panel;
    wxListBox *queue_list;
    wxButton *queue_remove_btn;
    wxButton *queue_clear_btn;
    wxMenuItem *menu_file_export;
    wxMenu *menu_file_recent;
    wxMenuItem *menu_view_info;
    wxMenuItem *menu_view_queue;
    wxMenuItem *menu_view_compare;
    wxMenuItem *menu_view_fit_window;
    wxMenuItem *menu_view_link_axes;
    wxStatusBar *status_bar;
    int info_sash_position;
    int info_panel_width;
    int queue_sash_position;
    wxString path;
    wxString secondary_path;
    wxString pngpath;
    wxString cur_dir;
    wxString description;
    wxArrayString recent_files;
    wxArrayString queue_paths;
    int active_queue_index;

    DECLARE_EVENT_TABLE()
};
