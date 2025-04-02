#include "stdafx.h"
#include "HUD.h"

#include <tge/engine.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/SpriteDrawer.h>

#include "tge\engine.h"
#include "MainSingleton.h"

const std::unordered_set<std::string> HUD::myAllowedMessages = {
    "Gather Gold",
    "Gather Stone",
    "Gather Food",
    "Gather Wood",
    "Build House",
    "Spawn Farmer",
    "Spawn Soldier"
};

HUD::HUD()
{
}

HUD::~HUD()
{
}

void HUD::Init()
{
    auto& textureManager = Tga::Engine::GetInstance()->GetTextureManager();

    myBackgroundTexture.myTexture = textureManager.GetTexture(L"Sprites/UI/Banners/Carved_3Slides_Remake.png");
    
    myBannerTexture.myTexture = textureManager.GetTexture(L"Sprites/UI/Banners/Banner_Horizontal.png");

    myFoodTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Resources/Food.png"); // Food
    myStoneTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Resources/Stone.png"); // Stone
    myWoodTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Resources/Wood.png"); // Wood
    myGoldTexture.myTexture = textureManager.GetTexture(L"Sprites/Resources/Resources/Gold.png"); // Gold

    myPopulationTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Troops/Pawn/Purple/Pawn_Purple_Solo.png");
    mySoldierTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Troops/Warrior/Purple/Warrior_Purple_Solo.png");
    myHousesTexture.myTexture = textureManager.GetTexture(L"Sprites/Factions/Knights/Buildings/House/House_Purple.png");


    mySpriteData.myPivot = { 0.5f, 0.5f };
    mySpriteData.mySize = { MainSingleton::GetInstance().GetTileSize() * 3.f, MainSingleton::GetInstance().GetTileSize() * 3.f };


    /**************************MESSAGES************************/
    float tileSize = MainSingleton::GetInstance().GetTileSize();
    int height = MainSingleton::GetInstance().GetHeight();

    Tga::Vector2f msgBgSize = { 10 * tileSize, 2 * tileSize };
    Tga::Vector2f msgBgTopLeft = { 0.f, tileSize * height - msgBgSize.y };

    myStartingMessagePosition = msgBgTopLeft; 
    myStartingMessagePosition.x += tileSize;
    myStartingMessagePosition.y += tileSize;
}

void HUD::Update(float aDeltaTime)
{
    myCurrentWorldState = MainSingleton::GetInstance().GetWorldState();
    float tileSize = MainSingleton::GetInstance().GetTileSize();
    float bannerWidth = 6 * tileSize; // required gap between messages

    if (myMessages.empty())
        return;

    myMessages[0].position.x += myMessages[0].speed * aDeltaTime;
    if (myMessages[0].position.x > Tga::Engine::GetInstance()->GetRenderSize().x)
    {
        myMessages.erase(myMessages.begin());
        if (myMessages.empty())
            return;
    }

    for (size_t i = 1; i < myMessages.size(); i++)
    {
        float movedDistance = myMessages[i - 1].position.x - myStartingMessagePosition.x;
        if (movedDistance < bannerWidth)
        {
            break;
        }
        else
        {
            myMessages[i].position.x += myMessages[i].speed * aDeltaTime;
            if (myMessages[i].position.x > Tga::Engine::GetInstance()->GetRenderSize().x)
            {
                myMessages.erase(myMessages.begin() + i);
                i--;
            }
        }
    }
}

void HUD::Render(Tga::SpriteDrawer& aSpriteDrawer)
{
    RenderResources(aSpriteDrawer);
    RenderMessages(aSpriteDrawer);
}

void HUD::AddMessage(HUDMessage& aMessage)
{
    if (myAllowedMessages.find(aMessage.text) != myAllowedMessages.end())
    {
        aMessage.position = myStartingMessagePosition;
        myMessages.push_back(aMessage);
    }
    else
    {
        std::cout << "Invalid Message" << std::endl;
    }
}

