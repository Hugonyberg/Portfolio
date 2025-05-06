#pragma once
#include "DreamEngine/math/Vector.h"
#include <string>
#include <unordered_map>
#include <fstream>
#include <DreamEngine/math/Transform.h>
#include "UnityLoader.h"
#include <type_traits>

class BinaryTemplate
{
public:
	BinaryTemplate() = default;
	~BinaryTemplate() = default;

	template <typename T>
	void SerializeData(std::ostream& anOut, const T& data);

	template <typename T>
	void DeserializeData(std::ifstream& anIn, const T& data);
private:

};

template <typename T>
void Serialize(std::ofstream& anOut, const T& data)
{
	if constexpr (std::is_enum_v<T>) 
	{
		// Handle enum serialization
		using UnderlyingType = typename std::underlying_type<T>::type;
		Serialize(anOut, static_cast<UnderlyingType>(data));
	}
	else 
	{
		// Handle non-enum serialization
		anOut.write(reinterpret_cast<const char*>(&data), sizeof(T));
	}
}

template <typename T>
void Deserialize(std::ifstream& anIn, T& data)
{
	if constexpr (std::is_enum_v<T>) 
	{
		// Handle enum deserialization
		using UnderlyingType = typename std::underlying_type<T>::type;
		UnderlyingType tempValue;
		Deserialize(anIn, tempValue);
		data = static_cast<T>(tempValue);
	}
	else 
	{
		// Handle non-enum deserialization
		anIn.read(reinterpret_cast<char*>(&data), sizeof(T));
	}
}

template <>
inline void Serialize<std::string>(std::ofstream& anOut, const std::string& aString)
{
	size_t length = aString.size();
	Serialize(anOut, length);
	anOut.write(aString.data(), length);
}

template <>
inline void Deserialize<std::string>(std::ifstream& anIn, std::string& aString)
{
	size_t length;
	Deserialize(anIn, length);
	aString.resize(length);
	anIn.read(&aString[0], length);
}

template <typename K, typename V>
inline void Serialize(std::ofstream& anOut, const std::unordered_map<K, V>& aMap)
{
	size_t size = aMap.size();
	Serialize(anOut, size);
	for (const auto& pair : aMap)
	{
		Serialize(anOut, pair.first);
		Serialize(anOut, pair.second);
	}

}

template <typename K, typename V>
inline void Deserialize(std::ifstream& anIn, std::unordered_map<K, V>& map) {
	size_t size;
	Deserialize(anIn, size); // Deserialize the map size
	for (size_t i = 0; i < size; ++i) {
		K key;
		V value;
		Deserialize(anIn, key);   // Deserialize the key
		Deserialize(anIn, value); // Deserialize the value
		map[key] = value;
	}
}

template <typename K, typename V>
inline void Serialize(std::ofstream& anOut, const std::map<K, V>& aMap) {
	size_t size = aMap.size();
	Serialize(anOut, size); // Write the number of elements

	for (const auto& pair : aMap) {
		Serialize(anOut, pair.first);  // Key (sorted order)
		Serialize(anOut, pair.second); // Value
	}
}

// --- Deserialize std::map ---
template <typename K, typename V>
inline void Deserialize(std::ifstream& anIn, std::map<K, V>& aMap) {
	size_t size;
	Deserialize(anIn, size); // Read the number of elements

	aMap.clear(); // Ensure the map is empty before inserting

	for (size_t i = 0; i < size; ++i) {
		K key;
		V value;
		Deserialize(anIn, key);   // Read key
		Deserialize(anIn, value); // Read value
		aMap[key] = value;        // Insert (automatically sorted)
	}
}

template <>
inline void Serialize<std::wstring>(std::ofstream& anOut, const std::wstring& wstr)
{
	size_t length = wstr.size();
	Serialize(anOut, length); // Serialize the length
	anOut.write(reinterpret_cast<const char*>(wstr.data()), length * sizeof(wchar_t));
}

template <>
inline void Deserialize<std::wstring>(std::ifstream& anIn, std::wstring& wstr) 
{
	size_t length;
	Deserialize(anIn, length); // Deserialize the length
	wstr.resize(length);
	anIn.read(reinterpret_cast<char*>(&wstr[0]), length * sizeof(wchar_t));
}



template <typename T>
inline void Serialize(std::ofstream& anOut, const std::vector<T>& aVector) 
{
	size_t size = aVector.size();
	Serialize(anOut, size);
	for (const auto& item : aVector) 
	{
		Serialize(anOut, item);
	}
}

template <typename T>
inline void Deserialize(std::ifstream& anIn, std::vector<T>& aVector)
{
	size_t size;
	Deserialize(anIn, size);
	aVector.resize(size);
	for (auto& item : aVector) 
	{
		Deserialize(anIn, item);
	}
}

template <>
inline void Serialize<DreamEngine::Vector3f>(std::ofstream& anOut, const DreamEngine::Vector3f& aVector3f) 
{
	Serialize(anOut, aVector3f.x);
	Serialize(anOut, aVector3f.y);
	Serialize(anOut, aVector3f.z);
}

