#include "cTitleFrame.h"
#include "cApplication.h"
#include "cFacesScreen.h"
#include "c_CCTVScreen.h"
#include "cFaceSerializer.h"

wxBEGIN_EVENT_TABLE(cTitleFrame, wxFrame)
EVT_BUTTON(1, OnCCTVButtonClicked)
EVT_BUTTON(2, OnDocumentedButtonClicked)
wxEND_EVENT_TABLE()

cTitleFrame::cTitleFrame() : wxFrame(nullptr, wxID_ANY, "Title", wxDefaultPosition, cApplication::GetDefaultWindowSize())
{
	int titleXOffset = -150, titleYOffset = -150, cctvButtonXOffset = -50, cctvButtonYOffset = -75, documentedButtonXOffset = -100, documentedButtonYOffset = 50;
	wxFont font(20, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	titleTxt = new wxStaticText(this, wxID_ANY, "Welcome to your CCTV!", wxPoint((cApplication::GetDefaultWindowSize().x / 2) + titleXOffset, (cApplication::GetDefaultWindowSize().y / 2) + titleYOffset), wxSize(450, 50), 1L);
	titleTxt->SetFont(font);
	showCCTVBtn = new wxButton(this, 1, "Show CCTV", wxPoint((cApplication::GetDefaultWindowSize().x / 2) + cctvButtonXOffset, (cApplication::GetDefaultWindowSize().y / 2) + cctvButtonYOffset), wxSize(100, 90));
	showDocumentedBtn = new wxButton(this, 2, "Show documented faces", wxPoint((cApplication::GetDefaultWindowSize().x / 2) + documentedButtonXOffset, (cApplication::GetDefaultWindowSize().y / 2) + documentedButtonYOffset), wxSize(200, 90));
}

cTitleFrame::~cTitleFrame()
{
}

void cTitleFrame::OnCCTVButtonClicked(wxCommandEvent& evt)
{
	//Open the CCTV screen here. Deletion of wxFrame objects is handled by wxWidgets automatically so therefore I do not need to delete this memory.
	c_CCTVScreen* CCTVScreen = new c_CCTVScreen();
	CCTVScreen->Show();
	evt.Skip();
	this->Destroy();
}

void cTitleFrame::OnDocumentedButtonClicked(wxCommandEvent& evt)
{
	cFacesScreen* FacesScreen = new cFacesScreen();
	FacesScreen->Show();
	evt.Skip();
	this->Destroy();
}
