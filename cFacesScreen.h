#pragma once
#include <wx/wx.h>
#include "cBackButton.h"
#include "cImplementsBackButton_Interface.h"

class cFacesScreen : public wxFrame, public cImplementsBackButton_Interface
{
private:
    std::vector<wxBitmap*> faceImages;
    cBackButton* m_backButton = nullptr;
    wxButton* m_emptyButton = nullptr;
    wxGridSizer* m_facesSizer = nullptr;

    void OnBack() override;
    void OnEmpty(wxCommandEvent& evt);
    void Reload();
    void ShowFaceImages();
    void DeleteFaceImages();

public:
    cFacesScreen();
    ~cFacesScreen();
};
