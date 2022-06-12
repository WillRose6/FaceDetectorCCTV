#include "cFacesScreen.h"
#include "cApplication.h"
#include "bmpfromocvpanel.h"
#include "cFaceSerializer.h"

cFacesScreen::cFacesScreen() : wxFrame(nullptr, wxID_ANY, "Documented Faces", wxDefaultPosition, cApplication::GetDefaultWindowSize())
{
	ShowFaceImages();

    //Create the buttons
    wxSize buttonSizes(490, 100);
    int buttonYOffset = cApplication::GetDefaultWindowSize().GetY() - 140;
	m_backButton = new cBackButton(this, 1, "Back", wxPoint(0, buttonYOffset), buttonSizes);

    m_emptyButton = new wxButton(this, 2, "Delete all faces", wxPoint(buttonSizes.GetX(), buttonYOffset), buttonSizes);
    m_emptyButton->Bind(wxEVT_BUTTON, &cFacesScreen::OnEmpty, this);
}

cFacesScreen::~cFacesScreen()
{
}

void cFacesScreen::OnBack()
{
}

void cFacesScreen::OnEmpty(wxCommandEvent& evt)
{

	Reload();
}

void cFacesScreen::Reload()
{
	DeleteFaceImages();
	ShowFaceImages();
}

void cFacesScreen::ShowFaceImages()
{
	int numOfFaces = 0;

	faceImages = cFaceSerializer::RetrieveAllFaces();
	numOfFaces = (int)faceImages.size();

	//This is managed by wxwidgets so 'new' is alright here
	m_facesSizer = new wxGridSizer(5, 5, 10, 0);

	for (wxBitmap* a : faceImages)
	{
		//This is deleted by wxWidgets!
		wxStaticBitmap* sBmp = new wxStaticBitmap(this, wxID_ANY, *a);
		sBmp->SetSize(wxSize(100, 100));
		m_facesSizer->Add(sBmp);
	}
	SetSizer(m_facesSizer);
	m_facesSizer->Layout();
	Layout();
}

void cFacesScreen::DeleteFaceImages()
{
	m_facesSizer->Clear(true);
	faceImages.empty();
	cFaceSerializer::DeleteAllFaces();
}
