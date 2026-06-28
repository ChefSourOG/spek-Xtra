#include <wx/artprov.h>
#include <wx/iconbndl.h>

#include "spek-artwork.h"

class SpekArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size);
    virtual wxIconBundle CreateIconBundle(const wxArtID& id, const wxArtClient& client);
};

wxBitmap SpekArtProvider::CreateBitmap(
    const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
	(void) id, (void) client, (void) size;
#ifdef OS_UNIX
    if (id == ART_SPEK) {
        return wxArtProvider::GetBitmap("spek", client, size);
    }
    if (id == ART_HELP) {
        return wxArtProvider::GetBitmap("gtk-help", client, size);
    }
    if (id == ART_OPEN) {
        return wxArtProvider::GetBitmap("gtk-open", client, size);
    }
    if (id == ART_SAVE) {
        return wxArtProvider::GetBitmap("gtk-save", client, size);
    }
    if (id == ART_CLOSE) {
        return wxArtProvider::GetBitmap("gtk-close", client, size);
    }
#endif
#ifdef OS_WIN
    wxSize icon_size = size;
    if (icon_size == wxDefaultSize) {
        icon_size = wxArtProvider::GetSizeHint(client);
    }
    if (icon_size == wxDefaultSize) {
        icon_size = wxSize(24, 24);
    }

    const char *resource = NULL;
    if (id == ART_HELP)        resource = "help";
    else if (id == ART_OPEN)   resource = "open";
    else if (id == ART_SAVE)   resource = "save";
    else if (id == ART_CLOSE)  resource = "close";
    else if (id == ART_COPY)   resource = "copy";
    else if (id == ART_HOME)   resource = "home";
    else if (id == ART_FILE_OPEN) resource = "fileopen";

    if (resource) {
        return wxBitmap(wxIcon(resource, wxBITMAP_TYPE_ICO_RESOURCE, icon_size.x, icon_size.y));
    }
#endif
#ifdef OS_OSX
    if (id == ART_HELP) {
        return wxBitmap("help", wxBITMAP_TYPE_PNG_RESOURCE);
    }
    if (id == ART_OPEN) {
        return wxBitmap("open", wxBITMAP_TYPE_PNG_RESOURCE);
    }
    if (id == ART_SAVE) {
        return wxBitmap("save", wxBITMAP_TYPE_PNG_RESOURCE);
    }
    if (id == ART_CLOSE) {
        return wxBitmap("close", wxBITMAP_TYPE_PNG_RESOURCE);
    }
#endif
    return wxNullBitmap;
}

wxIconBundle SpekArtProvider::CreateIconBundle(const wxArtID& id, const wxArtClient& client)
{
	(void) id, (void) client;
#ifdef OS_UNIX
    if (id == ART_SPEK) {
        return wxArtProvider::GetIconBundle("spek", client);
    }
#endif
#ifdef OS_WIN
    if (id == ART_SPEK) {
        wxIconBundle bundle;
        wxIcon icon16("aaaa", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
        if (icon16.IsOk()) bundle.AddIcon(icon16);
        wxIcon icon24("aaaa", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24);
        if (icon24.IsOk()) bundle.AddIcon(icon24);
        wxIcon icon32("aaaa", wxBITMAP_TYPE_ICO_RESOURCE, 32, 32);
        if (icon32.IsOk()) bundle.AddIcon(icon32);
        wxIcon icon48("aaaa", wxBITMAP_TYPE_ICO_RESOURCE, 48, 48);
        if (icon48.IsOk()) bundle.AddIcon(icon48);
        return bundle;
    }
#endif
    return wxNullIconBundle;
}

void spek_artwork_init()
{
    wxArtProvider::Push(new SpekArtProvider());
}
