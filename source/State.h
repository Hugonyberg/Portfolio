#pragma once
#include <memory> 

class State
{
public:
	enum class eStateName
	{
		Menu = 0,
		Start,
		Settings,
		InGame,
		LevelSelect,
		Credit,

		LevelOne,
		LevelTwo,
		PlayerGym,
		AssetGym,

		Splash,
		LoadingScreen,

		Back
	};
	virtual void Init() = 0;
	virtual void Update(float aDeltaTime) = 0;
	virtual void Render() = 0;
	virtual void DeleteState() {};
	virtual void ExitState(); 

	eStateName myStateName;
};
