#pragma once
#include "BoatComponent.h"

class Player;

namespace DreamEngine
{
    class SpotLight;
};

class LightComponent : public BoatComponent 
{
public:
    void Init() override;

    void OnInteractSouthButton(Player& aPlayer) override;

    void OnInteractNorthButton(Player& aPlayer) override;
    void RenderLight(DreamEngine::GraphicsEngine& aGraphicsEngine) override;
private:
    bool myLightOn = true;
    std::shared_ptr<DE::SpotLight> myLight;
};
