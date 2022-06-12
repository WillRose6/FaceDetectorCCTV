#pragma once

#include "wx/wx.h"
#include "cTitleFrame.h"

class cApplication : public wxApp
{
public:
	cApplication();
	~cApplication();

private:
	cTitleFrame* m_titleFrame = nullptr;

	bool OnInit() override;

	static const int defaultWindowXSize = 1000, defaultWindowYSize = 1000;

public:

	static const wxSize GetDefaultWindowSize();
};