template <>
inline void Deserialize<DreamEngine::Vector3f>(std::ifstream& anIn, DreamEngine::Vector3f& aVector3f)
{
	Deserialize(anIn, aVector3f.x);
	Deserialize(anIn, aVector3f.y);
	Deserialize(anIn, aVector3f.z);
}
template <>
inline void Serialize<physx::PxVec3>(std::ofstream& anOut, const physx::PxVec3& aPxVec3f)
{
	Serialize(anOut, aPxVec3f.x);
	Serialize(anOut, aPxVec3f.y);
	Serialize(anOut, aPxVec3f.z);
}

template <>
inline void Deserialize<physx::PxVec3>(std::ifstream& anIn, physx::PxVec3& aPxVec3f)
{
	physx::PxVec3 temp;
	Deserialize(anIn, temp.x);
	Deserialize(anIn, temp.y);
	Deserialize(anIn, temp.z);
	aPxVec3f = temp;
}
template <>
inline void Serialize<DreamEngine::Vector2f>(std::ofstream& anOut, const DreamEngine::Vector2f& aVector2f)
{
	Serialize(anOut, aVector2f.x);
	Serialize(anOut, aVector2f.y);
}

template <>
inline void Deserialize<DreamEngine::Vector2f>(std::ifstream& anIn, DreamEngine::Vector2f& aVector2f)
{
	Deserialize(anIn, aVector2f.x);
	Deserialize(anIn, aVector2f.y);
}

template <>
inline void Serialize<DreamEngine::Transform>(std::ofstream& anOut, const DreamEngine::Transform& aTransform)
{
	Serialize(anOut, aTransform.GetPosition());
	Serialize(anOut, aTransform.GetRotation());
	Serialize(anOut, aTransform.GetScale());
}

template <>
inline void Deserialize<DreamEngine::Transform>(std::ifstream& anIn, DreamEngine::Transform& aTransform)
{
	DreamEngine::Vector3f position;
	anIn.read(reinterpret_cast<char*>(&position), sizeof(DreamEngine::Vector3f));
	aTransform.SetPosition(position);
	// Write rotation (Vector3f as Rotator)
	DreamEngine::Rotator rotation;
	anIn.read(reinterpret_cast<char*>(&rotation), sizeof(DreamEngine::Vector3f));
	aTransform.SetRotation(rotation);

	// Write scale (Vector3f)
	DreamEngine::Vector3f scale;
	anIn.read(reinterpret_cast<char*>(&scale), sizeof(DreamEngine::Vector3f));
	aTransform.SetScale(scale);
}

template <>
inline void Serialize<DreamEngine::Color>(std::ofstream& anOut, const DreamEngine::Color& aColor)
{
	Serialize(anOut, aColor.myR);
	Serialize(anOut, aColor.myG);
	Serialize(anOut, aColor.myB);
	Serialize(anOut, aColor.myA);
}

template <>
inline void Deserialize<DreamEngine::Color>(std::ifstream& anIn, DreamEngine::Color& aColor)
{
	anIn.read(reinterpret_cast<char*>(&aColor), sizeof(aColor));
}

template <>
inline void Serialize<SpotLightData>(std::ofstream& anOut, const SpotLightData& aSpotLightData)
{
	Serialize(anOut, aSpotLightData.transform);
	Serialize(anOut, aSpotLightData.color);
	Serialize(anOut, aSpotLightData.intensity);
	Serialize(anOut, aSpotLightData.range);
}

template <>
inline void Deserialize<SpotLightData>(std::ifstream& anIn, SpotLightData& aSpotLightData)
{
	Deserialize(anIn, aSpotLightData.transform);
	Deserialize(anIn, aSpotLightData.color);
	Deserialize(anIn, aSpotLightData.intensity);
	Deserialize(anIn, aSpotLightData.range);
}

template <>
inline void Serialize<PointLightData>(std::ofstream& anOut, const PointLightData& aPointLightData)
{
	Serialize(anOut, aPointLightData.transform);
	Serialize(anOut, aPointLightData.color);
	Serialize(anOut, aPointLightData.intensity);
	Serialize(anOut, aPointLightData.range);
}

template <>
inline void Deserialize<PointLightData>(std::ifstream& anIn, PointLightData& aPointLightData)
{
	Deserialize(anIn, aPointLightData.transform);
	Deserialize(anIn, aPointLightData.color);
	Deserialize(anIn, aPointLightData.intensity);
	Deserialize(anIn, aPointLightData.range);
}
template <typename T, size_t N>
inline void Serialize(std::ofstream& anOut, const T(&array)[N]) 
{
	anOut.write(reinterpret_cast<const char*>(array), sizeof(T) * N);
}

template <typename T, size_t N>
inline void Deserialize(std::ifstream& anIn, T(&array)[N]) 
{
	anIn.read(reinterpret_cast<char*>(array), sizeof(T) * N);
}

template <>
inline void Serialize<MainCamera>(std::ofstream& anOut, const MainCamera& aMainCameraStruct)
{
	Serialize(anOut, aMainCameraStruct.transform);
	Serialize(anOut, aMainCameraStruct.foV);
	Serialize(anOut, aMainCameraStruct.farP);
	Serialize(anOut, aMainCameraStruct.nearP);
}

