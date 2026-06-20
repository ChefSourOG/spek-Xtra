#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/dnd.h>
#include <wx/filename.h>
#include <wx/protocol/http.h>
#include <wx/splitter.h>
#include <wx/sstream.h>

// WX on WIN doesn't like it when pthread.h is included first.
#include <pthread.h>

#include <spek-utils.h>

#include "spek-artwork.h"
#include "spek-export-dialog.h"
#include "spek-info-panel.h"
#include "spek-palette.h"
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

BEGIN_EVENT_TABLE(SpekWindow, wxFrame)
    EVT_MENU(wxID_OPEN, SpekWindow::on_open)
    EVT_MENU(ID_EXPORT_IMAGE, SpekWindow::on_export)
    EVT_MENU(wxID_EXIT, SpekWindow::on_exit)
    EVT_MENU(wxID_PREFERENCES, SpekWindow::on_preferences)
    EVT_MENU(wxID_HELP, SpekWindow::on_help)
    EVT_MENU(wxID_ABOUT, SpekWindow::on_about)
    EVT_MENU(ID_VIEW_SHOW_INFO_PANEL, SpekWindow::on_show_info_panel)
    EVT_MENU_RANGE(ID_FFT_SIZE_BASE, ID_FFT_SIZE_BASE + 3, SpekWindow::on_fft_size)
    EVT_MENU_RANGE(ID_WINDOW_FUNCTION_BASE, ID_WINDOW_FUNCTION_BASE + 3, SpekWindow::on_window_function)
    EVT_COMMAND(-1, SPEK_NOTIFY_EVENT, SpekWindow::on_notify)
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
        if (filenames.GetCount() == 1) {
            window->open(filenames[0]);
            return true;
        }
        return false;
    }

private:
    SpekWindow *window;
};

