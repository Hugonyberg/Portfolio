#include "Component.h"
#include "MainSingleton.h"

Component::Component(GameObject& aGameObject) :
	myGameObject(aGameObject),
	myComponentID(MainSingleton::GetInstance()->GetNewID())
{
}