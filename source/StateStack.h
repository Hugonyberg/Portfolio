#pragma once
#include <memory>
#include <stack>
#include "Observer.h"
#include "State.h"
#include "StateCredits.h"
#include "StateInGame.h"
#include "StateLevelSelect.h"
#include "StateMainMenu.h"
#include "StateSettings.h"
#include "StateSplash.h"
#include "StateTestingGym.h"
#include "StateLoadingScreen.h"
#include "BinaryExporter.h"
#include "UnityLoader.h"

class StateStack : public Observer
{
public:
	StateStack();
	~StateStack();

	void PushEnum(State::eStateName aEnum); 
	void Pop();
	size_t Size();
	std::shared_ptr<State> GetCurrentState();

	std::shared_ptr<State> GetState(State::eStateName aName);

	void Receive(const Message& aMsg) override;
	void ClearPhysXScene();

	void Render(DE::ModelDrawer& aModelDrawer, bool shouldRenderShadows);

private:
	std::stack<std::shared_ptr<State>> myStates;
	State::eStateName myLastState;
	State::eStateName myCurrentState = State::eStateName::Menu;

	std::shared_ptr<StateCredits> myStateCredit;
	std::shared_ptr<StateInGame> myStateInGame;
	std::shared_ptr<StateLevelSelect> myStateLevelSelect;
	std::shared_ptr<StateMainMenu> myStateMainMenu;
	std::shared_ptr<StateSettings> myStateSetting;
	std::shared_ptr<StateSplash> myStateSplash; 
	std::shared_ptr<StateLoadingScreen> myStateLoadingScreen; 

	//usage only for debug mode
	std::shared_ptr<StateTestingGym> myStatePlayerGym;
	std::shared_ptr<StateTestingGym> myStateAssetGym;

	std::vector<DE::ModelInstancer*> myModelInstancer;
	std::unordered_map<std::string, DE::ModelInstancer> myInstancersMap;

	LevelData myLevelOne;
	LevelData myLevelTwo;
	LevelData myPlayerGym;
	LevelData myAssetGym;

	std::vector<std::shared_ptr<Setdressing>> mySetdressingObjects;
	std::vector<std::shared_ptr<Setdressing>> myNotCulledSetdressingObjects;
	std::vector<std::shared_ptr<Setdressing>> mySetdressingObjectsWithCollision;
	
	BinaryExporter myBinaryExporter;

	bool myFirstLoadIn = true;
};