#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/button.h>
#include <wx/dnd.h>
#include <wx/filename.h>
#include <wx/listbox.h>
#include <wx/protocol/http.h>
#include <wx/splitter.h>
#include <wx/sstream.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>

// WX on WIN doesn't like it when pthread.h is included first.
#include <pthread.h>

#include <spek-utils.h>

#include "spek-artwork.h"
#include "spek-export-dialog.h"
#include "spek-info-panel.h"
#include "spek-palette.h"
#include "spek-platform.h"
#include "spek-preferences-dialog.h"
#include "spek-preferences.h"
#include "spek-spectrogram.h"

#include "spek-window.h"

wxDEFINE_EVENT(SPEK_NOTIFY_EVENT, wxCommandEvent);

#define ID_VIEW_SHOW_INFO_PANEL (wxID_HIGHEST + 10)
#define ID_FFT_SIZE_BASE (wxID_HIGHEST + 20)
#define ID_WINDOW_FUNCTION_BASE (wxID_HIGHEST + 30)
#define ID_PALETTE_BASE (wxID_HIGHEST + 40)
#define ID_EXPORT_IMAGE (wxID_HIGHEST + 60)
#define ID_RECENT_FILES_BASE (wxID_HIGHEST + 70)
#define ID_CLEAR_RECENT_FILES (wxID_HIGHEST + 80)
#define ID_QUEUE_LIST (wxID_HIGHEST + 90)
#define ID_QUEUE_REMOVE (wxID_HIGHEST + 91)
#define ID_QUEUE_CLEAR (wxID_HIGHEST + 92)
#define ID_VIEW_SHOW_QUEUE (wxID_HIGHEST + 100)
#define ID_VIEW_COMPARE_MODE (wxID_HIGHEST + 110)
#define ID_OPEN_SECONDARY (wxID_HIGHEST + 111)
#define ID_VIEW_FIT_WINDOW (wxID_HIGHEST + 112)
#define ID_VIEW_LINK_AXES (wxID_HIGHEST + 113)
#define ID_VIEW_LOG_FREQ (wxID_HIGHEST + 114)
#define ID_QUEUE_OPEN_PRIMARY (wxID_HIGHEST + 94)
#define ID_QUEUE_OPEN_SECONDARY (wxID_HIGHEST + 93)
#define MAX_RECENT_FILES 10
#define MAX_QUEUE_FILES 100

BEGIN_EVENT_TABLE(SpekWindow, wxFrame)
    EVT_MENU(wxID_OPEN, SpekWindow::on_open)
    EVT_MENU(ID_EXPORT_IMAGE, SpekWindow::on_export)
    EVT_MENU_RANGE(ID_RECENT_FILES_BASE, ID_RECENT_FILES_BASE + MAX_RECENT_FILES - 1, SpekWindow::on_recent_file)
    EVT_MENU(ID_CLEAR_RECENT_FILES, SpekWindow::on_clear_recent_files)
    EVT_MENU(wxID_EXIT, SpekWindow::on_exit)
    EVT_MENU(wxID_PREFERENCES, SpekWindow::on_preferences)
    EVT_MENU(wxID_HELP, SpekWindow::on_help)
    EVT_MENU(wxID_ABOUT, SpekWindow::on_about)
    EVT_MENU(ID_VIEW_SHOW_INFO_PANEL, SpekWindow::on_show_info_panel)
    EVT_MENU(ID_VIEW_SHOW_QUEUE, SpekWindow::on_show_queue)
    EVT_MENU(ID_VIEW_COMPARE_MODE, SpekWindow::on_compare_mode)
    EVT_TOOL(ID_VIEW_COMPARE_MODE, SpekWindow::on_compare_mode)
    EVT_TOOL(ID_OPEN_SECONDARY, SpekWindow::on_open_secondary)
    EVT_MENU(ID_VIEW_FIT_WINDOW, SpekWindow::on_fit_window)
    EVT_TOOL(ID_VIEW_FIT_WINDOW, SpekWindow::on_fit_window)
    EVT_MENU(ID_VIEW_LINK_AXES, SpekWindow::on_link_axes)
    EVT_MENU(ID_VIEW_LOG_FREQ, SpekWindow::on_log_freq)
    EVT_MENU_RANGE(ID_FFT_SIZE_BASE, ID_FFT_SIZE_BASE + 3, SpekWindow::on_fft_size)
    EVT_MENU_RANGE(ID_WINDOW_FUNCTION_BASE, ID_WINDOW_FUNCTION_BASE + 3, SpekWindow::on_window_function)
    EVT_SIZE(SpekWindow::on_size)
    EVT_COMMAND(-1, SPEK_NOTIFY_EVENT, SpekWindow::on_notify)
    EVT_BUTTON(ID_QUEUE_REMOVE, SpekWindow::on_queue_remove)
    EVT_BUTTON(ID_QUEUE_CLEAR, SpekWindow::on_queue_clear)
    EVT_LISTBOX(ID_QUEUE_LIST, SpekWindow::on_queue_select)
    EVT_MENU(ID_QUEUE_OPEN_PRIMARY, SpekWindow::on_queue_open_primary)
    EVT_MENU(ID_QUEUE_OPEN_SECONDARY, SpekWindow::on_queue_open_secondary)
END_EVENT_TABLE()

#ifdef SPEK_CHECK_VERSION
// Forward declarations.
static void * check_version(void *);
#endif

class SpekDropTarget : public wxFileDropTarget
{
public:
    SpekDropTarget(SpekWindow *window) : wxFileDropTarget(), window(window) {}

protected:
    virtual bool OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames){
        if (filenames.GetCount() > 0) {
            window->add_files_to_queue(filenames, true);
            return true;
        }
        return false;
    }

