#pragma once

#include "wx/wx.h"

class c_CCTVScreen;

class cTitleFrame : public wxFrame
{
public:
	cTitleFrame();
	~cTitleFrame();

private:
	wxStaticText* titleTxt = nullptr;
	wxButton* showCCTVBtn = nullptr;
	wxButton* showDocumentedBtn = nullptr;

	void OnCCTVButtonClicked(wxCommandEvent& evt);
	void OnDocumentedButtonClicked(wxCommandEvent& evt);

	void UploadFacesToDatabase();

	wxDECLARE_EVENT_TABLE();
};

