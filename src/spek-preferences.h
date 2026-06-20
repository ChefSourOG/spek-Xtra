#pragma once

#include <wx/arrstr.h>
#include <wx/fileconf.h>
#include <wx/intl.h>

class SpekPreferences
{
public:
    static SpekPreferences& get();

    void init();
    bool get_check_update();
    void set_check_update(bool value);
    long get_last_update();
    void set_last_update(long value);
    wxString get_language();
    void set_language(const wxString& value);
    bool get_hide_full_path();
    void set_hide_full_path(bool value);
    bool get_show_detailed_description();
    void set_show_detailed_description(bool value);
    bool get_show_info_panel();
    void set_show_info_panel(bool value);
    int get_fft_bits();
    void set_fft_bits(int value);
    int get_window_function();
    void set_window_function(int value);
    int get_palette();
    void set_palette(int value);
    int get_window_width();
    int get_window_height();
    wxArrayString get_recent_files();
    void add_recent_file(const wxString& value);
    void clear_recent_files();

private:
    SpekPreferences();
    SpekPreferences(const SpekPreferences&);
    void operator=(const SpekPreferences&);

    wxLocale *locale;
    wxFileConfig *config;
};