private:
    SpekWindow *window;
};

SpekWindow::SpekWindow(int width, int height, const wxString& path, const wxString& pngpath) :
    wxFrame(NULL, -1, wxEmptyString, wxDefaultPosition, wxSize(width, height)),
    spectrogram(NULL), spectrogram2(NULL), info_panel(NULL), splitter(NULL),
    compare_splitter(NULL), main_splitter(NULL), info_bar(NULL), queue_panel(NULL),
    right_panel(NULL), queue_list(NULL), queue_remove_btn(NULL),
    queue_clear_btn(NULL), menu_file_export(NULL), menu_file_recent(NULL),
    menu_view_info(NULL), menu_view_queue(NULL), menu_view_compare(NULL),
    menu_view_fit_window(NULL), menu_view_link_axes(NULL),
    menu_view_log_freq(NULL), status_bar(NULL), info_sash_position(width * 2),
    info_panel_width(260),
    queue_sash_position(220), path(path),
    secondary_path(wxEmptyString), pngpath(pngpath), cur_dir(wxEmptyString),
    description(wxEmptyString), active_queue_index(-1)
{
    this->description = _("Spek-Xtra - Acoustic Spectrum Analyser");
    SetTitle(_("Spek-Xtra"));

#ifndef OS_OSX
    SetIcons(wxArtProvider::GetIconBundle(ART_SPEK, wxART_FRAME_ICON));
#endif

    SetMinSize(wxSize(700 * spek_platform_dpi_scale(), 500 * spek_platform_dpi_scale()));

    int initial_fft_bits = SpekPreferences::get().get_fft_bits();
    if (initial_fft_bits < 9) {
        initial_fft_bits = 9;
    } else if (initial_fft_bits > 12) {
        initial_fft_bits = 12;
    }

    int initial_window_function = SpekPreferences::get().get_window_function();
    if (initial_window_function < 0 || initial_window_function >= WINDOW_COUNT) {
        initial_window_function = WINDOW_DEFAULT;
    }

    int initial_palette = SpekPreferences::get().get_palette();
    if (initial_palette < 0 || initial_palette >= PALETTE_COUNT) {
        initial_palette = PALETTE_DEFAULT;
    }

    bool initial_show_queue = SpekPreferences::get().get_show_queue();
    bool initial_log_freq = SpekPreferences::get().get_log_freq();

    this->main_splitter = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE);
    this->main_splitter->SetMinimumPaneSize(150);
    this->main_splitter->SetSashGravity(0.0);

    wxMenuBar *menu = new wxMenuBar();

    wxMenu *menu_file = new wxMenu();
    wxMenuItem *menu_file_open = new wxMenuItem(menu_file, wxID_OPEN);
    menu_file->Append(menu_file_open);
    this->menu_file_export = new wxMenuItem(menu_file, ID_EXPORT_IMAGE, _("Export &Image..."));
    this->menu_file_export->SetItemLabel(this->menu_file_export->GetItemLabelText() + "\tCtrl+S");
    menu_file->Append(this->menu_file_export);
    this->menu_file_recent = new wxMenu();
    menu_file->AppendSubMenu(this->menu_file_recent, _("Recent &Files"));
    menu_file->AppendSeparator();
    menu_file->Append(wxID_EXIT);
    menu->Append(menu_file, _("&File"));

    wxMenu *menu_edit = new wxMenu();
    wxMenuItem *menu_edit_prefs = new wxMenuItem(menu_edit, wxID_PREFERENCES);
    menu_edit_prefs->SetItemLabel(menu_edit_prefs->GetItemLabelText() + "\tCtrl-E");
    menu_edit->Append(menu_edit_prefs);
    menu->Append(menu_edit, _("&Edit"));

    wxMenu *menu_view = new wxMenu();
    this->menu_view_info = new wxMenuItem(
        menu_view, ID_VIEW_SHOW_INFO_PANEL, _("Show &Audio Info"),
        wxEmptyString, wxITEM_CHECK);
    this->menu_view_info->SetItemLabel(this->menu_view_info->GetItemLabelText() + "\tCtrl-I");
    menu_view->Append(this->menu_view_info);
    this->menu_view_queue = new wxMenuItem(
        menu_view, ID_VIEW_SHOW_QUEUE, _("Show &Queue"),
        wxEmptyString, wxITEM_CHECK);
    menu_view->Append(this->menu_view_queue);
    this->menu_view_compare = new wxMenuItem(
        menu_view, ID_VIEW_COMPARE_MODE, _("&Compare Mode"),
        wxEmptyString, wxITEM_CHECK);
    menu_view->Append(this->menu_view_compare);
    this->menu_view_fit_window = new wxMenuItem(
        menu_view, ID_VIEW_FIT_WINDOW, _("&Fit to Window"),
        wxEmptyString, wxITEM_NORMAL);
    menu_view->Append(this->menu_view_fit_window);
    this->menu_view_link_axes = new wxMenuItem(
        menu_view, ID_VIEW_LINK_AXES, _("&Link Axes"),
        wxEmptyString, wxITEM_CHECK);
    menu_view->Append(this->menu_view_link_axes);
    this->menu_view_link_axes->Check(true);
    this->menu_view_log_freq = new wxMenuItem(
        menu_view, ID_VIEW_LOG_FREQ, _("&Logarithmic Frequency Scale"),
        wxEmptyString, wxITEM_CHECK);
    menu_view->Append(this->menu_view_log_freq);
    this->menu_view_log_freq->Check(initial_log_freq);
    menu_view->AppendSeparator();

    wxMenu *menu_fft_size = new wxMenu();
    menu_fft_size->AppendRadioItem(ID_FFT_SIZE_BASE + 0, _("512"));
    menu_fft_size->AppendRadioItem(ID_FFT_SIZE_BASE + 1, _("1024"));
    menu_fft_size->AppendRadioItem(ID_FFT_SIZE_BASE + 2, _("2048"));
    menu_fft_size->AppendRadioItem(ID_FFT_SIZE_BASE + 3, _("4096"));
    menu_view->AppendSubMenu(menu_fft_size, _("FFT &Size"));

    wxMenu *menu_window_function = new wxMenu();
    menu_window_function->AppendRadioItem(ID_WINDOW_FUNCTION_BASE + 0, _("&Hann"));
    menu_window_function->AppendRadioItem(ID_WINDOW_FUNCTION_BASE + 1, _("&Hamming"));
    menu_window_function->AppendRadioItem(ID_WINDOW_FUNCTION_BASE + 2, _("&Blackman"));
    menu_window_function->AppendRadioItem(ID_WINDOW_FUNCTION_BASE + 3, _("&Rectangular"));
    menu_view->AppendSubMenu(menu_window_function, _("&Window Function"));

    menu_fft_size->Check(ID_FFT_SIZE_BASE + initial_fft_bits - 9, true);
    menu_window_function->Check(ID_WINDOW_FUNCTION_BASE + initial_window_function, true);

    menu->Append(menu_view, _("&View"));

    wxMenu *menu_help = new wxMenu();
    wxMenuItem *menu_help_contents = new wxMenuItem(
        menu_help, wxID_HELP, wxString(_("&Help")) + "\tF1");
    menu_help->Append(menu_help_contents);
    wxMenuItem *menu_help_about = new wxMenuItem(menu_help, wxID_ABOUT);
    menu_help_about->SetItemLabel(menu_help_about->GetItemLabelText() + "\tShift-F1");
    menu_help->Append(menu_help_about);
    menu->Append(menu_help, _("&Help"));

    SetMenuBar(menu);

    wxToolBar *toolbar = CreateToolBar();
    toolbar->AddTool(
        wxID_OPEN,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_OPEN, wxART_TOOLBAR),
        menu_file_open->GetItemLabelText()
    );
    toolbar->AddTool(
        ID_EXPORT_IMAGE,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_SAVE, wxART_TOOLBAR),
        menu_file_export->GetItemLabelText()
    );
    toolbar->AddCheckTool(
        ID_VIEW_COMPARE_MODE,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_COPY, wxART_TOOLBAR),
        wxNullBitmap,
        _("Show two spectrograms side-by-side")
    );
    toolbar->AddTool(
        ID_VIEW_FIT_WINDOW,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_HOME, wxART_TOOLBAR),
        _("Reset zoom to fit the full file")
    );
    toolbar->AddTool(
        ID_OPEN_SECONDARY,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_FILE_OPEN, wxART_TOOLBAR),
        _("Open a file in the secondary panel")
    );
    toolbar->AddStretchableSpace();
    toolbar->AddTool(
        wxID_HELP,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_HELP, wxART_TOOLBAR),
        _("Help")
    );
    toolbar->Realize();

    this->right_panel = new wxPanel(this->main_splitter);

    wxBoxSizer *palette_sizer = new wxBoxSizer(wxHORIZONTAL);
    const char *palette_labels[] = {
        N_("Spectrum"),
        N_("SoX"),
        N_("Green"),
        N_("Rainbow"),
        N_("Teal"),
        N_("Heat"),
        N_("Ice"),
        N_("Grayscale"),
    };
    for (int i = 0; i < PALETTE_COUNT; ++i) {
        wxBitmapButton *btn = new wxBitmapButton(
            this->right_panel, ID_PALETTE_BASE + i,
            spek_palette_bitmap((enum palette)i, 32, 16),
            wxDefaultPosition, wxSize(40, 24),
            wxBU_AUTODRAW | wxBORDER_RAISED);
        btn->SetToolTip(wxGetTranslation(palette_labels[i]));
        palette_sizer->Add(btn, 0, wxALL, 2);
        Connect(
            ID_PALETTE_BASE + i,
            wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(SpekWindow::on_palette));
    }
    palette_sizer->AddStretchSpacer(1);

    // wxInfoBar is too limited, construct a custom one.
    this->info_bar = new wxPanel(this->right_panel);
    this->info_bar->Hide();
    this->info_bar->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    this->info_bar->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    wxSizer *info_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *label = new wxStaticText(
        this->info_bar, -1, _("A new version of Spek is available, click to download."));
    label->SetCursor(*new wxCursor(wxCURSOR_HAND));
    label->Connect(wxEVT_LEFT_DOWN, wxCommandEventHandler(SpekWindow::on_visit));
    // This second Connect() handles clicks on the border
    this->info_bar->Connect(wxEVT_LEFT_DOWN, wxCommandEventHandler(SpekWindow::on_visit));
    info_sizer->Add(label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 6);
    wxBitmapButton *button = new wxBitmapButton(
        this->info_bar, -1, wxArtProvider::GetBitmap(ART_CLOSE, wxART_BUTTON),
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SpekWindow::on_close));
    info_sizer->Add(button, 0, wxALIGN_CENTER_VERTICAL);
    this->info_bar->SetSizer(info_sizer);

    this->queue_panel = new wxPanel(this->main_splitter);
    this->queue_panel->SetMinSize(wxSize(150, -1));
    if (!initial_show_queue) {
        this->queue_panel->Hide();
    }

    wxBoxSizer *queue_sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText *queue_label = new wxStaticText(this->queue_panel, -1, _("&Queue"));
    queue_sizer->Add(queue_label, 0, wxALL, 4);
    this->queue_list = new wxListBox(
        this->queue_panel, ID_QUEUE_LIST,
        wxDefaultPosition, wxDefaultSize,
        0, NULL, wxLB_SINGLE);
    this->queue_list->Connect(
        wxEVT_CONTEXT_MENU,
        wxContextMenuEventHandler(SpekWindow::on_queue_context_menu),
        NULL, this);
    queue_sizer->Add(this->queue_list, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);

    wxBoxSizer *queue_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    this->queue_remove_btn = new wxButton(this->queue_panel, ID_QUEUE_REMOVE, _("&Remove"));
    this->queue_clear_btn = new wxButton(this->queue_panel, ID_QUEUE_CLEAR, _("&Clear"));
    queue_btn_sizer->Add(this->queue_remove_btn, 1, wxEXPAND | wxRIGHT, 2);
    queue_btn_sizer->Add(this->queue_clear_btn, 1, wxEXPAND | wxLEFT, 2);
    queue_sizer->Add(queue_btn_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);
    this->queue_panel->SetSizer(queue_sizer);

    this->splitter = new wxSplitterWindow(this->right_panel, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE);
    this->splitter->SetSashGravity(1.0);
    this->splitter->Connect(
        wxEVT_SPLITTER_SASH_POS_CHANGED,
        wxSplitterEventHandler(SpekWindow::on_info_sash_changed),
        NULL, this);
    this->compare_splitter = new wxSplitterWindow(this->splitter, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE);
    this->compare_splitter->SetSashGravity(0.5);
    this->status_bar = CreateStatusBar();

    this->spectrogram = new SpekSpectrogram(this->compare_splitter);
    this->spectrogram->set_fft_bits(initial_fft_bits);
    this->spectrogram->set_window_function((enum window_function)initial_window_function);
    this->spectrogram->set_palette((enum palette)initial_palette);
    this->spectrogram->set_log_freq(initial_log_freq);
    this->spectrogram->set_status_bar(this->status_bar);
    this->spectrogram2 = new SpekSpectrogram(this->compare_splitter);
    this->spectrogram2->set_fft_bits(initial_fft_bits);
    this->spectrogram2->set_window_function((enum window_function)initial_window_function);
    this->spectrogram2->set_palette((enum palette)initial_palette);
    this->spectrogram2->set_log_freq(initial_log_freq);
    this->spectrogram2->set_status_bar(this->status_bar);
    this->spectrogram2->Hide();
    this->compare_splitter->Initialize(this->spectrogram);
    this->compare_splitter->SetMinimumPaneSize(100);
    this->info_panel = new SpekInfoPanel(this->splitter);
    this->splitter->SetMinimumPaneSize(260);
    this->splitter->Initialize(this->compare_splitter);

    wxBoxSizer *right_sizer = new wxBoxSizer(wxVERTICAL);
    right_sizer->Add(this->info_bar, 0, wxEXPAND);
    right_sizer->Add(palette_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 4);
    right_sizer->Add(this->splitter, 1, wxEXPAND);
    this->right_panel->SetSizer(right_sizer);

    if (initial_show_queue) {
        this->main_splitter->SplitVertically(this->queue_panel, this->right_panel, this->queue_sash_position);
    } else {
        this->main_splitter->Initialize(this->right_panel);
    }

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(this->main_splitter, 1, wxEXPAND);

    SetSizer(main_sizer);

    this->Layout();
    this->update_info_panel_visibility();
    this->update_queue_visibility();

    this->cur_dir = wxGetHomeDir();

    this->recent_files = SpekPreferences::get().get_recent_files();
    this->populate_recent_files_menu();

    this->queue_paths = SpekPreferences::get().get_queue();
    this->refresh_queue_list();

    if (!path.IsEmpty()) {
        open(path);
    }
    this->menu_file_export->Enable(!this->path.IsEmpty());

    SetDropTarget(new SpekDropTarget(this));

