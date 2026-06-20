#include <wx/string.h>

#include "spek-platform.h"
#include "spek-palette.h"
#include "spek-pipeline.h"

#include "spek-preferences.h"

SpekPreferences& SpekPreferences::get()
{
    static SpekPreferences instance;
    return instance;
}

void SpekPreferences::init()
{
    if (this->locale) {
        delete this->locale;
    }
    this->locale = new wxLocale();

#ifdef OS_WIN
    // Load MO files embedded in Windows executable file.
    wxTranslations *translation = wxTranslations::Get();
    if (translation == nullptr) {
        translation = new wxTranslations();
        wxTranslations::Set(translation);
    }
    wxResourceTranslationsLoader *loader = new wxResourceTranslationsLoader();
    translation->SetLoader(loader);
#endif

    int lang = wxLANGUAGE_DEFAULT;
    wxString code = this->get_language();
    if (spek_platform_can_change_language() && !code.IsEmpty()) {
        const wxLanguageInfo *info = wxLocale::FindLanguageInfo(code);
        if (info) {
            lang = info->Language;
        }
    }
    this->locale->Init(lang);
    this->locale->AddCatalog(GETTEXT_PACKAGE);
}

SpekPreferences::SpekPreferences() : locale(NULL)
{
    wxString path = spek_platform_config_path("spek");
    this->config = new wxFileConfig(
        wxEmptyString,
        wxEmptyString,
        path,
        wxEmptyString,
        wxCONFIG_USE_LOCAL_FILE,
        wxConvUTF8
    );
}

bool SpekPreferences::get_check_update()
{
    bool result = false;
    this->config->Read("/update/check", &result);
    return result;
}

void SpekPreferences::set_check_update(bool value)
{
    this->config->Write("/update/check", value);
    this->config->Flush();
}

long SpekPreferences::get_last_update()
{
    long result = 0;
    this->config->Read("/update/last", &result);
    return result;
}

void SpekPreferences::set_last_update(long value)
{
    this->config->Write("/update/last", value);
    this->config->Flush();
}

wxString SpekPreferences::get_language()
{
    wxString result("");
    this->config->Read("/general/language", &result);
    return result;
}

void SpekPreferences::set_language(const wxString& value)
{
    this->config->Write("/general/language", value);
    this->config->Flush();
}

bool SpekPreferences::get_hide_full_path()
{
    bool result = false;
    this->config->Read("/general/hidepath", &result);
    return result;
}

void SpekPreferences::set_hide_full_path(bool value)
{
    this->config->Write("/general/hidepath", value);
    this->config->Flush();
}

bool SpekPreferences::get_show_detailed_description()
{
    bool result = false;
    this->config->Read("/general/showdetails", &result);
    return result;
}

void SpekPreferences::set_show_detailed_description(bool value)
{
    this->config->Write("/general/showdetails", value);
    this->config->Flush();
}

bool SpekPreferences::get_show_info_panel()
{
    bool result = true;
    this->config->Read("/general/showinfopanel", &result);
    return result;
}

void SpekPreferences::set_show_info_panel(bool value)
{
    this->config->Write("/general/showinfopanel", value);
    this->config->Flush();
}

int SpekPreferences::get_fft_bits()
{
    int result = 11;
    this->config->Read("/analysis/fftbits", &result);
    return result;
}

void SpekPreferences::set_fft_bits(int value)
{
    this->config->Write("/analysis/fftbits", value);
    this->config->Flush();
}

int SpekPreferences::get_window_function()
{
    int result = WINDOW_DEFAULT;
    this->config->Read("/analysis/windowfunction", &result);
    return result;
}

void SpekPreferences::set_window_function(int value)
{
    this->config->Write("/analysis/windowfunction", value);
    this->config->Flush();
}

int SpekPreferences::get_palette()
{
    int result = PALETTE_DEFAULT;
    this->config->Read("/analysis/palette", &result);
    return result;
}

void SpekPreferences::set_palette(int value)
{
    this->config->Write("/analysis/palette", value);
    this->config->Flush();
}

int SpekPreferences::get_window_width()
{
    int result = 640 * spek_platform_dpi_scale();
    this->config->Read("/general/width", &result);
    return result;
}

int SpekPreferences::get_window_height()
{
    int result = 480 * spek_platform_dpi_scale();
    this->config->Read("/general/height", &result);
    return result;
}

wxArrayString SpekPreferences::get_recent_files()
{
    wxArrayString result;
    for (int i = 0; i < 10; ++i) {
        wxString value;
        if (this->config->Read(wxString::Format("/recent/file%d", i), &value) && !value.IsEmpty()) {
            result.Add(value);
        }
    }
    return result;
}

void SpekPreferences::add_recent_file(const wxString& value)
{
    wxArrayString files = this->get_recent_files();
    int index = files.Index(value);
    if (index != wxNOT_FOUND) {
        files.RemoveAt(index);
    }
    files.Insert(value, 0);
    while (files.GetCount() > 10) {
        files.RemoveAt(files.GetCount() - 1);
    }
    for (size_t i = 0; i < files.GetCount(); ++i) {
        this->config->Write(wxString::Format("/recent/file%d", (int)i), files[i]);
    }
    for (int i = (int)files.GetCount(); i < 10; ++i) {
        this->config->DeleteEntry(wxString::Format("/recent/file%d", i), false);
    }
    this->config->Flush();
}

void SpekPreferences::clear_recent_files()
{
    for (int i = 0; i < 10; ++i) {
        this->config->DeleteEntry(wxString::Format("/recent/file%d", i), false);
    }
    this->config->Flush();
}

wxArrayString SpekPreferences::get_queue()
{
    wxArrayString result;
    for (int i = 0; i < 100; ++i) {
        wxString value;
        if (this->config->Read(wxString::Format("/queue/file%d", i), &value) && !value.IsEmpty()) {
            result.Add(value);
        }
    }
    return result;
}

void SpekPreferences::set_queue(const wxArrayString& value)
{
    for (size_t i = 0; i < value.GetCount() && i < 100; ++i) {
        this->config->Write(wxString::Format("/queue/file%d", (int)i), value[i]);
    }
    for (size_t i = value.GetCount(); i < 100; ++i) {
        this->config->DeleteEntry(wxString::Format("/queue/file%d", (int)i), false);
    }
    this->config->Flush();
}

void SpekPreferences::clear_queue()
{
    for (int i = 0; i < 100; ++i) {
        this->config->DeleteEntry(wxString::Format("/queue/file%d", i), false);
    }
    this->config->Flush();
}

bool SpekPreferences::get_show_queue()
{
    bool result = true;
    this->config->Read("/general/showqueue", &result);
    return result;
}

void SpekPreferences::set_show_queue(bool value)
{
    this->config->Write("/general/showqueue", value);
    this->config->Flush();
}