SpekWindow::SpekWindow(int width, int height, const wxString& path, const wxString& pngpath) :
    wxFrame(NULL, -1, wxEmptyString, wxDefaultPosition, wxSize(width, height)),
    menu_view_info(NULL), info_sash_position(width - 280), path(path), pngpath(pngpath)
{
    this->description = _("Spek - Acoustic Spectrum Analyser");
    SetTitle(this->description);

#ifndef OS_OSX
    SetIcons(wxArtProvider::GetIconBundle(ART_SPEK, wxART_FRAME_ICON));
#endif

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

    wxMenuBar *menu = new wxMenuBar();

    wxMenu *menu_file = new wxMenu();
    wxMenuItem *menu_file_open = new wxMenuItem(menu_file, wxID_OPEN);
    menu_file->Append(menu_file_open);
    this->menu_file_export = new wxMenuItem(menu_file, ID_EXPORT_IMAGE, _("Export &Image..."));
    this->menu_file_export->SetItemLabel(this->menu_file_export->GetItemLabelText() + "\tCtrl+S");
    menu_file->Append(this->menu_file_export);
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
    toolbar->AddStretchableSpace();
    toolbar->AddTool(
        wxID_HELP,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_HELP, wxART_TOOLBAR),
        _("Help")
    );
    toolbar->Realize();

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
            this, ID_PALETTE_BASE + i,
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

    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    // wxInfoBar is too limited, construct a custom one.
    wxPanel *info_bar = new wxPanel(this);
    info_bar->Hide();
    info_bar->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    info_bar->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    wxSizer *info_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *label = new wxStaticText(
        info_bar, -1, _("A new version of Spek is available, click to download."));
    label->SetCursor(*new wxCursor(wxCURSOR_HAND));
    label->Connect(wxEVT_LEFT_DOWN, wxCommandEventHandler(SpekWindow::on_visit));
    // This second Connect() handles clicks on the border
    info_bar->Connect(wxEVT_LEFT_DOWN, wxCommandEventHandler(SpekWindow::on_visit));
    info_sizer->Add(label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 6);
    wxBitmapButton *button = new wxBitmapButton(
        info_bar, -1, wxArtProvider::GetBitmap(ART_CLOSE, wxART_BUTTON),
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SpekWindow::on_close));
    info_sizer->Add(button, 0, wxALIGN_CENTER_VERTICAL);
    info_bar->SetSizer(info_sizer);
    sizer->Add(info_bar, 0, wxEXPAND);
    sizer->Add(palette_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 4);

    this->splitter = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE);
    this->spectrogram = new SpekSpectrogram(this->splitter);
    this->spectrogram->set_fft_bits(initial_fft_bits);
    this->spectrogram->set_window_function((enum window_function)initial_window_function);
    this->spectrogram->set_palette((enum palette)initial_palette);
    this->info_panel = new SpekInfoPanel(this->splitter);
    this->splitter->SetMinimumPaneSize(260);
    sizer->Add(this->splitter, 1, wxEXPAND);

    SetSizer(sizer);

    this->update_info_panel_visibility();

    this->cur_dir = wxGetHomeDir();

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
    wxFileName file_name(path);
    if (file_name.FileExists()) {
        this->path = path;
        wxString full_name = file_name.GetFullName();
        // TRANSLATORS: window title, %s is replaced with the file name
        wxString title = wxString::Format(_("Spek - %s"), full_name.c_str());
        SetTitle(title);

        this->spectrogram->open(path, this->pngpath);

        this->update_info_panel_info();

        if (this->menu_file_export) {
            this->menu_file_export->Enable(true);
        }
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

void SpekWindow::on_open(wxCommandEvent&)
{
    static wxString filters = wxEmptyString;
    static int filter_index = 1;

    if (filters.IsEmpty()) {
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
    }

    wxFileDialog *dlg = new wxFileDialog(
        this,
        _("Open File"),
        this->cur_dir,
        wxEmptyString,
        filters,
        wxFD_OPEN
    );
    dlg->SetFilterIndex(filter_index);

    if (dlg->ShowModal() == wxID_OK) {
        this->cur_dir = dlg->GetDirectory();
        filter_index = dlg->GetFilterIndex();
        open(dlg->GetPath());
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
    info.SetName("Spek-X");
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
    this->GetSizer()->Show((size_t)0);
    this->Layout();
}

void SpekWindow::on_visit(wxCommandEvent&)
{
    wxLaunchDefaultBrowser("https://github.com/MikeWang000000/spek-X/releases");
}

void SpekWindow::on_close(wxCommandEvent& event)
{
    wxWindow *self = ((wxWindow *)event.GetEventObject())->GetGrandParent();
    self->GetSizer()->Hide((size_t)0);
    self->Layout();
}

void SpekWindow::on_show_info_panel(wxCommandEvent& event)
{
    SpekPreferences::get().set_show_info_panel(event.IsChecked());
    this->update_info_panel_visibility();
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
            int sash = this->info_sash_position;
            int max_sash = this->splitter->GetSize().GetWidth() - 260;
            if (sash < 260) {
                sash = 260;
            } else if (sash > max_sash) {
                sash = max_sash;
            }
            this->splitter->SplitVertically(this->spectrogram, this->info_panel, sash);
        }
    } else {
        if (this->splitter->IsSplit()) {
            this->info_sash_position = this->splitter->GetSashPosition();
            this->splitter->Unsplit(this->info_panel);
        }
    }
}

void SpekWindow::on_fft_size(wxCommandEvent& event)
{
    int bits = 9 + (event.GetId() - ID_FFT_SIZE_BASE);
    SpekPreferences::get().set_fft_bits(bits);
    this->spectrogram->set_fft_bits(bits);
    this->update_info_panel_info();
}

void SpekWindow::on_window_function(wxCommandEvent& event)
{
    enum window_function f = (enum window_function)(event.GetId() - ID_WINDOW_FUNCTION_BASE);
    SpekPreferences::get().set_window_function(f);
    this->spectrogram->set_window_function(f);
}

void SpekWindow::on_palette(wxCommandEvent& event)
{
    enum palette p = (enum palette)(event.GetId() - ID_PALETTE_BASE);
    SpekPreferences::get().set_palette(p);
    this->spectrogram->set_palette(p);
}

void SpekWindow::update_info_panel_info()
{
    if (this->info_panel && this->spectrogram) {
        SpekAudioInfo info;
        this->spectrogram->get_info(info);
        this->info_panel->set_info(info);
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