#ifdef SPEK_CHECK_VERSION
    pthread_t thread;
    pthread_create(&thread, NULL, &check_version, this);
#endif
}

void SpekWindow::open(const wxString& path)
{
    wxArrayString paths;
    paths.Add(path);
    add_files_to_queue(paths, true);
}

void SpekWindow::load_file(const wxString& path)
{
    wxFileName file_name(path);
    if (file_name.FileExists()) {
        file_name.Normalize(wxPATH_NORM_ABSOLUTE);
        wxString absolute_path = file_name.GetFullPath();
        this->path = absolute_path;
        wxString full_name = file_name.GetFullName();
        // TRANSLATORS: window title, %s is replaced with the file name
        wxString title = wxString::Format(_("Spek-Xtra - %s"), full_name.c_str());
        SetTitle(title);

        this->spectrogram->open(absolute_path, this->pngpath);

        this->update_info_panel_info();

        if (this->menu_file_export) {
            this->menu_file_export->Enable(true);
        }

        SpekPreferences::get().add_recent_file(absolute_path);
        this->recent_files = SpekPreferences::get().get_recent_files();
        this->populate_recent_files_menu();
    }
}

void SpekWindow::add_files_to_queue(const wxArrayString& paths, bool load_first)
{
    wxArrayString added;
    for (size_t i = 0; i < paths.GetCount(); ++i) {
        wxFileName file_name(paths[i]);
        if (!file_name.FileExists()) {
            continue;
        }
        file_name.Normalize(wxPATH_NORM_ABSOLUTE);
        wxString absolute_path = file_name.GetFullPath();

        int index = this->queue_paths.Index(absolute_path);
        if (index != wxNOT_FOUND) {
            this->queue_paths.RemoveAt(index);
        }
        this->queue_paths.Add(absolute_path);
        added.Add(absolute_path);
    }

    while ((int)this->queue_paths.GetCount() > MAX_QUEUE_FILES) {
        this->queue_paths.RemoveAt(0);
    }

    if (this->queue_paths.IsEmpty()) {
        this->active_queue_index = -1;
    }

    // Remove any duplicates created by re-adding existing files.
    for (size_t i = 0; i < this->queue_paths.GetCount(); ++i) {
        for (size_t j = i + 1; j < this->queue_paths.GetCount(); ) {
            if (this->queue_paths[i] == this->queue_paths[j]) {
                this->queue_paths.RemoveAt(j);
            } else {
                ++j;
            }
        }
    }

    this->refresh_queue_list();
    this->save_queue();

    if (load_first && !added.IsEmpty()) {
        int index = this->queue_paths.Index(added[0]);
        if (index != wxNOT_FOUND) {
            this->load_queue_item(index);
        }
    }
}

