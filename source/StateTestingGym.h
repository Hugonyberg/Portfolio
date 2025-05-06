#pragma once
#include "State.h"
#include "Observer.h"
#include "Scene.h"
#include "BinaryExporter.h"
#include "Navmesh.h"
#include "NavmeshCreationTool.h"

class StateTestingGym: public State, public Observer
{
public:
	StateTestingGym() = delete;
	StateTestingGym(bool aIsPlayerGym = true);
	//~StatePlayerGym();

	void DeleteState() override;

	void Init() override;
	void Update(float aDeltaTime) override;
	void Render() override;

	void Receive(const Message& aMsg) override;

private:
	Scene* myScene;
	BinaryExporter myBinaryExporter;
	bool myIsPlayerGym;
};

