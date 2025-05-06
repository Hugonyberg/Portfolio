#pragma once
#include "UnityLoader.h"

class BinaryExporter
{
public:
	BinaryExporter();
	~BinaryExporter();

	//Checks if the bin file exists and returns true if it doesn't. 
	//Also checks if bin or json is last updated, returns true if json is last updated.
	bool BinaryOrJson(const std::string& aBinaryFilePath, const std::string& aJsonFilePath); 

	//Takes LevelDataStruct and makes it into a binary file.
	// Must run in order of LevelDataStruct. If not, file will go boom.
	void ExportLevelDataToBinary(LevelData aLeveldata, std::string aFileName);

	//Imports LevelData from binary file. Should only be used after the file is written.
	void ImportLevelDataFromBinary(LevelData& aLevelData, std::string aFileName);
	
};