void SpekWindow::load_queue_item(int index)
{
    if (index < 0 || index >= (int)this->queue_paths.GetCount()) {
        return;
    }
    this->active_queue_index = index;
    if (this->queue_list) {
        this->queue_list->SetSelection(index);
    }
    this->load_file(this->queue_paths[index]);
}

void SpekWindow::refresh_queue_list()
{
    if (!this->queue_list) {
        return;
    }

    this->queue_list->Freeze();
    this->queue_list->Clear();
    for (size_t i = 0; i < this->queue_paths.GetCount(); ++i) {
        wxFileName file_name(this->queue_paths[i]);
        this->queue_list->Append(file_name.GetFullName());
    }

    if (this->active_queue_index >= 0 && this->active_queue_index < (int)this->queue_paths.GetCount()) {
        this->queue_list->SetSelection(this->active_queue_index);
    } else {
        this->active_queue_index = -1;
    }
    this->queue_list->Thaw();

    if (this->queue_remove_btn) {
        this->queue_remove_btn->Enable(this->queue_list->GetSelection() != wxNOT_FOUND);
    }
    if (this->queue_clear_btn) {
        this->queue_clear_btn->Enable(!this->queue_paths.IsEmpty());
    }
}

void SpekWindow::save_queue()
{
    SpekPreferences::get().set_queue(this->queue_paths);
}

