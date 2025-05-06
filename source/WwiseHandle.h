#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SimpleBlocking.h>
#include <AK/SoundEngine/Common/AkCallback.h>
#include <AK/SpatialAudio/Common/AkSpatialAudioTypes.h>
#include <AK/SoundEngine/Common/AkQueryParameters.h>

#include "DiffractionPath.h"

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>

class WwiseHandle
{
public:
	bool Init();
	void DeInit();
	AKRESULT LoadBank(const AkUniqueID& bankID);
	void RenderAudio();
	std::vector<DiffractionPath> GetDiffractionPaths(const AkGameObjectID& gameObjectID);
private:

	SimpleIOHook g_lowLevelIO;
	void Log(std::string_view logMsg);
};