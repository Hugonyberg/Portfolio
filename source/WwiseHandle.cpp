#include "WwiseHandle.h"
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/AkFilePackage.h>
#include <AK/AkFilePackageLUT.h>
#ifdef _DEBUG
#include "AK/Comm/AkCommunication.h"
#endif
#include "AK/SpatialAudio/Common/AkSpatialAudio.h"
#include <AK/MusicEngine/Common/AkMusicEngine.h>

#include <DreamEngine\math\Vector.h>
#include <DreamEngine\utilities\StringCast.h>

#include <AK/Wwise_IDs.h>

#include <DreamEngine/windows/settings.h>

bool WwiseHandle::Init()
{
	// Memory
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);
	AK::MemoryMgr::Init(&memSettings);

	// Streaming
	AkStreamMgrSettings streamSettings;
	AK::StreamMgr::GetDefaultSettings(streamSettings);
	AK::StreamMgr::Create(streamSettings);

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
	g_lowLevelIO.Init(deviceSettings);
	AK::StreamMgr::SetFileLocationResolver(&g_lowLevelIO);

	// Sound Engine
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;

	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	// Increase command queue size to help avoid deadlock
	initSettings.uCommandQueueSize = 4 * 1024 * 1024; // 4 MB instead of default 2 MB

	AK::SoundEngine::Init(&initSettings, &platformInitSettings);

	// Spatial Audio
	AkSpatialAudioInitSettings spatialAudioInitSettings;
	AK::SpatialAudio::Init(spatialAudioInitSettings);

	// Music Engine
	AkMusicSettings musicInitSettings;
	AK::MusicEngine::GetDefaultInitSettings(musicInitSettings);
	AK::MusicEngine::Init(&musicInitSettings);

	// File system base path
	std::wstring soundBankPath = DE::Settings::ResolveAssetPathW(L"GeneratedSoundBanks/Windows");
	g_lowLevelIO.SetBasePath(soundBankPath.c_str());

	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

#ifdef _DEBUG
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	AK::Comm::Init(commSettings);
#endif

	return true;
}


void WwiseHandle::DeInit()
{
	AK::SoundEngine::StopAll();
#ifdef _DEBUG 
	AK::Comm::Term();
#endif

	AK::MusicEngine::Term();
	AK::SoundEngine::Term();
	g_lowLevelIO.Term();
	if (AK::IAkStreamMgr::Get())
	{
		AK::IAkStreamMgr::Get()->Destroy();
	}
	AK::MemoryMgr::Term();
}

AKRESULT WwiseHandle::LoadBank(const AkUniqueID& bankID)
{
	return AK::SoundEngine::LoadBank(bankID);
}

void WwiseHandle::RenderAudio()
{
	AK::SoundEngine::RenderAudio();
}

std::vector<DiffractionPath> WwiseHandle::GetDiffractionPaths(const AkGameObjectID& gameObjectID)
{
	AkVector64 emitterPos;
	AkVector64 listenerPos;
	AkDiffractionPathInfo akDiffractionPaths[8];
	AkUInt32 numberOfPaths = 8;
	AK::SpatialAudio::QueryDiffractionPaths(gameObjectID, 0, listenerPos, emitterPos, akDiffractionPaths, numberOfPaths);

	std::vector<DiffractionPath> diffractionPaths;
	diffractionPaths.reserve(numberOfPaths);

	for (AkUInt32 path = 0; path < numberOfPaths; path++)
	{
		const AkDiffractionPathInfo& akDiffractionPath = akDiffractionPaths[path];
		DiffractionPath diffractionPath;

		for (size_t i = 0; i < akDiffractionPath.nodeCount; i++)
		{
			const AkVector64& akNode = akDiffractionPath.nodes[i];
			DE::Vector3f& convertedNode = diffractionPath.nodes[i];

			convertedNode.x = static_cast<float>(akNode.X);
			convertedNode.y = static_cast<float>(akNode.Y);
			convertedNode.z = static_cast<float>(-akNode.Z);

			diffractionPath.angles[i] = akDiffractionPath.angles[i];
			diffractionPath.portals[i] = static_cast<int>(akDiffractionPath.portals[i]);
			diffractionPath.rooms[i] = static_cast<int>(akDiffractionPath.rooms[i]);
		}

		diffractionPath.emitterPos.x = static_cast<float>(emitterPos.X);
		diffractionPath.emitterPos.y = static_cast<float>(emitterPos.Y);
		diffractionPath.emitterPos.z = static_cast<float>(emitterPos.Z);

		const AkVector& akOrientationFront = akDiffractionPath.virtualPos.OrientationFront();
		diffractionPath.virtualPos.forward.x = -akOrientationFront.X;
		diffractionPath.virtualPos.forward.y = -akOrientationFront.Y;
		diffractionPath.virtualPos.forward.z = akOrientationFront.Z;

		const AkVector& akOrientationTop = akDiffractionPath.virtualPos.OrientationTop();
		diffractionPath.virtualPos.up.x = akOrientationTop.X;
		diffractionPath.virtualPos.up.y = akOrientationTop.Y;
		diffractionPath.virtualPos.up.z = -akOrientationTop.Z;

		const AkVector64 akPosition = akDiffractionPath.virtualPos.Position();
		diffractionPath.virtualPos.position.x = static_cast<float>(akPosition.X);
		diffractionPath.virtualPos.position.y = static_cast<float>(akPosition.Y);
		diffractionPath.virtualPos.position.z = static_cast<float>(-akPosition.Z);

		diffractionPath.nodeCount = akDiffractionPath.nodeCount;

		diffractionPath.diffraction = akDiffractionPath.diffraction;

		diffractionPath.totLength = akDiffractionPath.totLength;

		diffractionPath.obstructionValue = akDiffractionPath.obstructionValue;


		diffractionPaths.emplace_back(std::move(diffractionPath));
	}
	return diffractionPaths;
}

void WwiseHandle::Log(std::string_view logMsg)
{
	std::cout << logMsg << std::endl;
}