void SpekWindow::remove_queue_item(int index)
{
    if (index < 0 || index >= (int)this->queue_paths.GetCount()) {
        return;
    }

    bool was_active = (index == this->active_queue_index);
    this->queue_paths.RemoveAt(index);

    if (was_active) {
        if (index < (int)this->queue_paths.GetCount()) {
            this->active_queue_index = index;
        } else if (!this->queue_paths.IsEmpty()) {
            this->active_queue_index = (int)this->queue_paths.GetCount() - 1;
        } else {
            this->active_queue_index = -1;
        }
        if (this->active_queue_index >= 0) {
            this->load_queue_item(this->active_queue_index);
        }
    } else if (this->active_queue_index > index) {
        this->active_queue_index--;
    }

    this->refresh_queue_list();
    this->save_queue();

    if (this->queue_paths.IsEmpty()) {
        this->path.Empty();
        SetTitle(_("Spek-Xtra"));
        if (this->menu_file_export) {
            this->menu_file_export->Enable(false);
        }
        if (this->spectrogram) {
            this->spectrogram->open(wxEmptyString, wxEmptyString);
        }
    }
}

void SpekWindow::clear_queue()
{
    this->queue_paths.Clear();
    this->active_queue_index = -1;
    this->refresh_queue_list();
    this->save_queue();

    this->path.Empty();
    SetTitle(_("Spek-Xtra"));
    if (this->menu_file_export) {
        this->menu_file_export->Enable(false);
    }
    if (this->spectrogram) {
        this->spectrogram->open(wxEmptyString, wxEmptyString);
    }
}

// TODO: s/audio/media/
static const char *audio_extensions[] = {
    "3gp",
    "aac",
    "ac3",
    "aif",
    "aifc",
    "aiff",
    "amr",
    "awb",
    "ape",
    "au",
    "dts",
    "flac",
    "flv",
    "gsm",
    "m4a",
    "m4p",
    "m4r",
    "mka",
    "mp3",
    "mp4",
    "mp+",
    "mpc",
    "mpp",
    "oga",
    "ogg",
    "opus",
    "ra",
    "ram",
    "snd",
    "wav",
    "wma",
    "wv",
    NULL
};

static wxString build_audio_filters()
{
    wxString filters;
    filters.Alloc(1024);
    filters += _("All files");
    filters += "|*.*|";
    filters += _("Audio files");
    filters += "|";
    for (int i = 0; audio_extensions[i]; ++i) {
        if (i) {
            filters += ";";
        }
        filters += "*.";
        filters += wxString::FromAscii(audio_extensions[i]);
    }
    filters.Shrink();
    return filters;
}

