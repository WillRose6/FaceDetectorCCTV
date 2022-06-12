#pragma once

#include "wx/wx.h"
#include "opencv2/videoio.hpp"
#include "opencv2/objdetect.hpp"
#include "cImplementsBackButton_Interface.h"

class wxBitmapFromOpenCVPanel;
class cBackButton;

wxDEFINE_EVENT(wxEVT_WEBCAM_FRAME_AVAILABLE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_WEBCAM_FRAME_EMPTY, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_WEBCAM_EXCEPTION, wxThreadEvent);

class t_WebcamThread : public wxThread {

private:
	virtual ExitCode Entry() override;

	bool CreateHaarCascadeClassifier(const std::string& path);
	void DisplayWebcam(std::shared_ptr<wxStopWatch>& m_detectionIntervalStopwatch);
	std::shared_ptr<wxStopWatch> CreateFaceDetectionTimer();
	std::vector<cv::Rect> PerformFaceDetection(const cv::Mat& capturedBitmap);
	void SaveDetectedFace(const cv::Rect& faceCoordinates, const cv::Mat& capturedBitmap) const;

	wxEvtHandler& m_eventHandler;
	cv::VideoCapture& m_videoCapture;
	std::unique_ptr<cv::CascadeClassifier> m_faceCascade;
	int m_faceID = 0;

	//Scan for faces every three seconds.
	const int m_interval = 3000.0;
	const std::string m_faceSavePath = "Faces/";

public:
	struct Frame {
		cv::Mat m_capturedBitmap;
		float m_time = 0;
	};

	t_WebcamThread(cv::VideoCapture& inVideoCapture, wxEvtHandler& inEventHandler);
	~t_WebcamThread();
};

class c_CCTVScreen : public wxFrame, public cImplementsBackButton_Interface
{
public:
	c_CCTVScreen();
	~c_CCTVScreen();

private:
	void StartWebcam();

	bool StartWebcamThread();
	bool DeleteWebcamThread();

	void OnWebcamFrameDetected(wxThreadEvent& evt);
	void OnWebcamFrameEmpty(wxThreadEvent& evt);
	void OnWebcamException(wxThreadEvent& evt);

	void OnBack() override;
	
	t_WebcamThread* m_webcamThread = nullptr;
	std::unique_ptr<cv::VideoCapture> m_videoCapture = nullptr;
	wxBitmapFromOpenCVPanel* m_bitmapPanel = nullptr;
	wxPanel* m_CCTVPanel = nullptr;
	wxBoxSizer* m_mainSizer = nullptr;
	cBackButton* m_backButton = nullptr;

	static constexpr float windowSizeXOffset = 10, windowSizeYOffset = 140;
};

