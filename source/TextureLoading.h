#pragma once
#include <DreamEngine/graphics/Model.h>
#include <DreamEngine/graphics/ModelInstance.h>
#include <DreamEngine/engine.h>
#include <DreamEngine/graphics/TextureResource.h>
#include <DreamEngine/graphics/TextureManager.h>
#include "DreamEngine/utilities/StringCast.h"
#include "DreamEngine/graphics/ModelInstancer.h"
#include "DreamEngine/graphics/AnimatedModelInstance.h"
#include <string>
#include <filesystem>

namespace CustomLoad
{
	DreamEngine::ModelInstance LoadModel(std::wstring& aPath, bool aAssignMaterial = true);
	DreamEngine::AnimatedModelInstance LoadAnimatedModel(std::wstring& aPath);
	void LoadTexture(DreamEngine::ModelInstance& aModelInstance, std::wstring& aPath);
	void LoadTexture(DreamEngine::AnimatedModelInstance& aModelInstance, std::wstring& aPath);

	void AssignMaterials(const std::wstring& someFilePath, std::shared_ptr<DreamEngine::Model> Mdl, DreamEngine::ModelInstancer& MI);
	void AssignMaterials(const std::wstring& someFilePath, DreamEngine::Model* Mdl, DreamEngine::ModelInstance& MI);
	void AssignMaterials(const std::wstring& someFilePath, DreamEngine::Model* Mdl, DreamEngine::AnimatedModelInstance& MI);
}