void HUD::RenderResources(Tga::SpriteDrawer& aSpriteDrawer)
{
    auto& singleton = MainSingleton::GetInstance();
    auto& engine = *Tga::Engine::GetInstance();

    Tga::Vector2ui intResolution = engine.GetRenderSize();
    Tga::Vector2f resolution = { static_cast<float>(intResolution.x), static_cast<float>(intResolution.y) };

    float tileSize = singleton.GetTileSize();
    
    // Calculate HUD background dimensions and position.
    Tga::Vector2f bgSize = { 30 * tileSize, 3 * tileSize };
    Tga::Vector2f screenCenter = { resolution.x / 2.f, bgSize.y / 2.f };
    Tga::Vector2f bgCenter = screenCenter;
    Tga::Vector2f bgTopLeft = { bgCenter.x - bgSize.x / 2.f, bgCenter.y - bgSize.y / 2.f };

    // Draw the HUD background.
    mySpriteData.myPosition = bgCenter;  // With pivot 0.5, this centers the sprite.
    mySpriteData.mySize = bgSize;
    aSpriteDrawer.Draw(myBackgroundTexture, mySpriteData);

    // Set parameters for the banners and resource icons.
    float bannerSize = 3.f * tileSize;
    float bannerSizeX = 6.f * tileSize;
    float resourceIconSize = 1.f * tileSize;
    float bannerSpacing = tileSize;
    float startingOffset = tileSize * 1.5f;

    std::vector<std::pair<Tga::SpriteSharedData, int>> indicators = 
    {
        { myFoodTexture, myCurrentWorldState.food },
        { myWoodTexture, myCurrentWorldState.wood },
        { myStoneTexture, myCurrentWorldState.stone },
        { myGoldTexture, myCurrentWorldState.gold },
        { myPopulationTexture, myCurrentWorldState.numOfFarmers },
        { mySoldierTexture, myCurrentWorldState.numOfSoldiers },
        { myHousesTexture, myCurrentWorldState.houses }
    };

    // Loop over each resource indicator and draw its banner, icon, and text.
    for (size_t i = 0; i < indicators.size(); ++i)
    {
        Tga::Vector2f bannerCenter = 
        {
            startingOffset + bgTopLeft.x + (bannerSize / 2.f) + i * (bannerSize + bannerSpacing),
            bgTopLeft.y + bgSize.y / 2.f
        };

        // Draw the banner.
        mySpriteData.myPosition = bannerCenter;
        mySpriteData.mySize = { bannerSizeX, bannerSize };
        aSpriteDrawer.Draw(myBannerTexture, mySpriteData);

        Tga::Vector2f resourceIconPos = {
            bannerCenter.x - bannerSize / 6.f,
            bannerCenter.y
        };

        mySpriteData.myPosition = resourceIconPos;
        mySpriteData.mySize = { resourceIconSize, resourceIconSize };
        aSpriteDrawer.Draw(indicators[i].first, mySpriteData);

        // Calculate the position for the text (placed to the right of the icon).
        Tga::Vector2f textPos =
        {
            resourceIconPos.x + resourceIconSize / 4.f + tileSize / 2.f,
            bannerCenter.y - tileSize / 4.f
        };


        myText.SetScale(1.f);
        if (i == 4)
        {
            textPos.x -= tileSize / 6.5f;
            myText.SetScale(0.8f);
            myText.SetText(std::to_string(myCurrentWorldState.numOfFarmers) + "/" + std::to_string(myCurrentWorldState.maxNumOfFarmers));
        }
        else if (i == 5)
        {
            textPos.x -= tileSize / 6.5f;
            myText.SetScale(0.8f);
            myText.SetText(std::to_string(myCurrentWorldState.numOfSoldiers) + "/" + std::to_string(myCurrentWorldState.maxNumOfSoldiers));
        }
        else
        {
            myText.SetText(std::to_string(indicators[i].second));
        }

        myText.SetPosition(textPos);
        myText.SetColor({ 0.f, 0.f, 0.f, 1.f });
        myText.Render();
    }
}

void HUD::RenderMessages(Tga::SpriteDrawer& aSpriteDrawer)
{
    auto& engine = *Tga::Engine::GetInstance();
    Tga::Vector2ui intResolution = engine.GetRenderSize();
    float tileSize = MainSingleton::GetInstance().GetTileSize();

    int height = MainSingleton::GetInstance().GetHeight();

    // Define message background dimensions:
    Tga::Vector2f msgBgSize = { 10 * tileSize, 2 * tileSize };
    Tga::Vector2f msgBgTopLeft = { 0.f, tileSize * height - msgBgSize.y };
    Tga::Vector2f msgBgCenter = { msgBgTopLeft.x + msgBgSize.x / 2.f, msgBgTopLeft.y + msgBgSize.y / 2.f };

    Tga::Vector2f msgPos = msgBgTopLeft;
    std::string messageText = "Orders from the King -> ";
    msgPos.x += tileSize;
    msgPos.y += tileSize/2.f;

    // Render each message text.
    for (auto& message : myMessages)
    {
        float movedDistance = message.position.x - myStartingMessagePosition.x;
        float bannerWidth = 6 * tileSize; // required gap between messages
        if (movedDistance < bannerWidth)
        {
            break;
        }

        Tga::Vector2f messageBannerSize = { 8 * tileSize, 2 * tileSize };
        mySpriteData.myPosition = message.position;
        mySpriteData.mySize = messageBannerSize;
        aSpriteDrawer.Draw(myBannerTexture, mySpriteData);

        float offsetX = messageBannerSize.x / 4.5f;
        float offsetY = messageBannerSize.y / 6.f;
        Tga::Vector2f offsetPosition = message.position;
        offsetPosition.x -= offsetX;
        offsetPosition.y -= offsetY;

        myText.SetScale(1.f);
        myText.SetPosition(offsetPosition);
        myText.SetText(message.text);
        myText.SetColor({ 0.f, 0.f, 0.f, 1.f });
        myText.Render();
    }

    mySpriteData.myPosition = msgBgCenter;
    mySpriteData.mySize = msgBgSize;
    aSpriteDrawer.Draw(myBackgroundTexture, mySpriteData);
    myText.SetPosition(msgPos);
    myText.SetScale(1.3f);
    myText.SetText(messageText);
    myText.SetColor({ 0.f, 0.f, 0.f, 1.f });
    myText.Render();
}
