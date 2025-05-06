#include "GameObject.h"
#include "MainSingleton.h"
#include "DreamEngine/graphics/ModelInstancer.h"

GameObject::GameObject() : myID(MainSingleton::GetInstance()->GetNewID()),
myName("GameObject " + std::to_string(myID))
{
}
