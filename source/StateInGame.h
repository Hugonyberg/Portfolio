#pragma once
#include "State.h"
#include "Observer.h"
#include "Scene.h"
#include "BinaryExporter.h"

class StateInGame : public State, public Observer
{
public:
	StateInGame();
	~StateInGame();
	void DeleteState() override;

	void Init() override;
	void Update(float aDeltaTime) override;
	void Render() override;

	void SelectLevel(int aSelectedLevel, LevelData aLevelData);
	void Receive(const Message& aMsg) override;

private:
	Scene* myScene; 

	BinaryExporter myBinaryExporter;
};