template <>
inline void Deserialize<MainCamera>(std::ifstream& anIn, MainCamera& aMainCameraStruct)
{
	Deserialize(anIn, aMainCameraStruct.transform);
	Deserialize(anIn, aMainCameraStruct.foV);
	Deserialize(anIn, aMainCameraStruct.farP);
	Deserialize(anIn, aMainCameraStruct.nearP);;
}

template <>
inline void Serialize<MainDirectionalLight>(std::ofstream& anOut, const MainDirectionalLight& aDirLightStruct)
{
	Serialize(anOut, aDirLightStruct.transform);
	Serialize(anOut, aDirLightStruct.color);
	Serialize(anOut, aDirLightStruct.intensity);

}

template <>
inline void Deserialize<MainDirectionalLight>(std::ifstream& anIn, MainDirectionalLight& aDirLightStruct)
{
	Deserialize(anIn, aDirLightStruct.transform);
	Deserialize(anIn, aDirLightStruct.color);
	Deserialize(anIn, aDirLightStruct.intensity);

}

template <>
inline void Serialize<MeshData>(std::ofstream& anOut, const MeshData& aMeshData)
{
	Serialize(anOut, aMeshData.vertices);
	Serialize(anOut, aMeshData.indices);
	Serialize(anOut, aMeshData.vertexCount);

}

template <>
inline void Deserialize<MeshData>(std::ifstream& anIn, MeshData& aMeshData)
{
	Deserialize(anIn, aMeshData.vertices);
	Deserialize(anIn, aMeshData.indices);
	Deserialize(anIn, aMeshData.vertexCount);

}

template <>
inline void Serialize<ColliderData>(std::ofstream& anOut, const ColliderData& aColliderData)
{
	Serialize(anOut, aColliderData.type);
	Serialize(anOut, aColliderData.size);
	Serialize(anOut, aColliderData.offset);
	Serialize(anOut, aColliderData.scale);
	Serialize(anOut, aColliderData.includeLayers);
	Serialize(anOut, aColliderData.excludeLayers);
	Serialize(anOut, aColliderData.meshData);
	Serialize(anOut, aColliderData.radius);
	Serialize(anOut, aColliderData.layerOverride);
	Serialize(anOut, aColliderData.isTrigger);
}

template <>
inline void Deserialize<ColliderData>(std::ifstream& anIn, ColliderData& aColliderData)
{
	Deserialize(anIn, aColliderData.type);
	Deserialize(anIn, aColliderData.size);
	Deserialize(anIn, aColliderData.offset);
	Deserialize(anIn, aColliderData.scale);
	Deserialize(anIn, aColliderData.includeLayers);
	Deserialize(anIn, aColliderData.excludeLayers);
	Deserialize(anIn, aColliderData.meshData);
	Deserialize(anIn, aColliderData.radius);
	Deserialize(anIn, aColliderData.layerOverride);
	Deserialize(anIn, aColliderData.isTrigger);

}

template <>
inline void Serialize<EnemySpawnerInfo>(std::ofstream& anOut, const EnemySpawnerInfo& aEnemySpawner)
{
	Serialize(anOut, aEnemySpawner.amountShooting);
	Serialize(anOut, aEnemySpawner.amountMelee);
	Serialize(anOut, aEnemySpawner.isShooting);
	Serialize(anOut, aEnemySpawner.isMelee);
	Serialize(anOut, aEnemySpawner.spawnerRange);
}

template <>
inline void Deserialize<EnemySpawnerInfo>(std::ifstream& anIn, EnemySpawnerInfo& aEnemySpawner)
{
	Deserialize(anIn, aEnemySpawner.amountShooting);
	Deserialize(anIn, aEnemySpawner.amountMelee);
	Deserialize(anIn, aEnemySpawner.isShooting);
	Deserialize(anIn, aEnemySpawner.isMelee);
	Deserialize(anIn, aEnemySpawner.spawnerRange);
}

template <>
inline void Serialize<LeviathanInfo>(std::ofstream& anOut, const LeviathanInfo& aLeviathanInfo)
{
	Serialize(anOut, aLeviathanInfo.attackCoolDown);
	Serialize(anOut, aLeviathanInfo.attackingSpeed);
	Serialize(anOut, aLeviathanInfo.attackRange);
	Serialize(anOut, aLeviathanInfo.seekingSpeed);
	Serialize(anOut, aLeviathanInfo.seekRange);
	Serialize(anOut, aLeviathanInfo.waypoints);
}

template <>
inline void Deserialize<LeviathanInfo>(std::ifstream& anIn, LeviathanInfo& aLeviathanInfo)
{
	Deserialize(anIn, aLeviathanInfo.attackCoolDown);
	Deserialize(anIn, aLeviathanInfo.attackingSpeed);
	Deserialize(anIn, aLeviathanInfo.attackRange);
	Deserialize(anIn, aLeviathanInfo.seekingSpeed);
	Deserialize(anIn, aLeviathanInfo.seekRange);
	Deserialize(anIn, aLeviathanInfo.waypoints);
}

