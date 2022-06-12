#pragma once
#include <opencv2/opencv.hpp>

#include "wx/wx.h"
#include <SQLAPI.h>
#include "base64.h"
#include <wx/rawbmp.h>

using namespace cv;

class cConverters
{
public:

	//Default function
	template <class A, class B>
	static void Convert(const A& inObj, B& outObj) { std::cerr << "Error: Conversion not defined between " + inObj + " and " + outObj; };

	//Convert wxImage to Base64SAString
	template<> static void Convert(const wxImage& inImg, SAString& outStr) {
		Mat mat;
		Convert<wxImage, Mat>(inImg, mat);
		std::vector<uchar> buffer;
		cv::imencode(".png", mat, buffer);
		auto base64_png = reinterpret_cast<const unsigned char*>(buffer.data());
		outStr = base64_encode(base64_png, buffer.size()).c_str();
	}

	//Convert wxImage to OpenCVMat
	template<> static void Convert(const wxImage& inImg, Mat& outMat) {
		Mat img(Size(inImg.GetWidth(), inImg.GetHeight()), CV_8UC3, inImg.GetData());
		cvtColor(img, outMat, COLOR_RGB2BGR);
	}

	//Convert OpenCVMat to wxImage
	template<> static void Convert(const Mat& inMat, wxImage& outImg) {
		wxBitmap bitmap(inMat.cols, inMat.rows, 24);

		wxNativePixelData           pixelData(bitmap);
		wxNativePixelData::Iterator pixelDataIt(pixelData);
		const uchar* bgr = inMat.data;

		for (int row = 0; row < pixelData.GetHeight(); ++row)
		{
			pixelDataIt.MoveTo(pixelData, 0, row);

			for (int col = 0;
				col < pixelData.GetWidth();
				++col, ++pixelDataIt)
			{
				pixelDataIt.Blue() = *bgr++;
				pixelDataIt.Green() = *bgr++;
				pixelDataIt.Red() = *bgr++;
			}
		}

		auto matBitmapIt = inMat.begin<cv::Vec3b>();

		for (int row = 0; row < pixelData.GetHeight(); ++row)
		{
			pixelDataIt.MoveTo(pixelData, 0, row);

			for (int col = 0;
				col < pixelData.GetWidth();
				++col, ++pixelDataIt, ++matBitmapIt)
			{
				pixelDataIt.Blue() = (*matBitmapIt)[0];
				pixelDataIt.Green() = (*matBitmapIt)[1];
				pixelDataIt.Red() = (*matBitmapIt)[2];
			}
		}

		outImg = bitmap.ConvertToImage();
	}

	//Convert SAString to OpenCVMat
	template<> static void Convert(const SAString& inStr, Mat& outMat) {
		std::string dec_jpg = base64_decode(inStr.GetMultiByteChars());
		std::vector<uchar> data(dec_jpg.begin(), dec_jpg.end());
		outMat = cv::imdecode(cv::Mat(data), 1);
	}

	//Convert SAString to wxImage
	template<> static void Convert(const SAString& inStr, wxImage& outImg) {
		auto mat = std::make_unique<Mat>();
		Convert<SAString, Mat>(inStr, *mat);
		Convert<Mat, wxImage>(*mat, outImg);
	}
};

