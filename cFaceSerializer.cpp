#include "cFaceSerializer.h"
#include "wx/wx.h"
#include <SQLAPI.h>
#include "base64.h"
#include <opencv2/opencv.hpp>
#include "cConverters.h"
#include <cstdio>

using namespace cv;

const char* cFaceSerializer::m_host = "sql4.freesqldatabase.com";
const char* cFaceSerializer::m_username = "sql4499303";
const char* cFaceSerializer::m_password = "zbhZM9vN2s";
const char* cFaceSerializer::m_database = "sql4499303";
const int cFaceSerializer::m_portNumber = 3306;

std::vector<wxBitmap*> cFaceSerializer::RetrieveFacesFromLocalMachine()
{
	std::vector<wxBitmap*> faceImages;
	for (const auto& entry : std::experimental::filesystem::directory_iterator("Faces/"))
	{
		if (CheckFileIsFaceImage(entry.path()))
		{
			wxBitmap* bitmap = new wxBitmap();
			bitmap->LoadFile(entry.path().generic_string(), wxBITMAP_TYPE_PNG);
			faceImages.push_back(bitmap);
		}
	}

	return faceImages;
}

int cFaceSerializer::GetNumberOfFacesSavedLocally() {

	int counter = 0;

	for (const auto& entry : std::experimental::filesystem::directory_iterator("Faces/"))
	{
		if (CheckFileIsFaceImage(entry.path())) {
			counter++;
		}
	}

	return counter;
}

std::vector<wxBitmap*> cFaceSerializer::RetrieveFacesFromDatabase()
{
	std::vector<wxBitmap*> faceImages;

	SAConnection connection;
	try {
		connection.setClient(SA_MySQL_Client);
		SAString connectionString = "sql4.freesqldatabase.com@";
		connectionString += m_database;
		connection.Connect(connectionString, _TSA(m_username), _TSA(m_password));

		SACommand select(&connection, _TSA("SELECT Face FROM Faces"));
		select.Execute();
		
		while (select.FetchNext()) {
			SAString str = select[1].asBLob();
			wxImage* img = new wxImage();
			cConverters::Convert<SAString, wxImage>(str, *img);
			wxBitmap* bmp = new wxBitmap(*img);
			faceImages.push_back(bmp);
		}

		connection.Disconnect();
	}
	catch (const SAException& e) {
		connection.Rollback();
		wxMessageBox(e.ErrText().GetMultiByteChars());
		connection.Disconnect();
	}

    return faceImages;
}

std::vector<wxBitmap*> cFaceSerializer::RetrieveAllFaces()
{
	std::vector<wxBitmap*> localFaces = RetrieveFacesFromLocalMachine();
	std::vector<wxBitmap*> databaseFaces = RetrieveFacesFromDatabase();

	localFaces.insert(localFaces.end(), databaseFaces.begin(), databaseFaces.end());
	return localFaces;
}

void cFaceSerializer::UploadFacesToDatabase()
{
	SAConnection connection;
	try {
		connection.setClient(SA_MySQL_Client);
		SAString connectionString = "sql4.freesqldatabase.com@";
		connectionString += m_database;
		connection.Connect(connectionString, _TSA(m_username), _TSA(m_password));		

		for (wxBitmap* bitmap : RetrieveFacesFromLocalMachine()) {
			SACommand insert(&connection, "INSERT INTO Faces (Face) VALUES (:1)");
			wxImage img = bitmap->ConvertToImage();
			SAString base64Str;
			cConverters::Convert<wxImage, SAString>(img, base64Str);
			insert.Param(1).setAsBLob() = base64Str;
			insert.Execute();
			connection.Commit();
		}

		connection.Disconnect();
	}
	catch (const SAException& e) {
		connection.Rollback();
		wxMessageBox(e.ErrText().GetMultiByteChars());
		connection.Disconnect();
	}
}

void cFaceSerializer::DeleteAllFaces()
{
	DeleteFacesFromLocalMachine();
	DeleteFacesFromDatabase();
}

void cFaceSerializer::DeleteFacesFromLocalMachine()
{
	for (const auto& entry : std::experimental::filesystem::directory_iterator("Faces/"))
	{
		if (CheckFileIsFaceImage(entry.path()))
		{
			remove(entry.path());
		}
	}
}

void cFaceSerializer::DeleteFacesFromDatabase()
{
	SAConnection connection;
	try {
		connection.setClient(SA_MySQL_Client);
		SAString connectionString = "sql4.freesqldatabase.com@";
		connectionString += m_database;
		connection.Connect(connectionString, _TSA(m_username), _TSA(m_password));

		SACommand del(&connection, "TRUNCATE TABLE Faces");
		del.Execute();

		connection.Disconnect();
	}
	catch (const SAException& e) {
		connection.Rollback();
		wxMessageBox(e.ErrText().GetMultiByteChars());
		connection.Disconnect();
	}

}

bool cFaceSerializer::CheckFileIsFaceImage(const std::experimental::filesystem::path& path)
{
	if (path.extension().generic_string() == ".png")
	{
		return true;
	}
	return false;
}
