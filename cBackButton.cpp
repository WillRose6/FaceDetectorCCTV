#include "cBackButton.h"
#include "cTitleFrame.h"
#include "cImplementsBackButton_Interface.h"

cBackButton::cBackButton(wxWindow* Parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size) : wxButton(Parent, id, title, pos, size)
{
	Bind(wxEVT_BUTTON, &cBackButton::Back, this);
}

cBackButton::~cBackButton()
{
}

void cBackButton::Back(wxCommandEvent& evt)
{
	cImplementsBackButton_Interface* parent = dynamic_cast<cImplementsBackButton_Interface*>(GetParent());
	if (parent) {
		parent->OnBack();
	}

	cTitleFrame* titleFrame = new cTitleFrame();
	titleFrame->Show();
	evt.Skip();
	GetParent()->Destroy();
}
