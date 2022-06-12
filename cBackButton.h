#pragma once
#include "wx/wx.h"
class cBackButton : public wxButton
{
private:
	~cBackButton();

	void Back(wxCommandEvent& evt);

public:
	cBackButton(wxWindow* Parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
};

