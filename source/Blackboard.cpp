#include "stdafx.h"
#include "Blackboard.h"

void Blackboard::HandlePlayerWithinPath()
{
    DE::Vector3f playerPos = myPlayer->GetTransform()->GetPosition();
    DE::Vector2f horizontalPos = { playerPos.x, playerPos.z };

    myPlayerWithinPath = false;
}

void Blackboard::SetCamera(DE::Camera* aCameraPtr)
{
    myCameraPtr = aCameraPtr;
}
