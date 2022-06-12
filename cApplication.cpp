#include "cApplication.h"
#include "cFaceSerializer.h"

wxIMPLEMENT_APP(cApplication);

cApplication::cApplication()
{
}

cApplication::~cApplication()
{
	cFaceSerializer::UploadFacesToDatabase();
	cFaceSerializer::DeleteFacesFromLocalMachine();
}

bool cApplication::OnInit()
{
	m_titleFrame = new cTitleFrame();
	m_titleFrame->Show();

	wxImage::AddHandler(new wxPNGHandler);

	return true;
}

const wxSize cApplication::GetDefaultWindowSize()
{
	return wxSize(defaultWindowXSize, defaultWindowYSize);
}
