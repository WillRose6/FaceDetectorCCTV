#include "c_CCTVScreen.h"
#include "cApplication.h"
#include "convertmattowxbmp.h"
#include "bmpfromocvpanel.h"
#include "cBackButton.h"
#include "cConverters.h"
#include "cFaceSerializer.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

using namespace cv;

#define DELETE_FRAME(frame) delete frame; frame = nullptr;

c_CCTVScreen::c_CCTVScreen() : wxFrame(nullptr, wxID_ANY, "CCTV", wxDefaultPosition, wxDefaultSize)
{
	//No need to delete these, wxWidgets handles all memory management for it's classes.
	m_CCTVPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_bitmapPanel = new wxBitmapFromOpenCVPanel(m_CCTVPanel);
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	m_backButton = new cBackButton(this, 1, "Back", wxPoint(0, 480), wxSize(640 , 100));

	m_mainSizer->Add(m_bitmapPanel, wxSizerFlags().Proportion(1).Expand());

	m_CCTVPanel->SetSizerAndFit(m_mainSizer);

	Bind(wxEVT_WEBCAM_FRAME_AVAILABLE, &c_CCTVScreen::OnWebcamFrameDetected, this);
	Bind(wxEVT_WEBCAM_FRAME_EMPTY, &c_CCTVScreen::OnWebcamFrameEmpty, this);
	Bind(wxEVT_WEBCAM_EXCEPTION, &c_CCTVScreen::OnWebcamException, this);

	StartWebcam();
}

c_CCTVScreen::~c_CCTVScreen() { DeleteWebcamThread(); }

void c_CCTVScreen::StartWebcam()
{
	StartWebcamThread();
}

bool c_CCTVScreen::StartWebcamThread()
{
	DeleteWebcamThread();

	m_videoCapture = std::make_unique<VideoCapture>(0);
	float height = m_videoCapture->get(CAP_PROP_FRAME_HEIGHT), width = m_videoCapture->get(CAP_PROP_FRAME_WIDTH);
	SetSize(wxSize(width + windowSizeXOffset, height + windowSizeYOffset));
	m_CCTVPanel->SetSize(wxSize(width, height));
	
	//Using 'new' here so I can shut the thread down when I want
	m_webcamThread = new t_WebcamThread(*m_videoCapture, *this);
	m_webcamThread->Run();

	return false;
}

bool c_CCTVScreen::DeleteWebcamThread()
{
	if (m_webcamThread) {
		m_webcamThread->Pause();
		m_webcamThread->Delete();
		m_webcamThread = nullptr;
		return true;
	}
	return false;
}

void c_CCTVScreen::OnWebcamFrameDetected(wxThreadEvent& evt)
{
	t_WebcamThread::Frame* frame = evt.GetPayload<t_WebcamThread::Frame*>();
	long conversionTime = 0;

	wxImage image;
	cConverters::Convert<Mat, wxImage>(frame->m_capturedBitmap, image);
	wxBitmap bitmap(image);

	if (bitmap.IsOk()) {
		m_bitmapPanel->SetBitmap(bitmap, frame->m_time, 0.0);
	}
	else {
		m_bitmapPanel->SetBitmap(wxBitmap(), 0, 0);
	}
}

void c_CCTVScreen::OnWebcamFrameEmpty(wxThreadEvent& evt)
{
	wxMessageBox("Connection to camera lost...");
	DeleteWebcamThread();
	
}

void c_CCTVScreen::OnWebcamException(wxThreadEvent& evt)
{
	wxMessageBox("Camera error occurred: %s", evt.GetString());
	DeleteWebcamThread();
}

void c_CCTVScreen::OnBack()
{
	DeleteWebcamThread();
}

wxThread::ExitCode t_WebcamThread::Entry()
{
	CreateHaarCascadeClassifier("Resources/haarcascade_frontalface_default.xml");
	std::shared_ptr<wxStopWatch> timer = CreateFaceDetectionTimer();
	m_faceID = cFaceSerializer::GetNumberOfFacesSavedLocally();
	DisplayWebcam(timer);
	return ExitCode();
}

bool t_WebcamThread::CreateHaarCascadeClassifier(const std::string& path)
{
	m_faceCascade = std::make_unique<CascadeClassifier>();
	m_faceCascade->load(path);

	if (m_faceCascade->empty()) {

		//For some reason using wxLogError destroys the performance of my PC so I'm using this instead.
		wxMessageBox("Couldn't load Haar Cascade");
		return false;
	}

	return true;
}

void t_WebcamThread::DisplayWebcam(std::shared_ptr<wxStopWatch>& m_detectionIntervalStopwatch)
{
	wxStopWatch timer;

	while (!TestDestroy()) {
		std::shared_ptr<Frame> currentFrame = std::make_shared<Frame>();
		try
		{
			timer.Start();

			m_videoCapture >> currentFrame->m_capturedBitmap;
			currentFrame->m_time = timer.Time();

			if(m_detectionIntervalStopwatch->Time() > m_interval)
			{
				PerformFaceDetection(currentFrame->m_capturedBitmap);
				m_detectionIntervalStopwatch->Start();
			}

			if (!currentFrame->m_capturedBitmap.empty()) {
				//wxThreadEvents are automatically cleared up - no need to manage the memory here
				wxThreadEvent* evt = new wxThreadEvent(wxEVT_WEBCAM_FRAME_AVAILABLE);
				evt->SetPayload(currentFrame);
				m_eventHandler.QueueEvent(evt);
			}
			else {
				//If the camera is no longer outputting anything
				m_eventHandler.QueueEvent(new wxThreadEvent(wxEVT_WEBCAM_FRAME_EMPTY));
				break;
			}
		}
		catch (const std::exception& e)
		{
			wxThreadEvent* evt = new wxThreadEvent(wxEVT_WEBCAM_EXCEPTION);
			evt->SetString(e.what());
			m_eventHandler.QueueEvent(evt);
			break;
		}
	}
}

std::shared_ptr<wxStopWatch> t_WebcamThread::CreateFaceDetectionTimer()
{
	std::shared_ptr<wxStopWatch> m_detectionIntervalStopwatch = std::make_shared<wxStopWatch>();
	m_detectionIntervalStopwatch->Start();
	return m_detectionIntervalStopwatch;
}

std::vector<cv::Rect> t_WebcamThread::PerformFaceDetection(const cv::Mat& capturedBitmap)
{
	std::vector<cv::Rect> faces;
	m_faceCascade->detectMultiScale(capturedBitmap, faces, 1.1, 10);

	for (int i = 0; i < faces.size(); i++) {
		SaveDetectedFace(faces[i], capturedBitmap);
		m_faceID++;
	}
	return faces;
}

void t_WebcamThread::SaveDetectedFace(const cv::Rect& faceCoordinates, const cv::Mat& capturedBitmap) const
{
	Rect roi(faceCoordinates.x, faceCoordinates.y, faceCoordinates.width, faceCoordinates.height);
	Mat img = capturedBitmap(roi);
	Mat dst;
	resize(img, dst, Size(100,100), 0, 0, INTER_CUBIC);
	imwrite(m_faceSavePath + std::to_string(m_faceID) + ".png", dst);
}

t_WebcamThread::t_WebcamThread(cv::VideoCapture& inVideoCapture, wxEvtHandler& inEventHandler) : m_videoCapture(inVideoCapture), m_eventHandler(inEventHandler) { }

t_WebcamThread::~t_WebcamThread() { }