void SpekWindow::populate_recent_files_menu()
{
    if (!this->menu_file_recent) {
        return;
    }

    while (this->menu_file_recent->GetMenuItemCount() > 0) {
        this->menu_file_recent->Destroy(this->menu_file_recent->FindItemByPosition(0));
    }

    if (this->recent_files.IsEmpty()) {
        wxMenuItem *item = this->menu_file_recent->Append(-1, _("No Recent Files"));
        item->Enable(false);
    } else {
        for (size_t i = 0; i < this->recent_files.GetCount(); ++i) {
            this->menu_file_recent->Append(ID_RECENT_FILES_BASE + i, this->recent_files[i]);
        }
        this->menu_file_recent->AppendSeparator();
        this->menu_file_recent->Append(ID_CLEAR_RECENT_FILES, _("Clear Recent Files"));
    }
}

void SpekWindow::on_recent_file(wxCommandEvent& event)
{
    int index = event.GetId() - ID_RECENT_FILES_BASE;
    if (index >= 0 && index < (int)this->recent_files.GetCount()) {
        this->open(this->recent_files[index]);
    }
}

void SpekWindow::on_clear_recent_files(wxCommandEvent&)
{
    SpekPreferences::get().clear_recent_files();
    this->recent_files.Clear();
    this->populate_recent_files_menu();
}

void SpekWindow::on_open(wxCommandEvent&)
{
    static wxString filters = build_audio_filters();
    static int filter_index = 1;

    wxFileDialog *dlg = new wxFileDialog(
        this,
        _("Open File"),
        this->cur_dir,
        wxEmptyString,
        filters,
        wxFD_OPEN | wxFD_MULTIPLE
    );
    dlg->SetFilterIndex(filter_index);

    if (dlg->ShowModal() == wxID_OK) {
        this->cur_dir = dlg->GetDirectory();
        filter_index = dlg->GetFilterIndex();
        wxArrayString paths;
        dlg->GetPaths(paths);
        add_files_to_queue(paths, true);
    }

    dlg->Destroy();
}

void SpekWindow::on_open_secondary(wxCommandEvent&)
{
    static wxString filters = build_audio_filters();
    static int filter_index = 1;

    wxFileDialog *dlg = new wxFileDialog(
        this,
        _("Open Comparison File"),
        this->cur_dir,
        wxEmptyString,
        filters,
        wxFD_OPEN | wxFD_FILE_MUST_EXIST
    );
    dlg->SetFilterIndex(filter_index);

    if (dlg->ShowModal() == wxID_OK) {
        this->cur_dir = dlg->GetDirectory();
        filter_index = dlg->GetFilterIndex();
        this->load_secondary_file(dlg->GetPath());
    }

    dlg->Destroy();
}

void SpekWindow::on_export(wxCommandEvent&)
{
    wxString default_name = _("Untitled.png");
    if (!this->path.IsEmpty()) {
        wxFileName file_name(this->path);
        default_name = file_name.GetFullName() + wxT(".png");
    }

    SpekExportDialog dlg(this, default_name);
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    SpekExportOptions options = dlg.get_options();
    wxString path = dlg.get_path();
    if (path.IsEmpty()) {
        return;
    }

    wxBitmap bitmap = this->spectrogram->render_export(options.width, options.height);

    wxBitmapType type = wxBITMAP_TYPE_PNG;
    switch (options.format) {
    case EXPORT_FORMAT_JPEG:
        type = wxBITMAP_TYPE_JPEG;
        break;
    case EXPORT_FORMAT_BMP:
        type = wxBITMAP_TYPE_BMP;
        break;
    case EXPORT_FORMAT_PNG:
    default:
        type = wxBITMAP_TYPE_PNG;
        break;
    }

    if (!bitmap.SaveFile(path, type)) {
        wxMessageBox(
            wxString::Format(_("Failed to save image to %s"), path.c_str()),
            _("Export Error"),
            wxOK | wxICON_ERROR,
            this
        );
    }
}

void SpekWindow::on_exit(wxCommandEvent&)
{
    Close(true);
}

void SpekWindow::on_preferences(wxCommandEvent&)
{
    SpekPreferencesDialog dlg(this);
    dlg.ShowModal();
}

void SpekWindow::on_help(wxCommandEvent&)
{
    wxLaunchDefaultBrowser(
        wxString::Format("https://github.com/MikeWang000000/spek-X/blob/v%s/MANUAL.md", PACKAGE_VERSION)
    );
}

void SpekWindow::on_about(wxCommandEvent&)
{
    wxAboutDialogInfo info;
    info.AddDeveloper("Alexander Kojevnikov");
    info.AddDeveloper("Andreas Cadhalpun");
    info.AddDeveloper("Colin Watson");
    info.AddDeveloper("Daniel Hams");
    info.AddDeveloper("Elias Ojala");
    info.AddDeveloper("Fabian Deutsch");
    info.AddDeveloper("Guillaume Fourrier");
    info.AddDeveloper("Jakov Smolic");
    info.AddDeveloper("Jonathan Gonzalez V");
    info.AddDeveloper("Matteo Bini");
    info.AddDeveloper("Mike Wang");
    info.AddDeveloper("Simon Ruderich");
    info.AddDeveloper("Stefan Kost");
    info.AddDeveloper("Thibault North");
    info.AddDeveloper("Wyatt J. Brown");
    info.AddArtist("Olga Vasylevska");
    // TRANSLATORS: Add your name here
    wxString translator = _("translator-credits");
    if (translator != "translator-credits") {
        info.AddTranslator(translator);
    }
    info.SetName("Spek-Xtra");
    info.SetVersion(PACKAGE_VERSION);
    info.SetCopyright(_("Copyright (c) 2010-2013 Alexander Kojevnikov and contributors"));
    info.SetDescription(this->description);
#ifdef OS_UNIX
    info.SetWebSite(
        wxString::Format("https://github.com/MikeWang000000/spek-X/blob/v%s/MANUAL.md", PACKAGE_VERSION),
        _("Spek Website")
    );
    info.SetIcon(wxArtProvider::GetIcon("spek", wxART_OTHER, wxSize(128, 128)));
#endif
    wxAboutBox(info);
}

