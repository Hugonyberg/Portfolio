#pragma once
#include <DreamEngine/math/Transform.h>
#include "UnityLoader.h"
#include <vector>
#include <string>
/// <summary>
/// Get all Id mapped to relevant tag
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="aData">: LevelData to parse</param>
/// <param name="aTag">: expected tag to find in aData</param>
/// <param name="aMap">: unordered map from in aData</param>
/// <returns>Vector list with aMap second element for all matching aTag found in aData based on matching ids</returns>
template <typename T>
static const std::vector<T> GetListFromData(const LevelData& aData, std::string aTag, std::unordered_map<int, T> aMap)
{
	std::vector<T> list;
	for each(auto & var in aData.tags)
	{
		if(var.second == aTag)
		{
			//copies level data
			list.push_back(aMap[var.first]);
		}
	}
	return list;
}
/// <summary>
/// Get all Id mapped to relevant tag
/// </summary>
/// <param name="aData">: a data to parse</param>
/// <param name="aTag">: a tag to find</param>
/// <returns> Vector list of all id mapped to aTag from aData</returns>
static const std::vector<int> GetIdFromTag(const std::unordered_map<int, std::string>& aMap, std::string aTag)
{
	std::vector<int> list;
	for each(auto & var in aMap)
	{
		if(var.second == aTag)
		{
			//copies level data
			list.push_back(var.first);
		}
	}
	return list;
}
/// <summary>
/// Get first id found with tag in a LevelData
/// </summary>
/// <param name="aData">: Data to look through for id</param>
/// <param name="aTag">: Tag to look for</param>
/// <returns>first id for specified tag, returns -1 if no valid tag found</returns>
static const int GetFirstIdFromTag(const std::unordered_map<int, std::string>& aMap, std::string aTag)
{
	for each(auto & var in aMap)
	{
		if(var.second == aTag)
		{
			return var.first;
		}
	}
	//Error code
	return -1;
}