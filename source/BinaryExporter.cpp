#include "BinaryExporter.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <DreamEngine/windows/settings.h>
#include <string>
#include "BinaryTemplate.h"
BinaryExporter::BinaryExporter()
{
}
BinaryExporter::~BinaryExporter()
{
}

bool BinaryExporter::BinaryOrJson(const std::string& aBinaryFilePath, const std::string& aJsonFilePath)
{
	std::ifstream DataWriter(aBinaryFilePath, std::ios::binary);

	if (!DataWriter)
	{
#ifndef _RETAIL
		std::cout << "Cannot open file!" << std::endl;
#endif
		return true;
	}
	DataWriter.close();
	
	if (DreamEngine::Settings::ResolveAssetPath(aJsonFilePath) == "")
	{
		return false;
	}
	std::filesystem::file_time_type JsonTime = std::filesystem::last_write_time(DreamEngine::Settings::ResolveAssetPath(aJsonFilePath));
	std::filesystem::file_time_type BinaryTime = std::filesystem::last_write_time(DreamEngine::Settings::ResolveAssetPath(aBinaryFilePath));

	if(JsonTime >= BinaryTime)
	{
		std::remove(aBinaryFilePath.c_str());
		return true;
	}
	return false;
}

void BinaryExporter::ExportLevelDataToBinary(LevelData aLevelData, std::string aFileName)
{
	std::ofstream DataWriter(aFileName, std::ios::binary);

	if (!DataWriter)
	{
#ifndef _RETAIL
		std::cout << "Cannot open file!" << std::endl;
#endif
		return;
	}
	Serialize(DataWriter, aLevelData.mainCamera);
	Serialize(DataWriter, aLevelData.directionalLight);
	Serialize(DataWriter, aLevelData.levelName);
	Serialize(DataWriter, aLevelData.names);
	Serialize(DataWriter, aLevelData.tags);
	Serialize(DataWriter, aLevelData.transforms);
	Serialize(DataWriter, aLevelData.fbxPaths);
	Serialize(DataWriter, aLevelData.colliders);
	Serialize(DataWriter, aLevelData.spotLights);
	Serialize(DataWriter, aLevelData.pointLights);
	Serialize(DataWriter, aLevelData.decalPaths);
	Serialize(DataWriter, aLevelData.boatIDs);
	Serialize(DataWriter, aLevelData.boatModelPaths);
	Serialize(DataWriter, aLevelData.componentInteractionPoint);
	Serialize(DataWriter, aLevelData.enemySpawners);
	Serialize(DataWriter, aLevelData.leviathanInfo);
	

	DataWriter.close();

	if (!DataWriter.good())
	{
#ifndef _RETAIL
		std::cout << "Error occurred at writing time!" << std::endl;
#endif
	}
}

void BinaryExporter::ImportLevelDataFromBinary(LevelData& aLevelData, std::string aFileName)
{
	std::ifstream DataReader(aFileName, std::ios::binary);

	if (!DataReader)
	{
#ifndef _RETAIL
		std::cout << "Cannot open file!" << std::endl;
#endif
		return;
	}
	Deserialize(DataReader, aLevelData.mainCamera);
	Deserialize(DataReader, aLevelData.directionalLight);
	Deserialize(DataReader, aLevelData.levelName);
	Deserialize(DataReader, aLevelData.names);
	Deserialize(DataReader, aLevelData.tags);
	Deserialize(DataReader, aLevelData.transforms);
	Deserialize(DataReader, aLevelData.fbxPaths);
	Deserialize(DataReader, aLevelData.colliders);
	Deserialize(DataReader, aLevelData.spotLights);
	Deserialize(DataReader, aLevelData.pointLights);
	Deserialize(DataReader, aLevelData.decalPaths);
	Deserialize(DataReader, aLevelData.boatIDs);
	Deserialize(DataReader, aLevelData.boatModelPaths);
	Deserialize(DataReader, aLevelData.componentInteractionPoint);
	Deserialize(DataReader, aLevelData.enemySpawners);
	Deserialize(DataReader, aLevelData.leviathanInfo);

	if (!DataReader.good())
	{
#ifndef _RETAIL
		std::cout << "Error occurred at writing time!" << std::endl;
#endif
	}
}