void SpekWindow::on_notify(wxCommandEvent&)
{
    if (this->info_bar) {
        this->info_bar->Show();
        this->Layout();
    }
}

void SpekWindow::on_visit(wxCommandEvent&)
{
    wxLaunchDefaultBrowser("https://github.com/MikeWang000000/spek-X/releases");
}

void SpekWindow::on_close(wxCommandEvent&)
{
    if (this->info_bar) {
        this->info_bar->Hide();
        this->Layout();
    }
}

void SpekWindow::on_show_info_panel(wxCommandEvent& event)
{
    SpekPreferences::get().set_show_info_panel(event.IsChecked());
    this->update_info_panel_visibility();
}

void SpekWindow::on_show_queue(wxCommandEvent& event)
{
    SpekPreferences::get().set_show_queue(event.IsChecked());
    this->update_queue_visibility();
}

void SpekWindow::on_compare_mode(wxCommandEvent& event)
{
    this->set_compare_mode(event.IsChecked());
}

void SpekWindow::on_fit_window(wxCommandEvent&)
{
    if (this->spectrogram) {
        this->spectrogram->reset_view();
    }
    if (this->spectrogram2) {
        this->spectrogram2->reset_view();
    }
}

void SpekWindow::on_link_axes(wxCommandEvent& event)
{
    bool linked = event.IsChecked();
    if (this->spectrogram) {
        this->spectrogram->set_axes_linked(linked);
    }
    if (this->spectrogram2) {
        this->spectrogram2->set_axes_linked(linked);
    }
}

void SpekWindow::on_log_freq(wxCommandEvent& event)
{
    bool log_freq = event.IsChecked();
    SpekPreferences::get().set_log_freq(log_freq);
    if (this->spectrogram) {
        this->spectrogram->set_log_freq(log_freq);
    }
    if (this->spectrogram2) {
        this->spectrogram2->set_log_freq(log_freq);
    }
}

void SpekWindow::set_compare_mode(bool compare)
{
    if (this->menu_view_compare) {
        this->menu_view_compare->Check(compare);
    }
    wxToolBar *toolbar = this->GetToolBar();
    if (toolbar) {
        toolbar->ToggleTool(ID_VIEW_COMPARE_MODE, compare);
    }

    if (!this->compare_splitter) {
        return;
    }

    if (compare) {
        if (!this->compare_splitter->IsSplit()) {
            int sash = this->compare_splitter->GetClientSize().GetWidth() / 2;
            if (sash < 100) {
                sash = 100;
            }
            this->spectrogram2->Show();
            this->compare_splitter->SplitVertically(this->spectrogram, this->spectrogram2, sash);
        }
    } else {
        if (this->compare_splitter->IsSplit()) {
            this->compare_splitter->Unsplit(this->spectrogram2);
            this->spectrogram2->Hide();
        }
    }

    this->update_info_panel_info();
    this->Layout();
}

void SpekWindow::load_secondary_file(const wxString& path)
{
    wxFileName file_name(path);
    if (file_name.FileExists()) {
        file_name.Normalize(wxPATH_NORM_ABSOLUTE);
        wxString absolute_path = file_name.GetFullPath();
        this->secondary_path = absolute_path;
        this->spectrogram2->open(absolute_path, wxEmptyString);
        this->update_info_panel_info();

        if (!this->menu_view_compare->IsChecked()) {
            this->set_compare_mode(true);
        }
    }
}

void SpekWindow::on_queue_context_menu(wxContextMenuEvent& event)
{
    int index = wxNOT_FOUND;
    wxPoint pos = event.GetPosition();
    if (pos != wxDefaultPosition) {
        index = this->queue_list->HitTest(this->queue_list->ScreenToClient(pos));
    } else {
        index = this->queue_list->GetSelection();
    }
    if (index == wxNOT_FOUND) {
        return;
    }
    this->queue_list->SetSelection(index);

    wxMenu menu;
    menu.Append(ID_QUEUE_OPEN_PRIMARY, _("Open in Primary Panel"));
    menu.Append(ID_QUEUE_OPEN_SECONDARY, _("Open in Secondary Panel"));
    this->PopupMenu(&menu);
}

void SpekWindow::on_queue_open_primary(wxCommandEvent&)
{
    int index = this->queue_list->GetSelection();
    if (index != wxNOT_FOUND) {
        this->load_queue_item(index);
    }
}

void SpekWindow::on_queue_open_secondary(wxCommandEvent&)
{
    int index = this->queue_list->GetSelection();
    if (index != wxNOT_FOUND) {
        this->load_secondary_file(this->queue_paths[index]);
    }
}

void SpekWindow::on_info_sash_changed(wxSplitterEvent&)
{
    if (this->splitter && this->splitter->IsSplit()) {
        this->info_sash_position = this->splitter->GetSashPosition();
        this->info_panel_width = this->splitter->GetSize().GetWidth() - this->info_sash_position;
        if (this->info_panel_width < 260) {
            this->info_panel_width = 260;
        }
    }
}

