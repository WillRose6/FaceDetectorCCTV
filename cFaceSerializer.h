#pragma once
#include <vector>
#include <experimental/filesystem>

class wxBitmap;
class SAString;

class cFaceSerializer
{
private:
	static const char* m_host;
	static const char* m_username;
	static const char* m_password;
	static const char* m_database;
	static const int m_portNumber;

public:
	static std::vector<wxBitmap*> RetrieveFacesFromLocalMachine();
	static std::vector<wxBitmap*> RetrieveFacesFromDatabase();
	static std::vector<wxBitmap*> RetrieveAllFaces();

	static void UploadFacesToDatabase();

	static void DeleteAllFaces();
	static void DeleteFacesFromLocalMachine();
	static void DeleteFacesFromDatabase();

	static bool CheckFileIsFaceImage(const std::experimental::filesystem::path& path);
	static int GetNumberOfFacesSavedLocally();

};