void SpekWindow::on_size(wxSizeEvent& event)
{
    event.Skip();
    if (this->splitter && this->splitter->IsSplit()) {
        this->CallAfter([this]() {
            if (!this->splitter || !this->splitter->IsSplit()) {
                return;
            }
            int splitter_width = this->splitter->GetSize().GetWidth();
            int sash = splitter_width - this->info_panel_width;
            if (sash < 260) {
                sash = 260;
            }
            this->splitter->SetSashPosition(sash);
        });
    }
}

void SpekWindow::update_info_panel_visibility()
{
    bool show = SpekPreferences::get().get_show_info_panel();
    if (this->menu_view_info) {
        this->menu_view_info->Check(show);
    }

    if (!this->splitter) {
        return;
    }

    if (show) {
        if (!this->splitter->IsSplit()) {
            int splitter_width = this->splitter->GetSize().GetWidth();
            int sash = splitter_width - this->info_panel_width;
            if (sash < 260) {
                sash = 260;
            }
            this->splitter->SplitVertically(this->compare_splitter, this->info_panel, sash);
        }
    } else {
        if (this->splitter->IsSplit()) {
            this->info_sash_position = this->splitter->GetSashPosition();
            this->splitter->Unsplit(this->info_panel);
        }
    }
}

void SpekWindow::update_queue_visibility()
{
    bool show = SpekPreferences::get().get_show_queue();
    if (this->menu_view_queue) {
        this->menu_view_queue->Check(show);
    }

    if (!this->main_splitter || !this->queue_panel || !this->right_panel) {
        return;
    }

    if (show) {
        if (!this->main_splitter->IsSplit()) {
            this->main_splitter->SplitVertically(this->queue_panel, this->right_panel, this->queue_sash_position);
        }
    } else {
        if (this->main_splitter->IsSplit()) {
            this->queue_sash_position = this->main_splitter->GetSashPosition();
            this->main_splitter->Unsplit(this->queue_panel);
        }
    }
}

void SpekWindow::on_fft_size(wxCommandEvent& event)
{
    int bits = 9 + (event.GetId() - ID_FFT_SIZE_BASE);
    SpekPreferences::get().set_fft_bits(bits);
    this->spectrogram->set_fft_bits(bits);
    if (this->spectrogram2) {
        this->spectrogram2->set_fft_bits(bits);
    }
    this->update_info_panel_info();
}

void SpekWindow::on_window_function(wxCommandEvent& event)
{
    enum window_function f = (enum window_function)(event.GetId() - ID_WINDOW_FUNCTION_BASE);
    SpekPreferences::get().set_window_function(f);
    this->spectrogram->set_window_function(f);
    if (this->spectrogram2) {
        this->spectrogram2->set_window_function(f);
    }
}

void SpekWindow::on_palette(wxCommandEvent& event)
{
    enum palette p = (enum palette)(event.GetId() - ID_PALETTE_BASE);
    SpekPreferences::get().set_palette(p);
    this->spectrogram->set_palette(p);
    if (this->spectrogram2) {
        this->spectrogram2->set_palette(p);
    }
}

void SpekWindow::on_queue_select(wxCommandEvent&)
{
    int index = this->queue_list->GetSelection();
    if (index != wxNOT_FOUND) {
        this->load_queue_item(index);
    }
}

void SpekWindow::on_queue_remove(wxCommandEvent&)
{
    int index = this->queue_list->GetSelection();
    if (index != wxNOT_FOUND) {
        this->remove_queue_item(index);
    }
}

void SpekWindow::on_queue_clear(wxCommandEvent&)
{
    this->clear_queue();
}

void SpekWindow::update_info_panel_info()
{
    if (this->info_panel && this->spectrogram) {
        SpekAudioInfo info;
        this->spectrogram->get_info(info);
        this->info_panel->set_info(info);

        if (this->menu_view_compare && this->menu_view_compare->IsChecked() &&
            this->spectrogram2 && !this->secondary_path.IsEmpty()) {
            SpekAudioInfo info2;
            this->spectrogram2->get_info(info2);
            this->info_panel->set_secondary_info(info2);
        } else {
            this->info_panel->clear_secondary();
        }
    }
}

#ifdef SPEK_CHECK_VERSION
static void * check_version(void *p)
{
    // Does the user want to check for updates?
    SpekPreferences& prefs = SpekPreferences::get();
    if (!prefs.get_check_update()) {
        return NULL;
    }

    // Calculate the number of days since 0001-01-01, borrowed from GLib.
    wxDateTime now = wxDateTime::Now();
    int year = now.GetYear() - 1;
    int days = year * 365;
    days += (year >>= 2); // divide by 4 and add
    days -= (year /= 25); // divides original # years by 100
    days += year >> 2; // divides by 4, which divides original by 400
    days += now.GetDayOfYear();

    // When was the last update?
    int diff = days - prefs.get_last_update();
    if (diff < 7) {
        return NULL;
    }

    // Get the version number.
    wxString version;
    wxHTTP http;
    if (http.Connect("morten.with.de")) {
        wxInputStream *stream = http.GetInputStream("/spek");
        if (stream) {
            wxStringOutputStream out(&version);
            stream->Read(out);
            version.Trim();
            delete stream;
        }
    }

    if (version.IsEmpty()) {
        return NULL;
    }

    if (1 == spek_vercmp(version.mb_str(wxConvLibc), PACKAGE_VERSION)) {
        SpekWindow *self = (SpekWindow *)p;
        wxCommandEvent event(SPEK_NOTIFY_EVENT, -1);
        event.SetEventObject(self);
        wxPostEvent(self, event);
    }

    // Update the preferences.
    prefs.set_check_update(true);
    prefs.set_last_update(days);
    return NULL;
}
#endif
