#include "StateStack.h"
#include "MainSingleton.h"
#include "TextureLoading.h"
#include "Setdressing.h"
#include "JsonSoreter.hpp"

#include <locale>
#include <codecvt>

#include <DreamEngine/graphics/ModelFactory.h>
#include <DreamEngine/windows/settings.h>
#include <DreamEngine/ThreadPool.h>
#include <DreamEngine/graphics/ModelDrawer.h>

std::mutex loadingModelMutex;
std::mutex pushingModels;

void LoadModels(LevelData& aLevelData,
    std::vector<std::shared_ptr<Setdressing>>* someSetdressingObjects,
    std::vector<int> someUntaggedID, int startIndex, int endIndex, std::string aTagName, std::vector<DE::ModelInstancer*>* someModelInstancers, std::unordered_map<std::string, DE::ModelInstancer>* aInstancersMap);

int GetSplitAmount(int aAmountOfObjects);

void RunThreadsForModelLoading(LevelData& aLevelData,
    std::vector<std::shared_ptr<Setdressing>>* someSetdressingObjects,
    std::vector<int> someTaggedID, int aSplitAmount, std::string aTagName, std::vector<DE::ModelInstancer*>* someModelInstancers, std::unordered_map<std::string, DE::ModelInstancer>* aInstancersMap);


void LoadModels(LevelData& aLevelData,
    std::vector<std::shared_ptr<Setdressing>>* someSetdressingObjects,
    std::vector<int> someUntaggedID, int startIndex, int endIndex, std::string aTagName, std::vector<DE::ModelInstancer*>* someModelInstancers, std::unordered_map<std::string, DE::ModelInstancer>* aInstancersMap)
{
    for (size_t i = startIndex; i <= endIndex; i++)
    {
        const std::string modelPath(aLevelData.fbxPaths[someUntaggedID[i]].begin(), aLevelData.fbxPaths[someUntaggedID[i]].end());

        auto it = aInstancersMap->find(modelPath);

        if (it == aInstancersMap->end()) 
        {
            DE::ModelInstancer inst;
            std::wstring path = L"../Assets/";
            std::wstring pathTexture = L"../Assets/3D/T_";
            if (modelPath.size() > 0)
            {
                path += std::wstring(modelPath.begin(), modelPath.end());
                pathTexture += std::wstring(modelPath.begin()+3, modelPath.end()-4);
                //pathTexture += std::wstring(L"dds");
            }
            else
            {
                path = L"D:\Perforce P7\DreamEngine\Assets\3D\cubePrimitive.fbx";
            }

            std::shared_ptr<DE::Model> mdl = CustomLoad::LoadModel(path).GetModel();
            inst.Init(mdl);
            path = std::wstring() + std::wstring(path.begin(), path.end() - 4);
            CustomLoad::AssignMaterials(pathTexture, mdl, inst);
            aInstancersMap->insert(std::make_pair(modelPath, inst));

            loadingModelMutex.lock();
            someModelInstancers->push_back(&aInstancersMap->at(modelPath));
            loadingModelMutex.unlock();
        }
    }
}

void PushInstances(LevelData& aLevelData, std::vector<std::shared_ptr<Setdressing>>* someSetdressingObjects, std::vector<int> someUntaggedID,
                   std::vector<DE::ModelInstancer*>* someModelInstancers, std::unordered_map<std::string, DE::ModelInstancer>* aInstancersMap)
{
    for (int i = 0; i < someUntaggedID.size(); i++)
    {
        const std::string modelPath(aLevelData.fbxPaths[someUntaggedID[i]].begin(), aLevelData.fbxPaths[someUntaggedID[i]].end());
        auto it = aInstancersMap->find(modelPath);
        it->second.AddInstance(aLevelData.transforms[someUntaggedID[i]]);
        

        // Add collisions via PhysX
        std::shared_ptr<Setdressing> setdressingModel = std::make_shared<Setdressing>(Setdressing());
        if (aLevelData.fbxPaths[someUntaggedID[i]] != L"")
        {
            std::shared_ptr<DreamEngine::ModelInstance> modelI;
            loadingModelMutex.lock();
            modelI = std::make_shared<DreamEngine::ModelInstance>(DreamEngine::ModelFactory::GetInstance().GetModelInstance(aLevelData.fbxPaths[someUntaggedID[i]]));
            modelI->SetTransform(aLevelData.transforms[someUntaggedID[i]]);
            modelI->SetRotation(DE::Vector3f(std::round(modelI->GetTransform().GetRotation().x), std::round(modelI->GetTransform().GetRotation().y), std::round(modelI->GetTransform().GetRotation().z)));
            modelI->SetScale(DE::Vector3f(std::round(modelI->GetTransform().GetScale().x), std::round(modelI->GetTransform().GetScale().y), std::round(modelI->GetTransform().GetScale().z)));
            setdressingModel->SetTransform(modelI->GetTransform());
            loadingModelMutex.unlock();
            setdressingModel->SetModelInstance(modelI);
            pushingModels.lock();

            if (aLevelData.colliders.find(someUntaggedID[i]) != aLevelData.colliders.end())
            {
                setdressingModel->InitCollider(aLevelData.colliders[someUntaggedID[i]]);
            }
            setdressingModel->SetBoundingSphere();
            someSetdressingObjects->push_back(setdressingModel);
            pushingModels.unlock();
        }
    }
}

int GetSplitAmount(int aAmountOfObjects)
{
    int splitAmount = 0;
    int amountOfObjects = aAmountOfObjects;
    if (amountOfObjects % 4 == 0)
    {
        return splitAmount = amountOfObjects / 4;
    }
    else if (amountOfObjects >= 800)
    {
        return splitAmount = 200;
    }
    else if (amountOfObjects >= 400)
    {
        return splitAmount = 100;
    }
    else if (amountOfObjects >= 200)
    {
        return splitAmount = 50;
    }
    else if (amountOfObjects >= 100)
    {
        return splitAmount = 25;
    }
    else if (amountOfObjects >= 50)
    {
        return splitAmount = 12;
    }
    else if (amountOfObjects >= 20)
    {
        return splitAmount = 5;
    }
    return splitAmount;
}

void RunThreadsForModelLoading(LevelData& aLevelData,
    std::vector<std::shared_ptr<Setdressing>>* someSetdressingObjects,
    std::vector<int> someTaggedID, int aSplitAmount, std::string aTagName, std::vector<DE::ModelInstancer*>* someModelInstancers, std::unordered_map<std::string, DE::ModelInstancer>* aInstancersMap)
{
    LoadModels(aLevelData, someSetdressingObjects, someTaggedID, 0, static_cast<int>(someTaggedID.size() - 1), aTagName, someModelInstancers, aInstancersMap);
}



StateStack::StateStack()
{
    myStateCredit = std::make_shared<StateCredits>();
    myStateInGame = std::make_shared<StateInGame>();
    myStateLevelSelect = std::make_shared<StateLevelSelect>();
    myStateMainMenu = std::make_shared<StateMainMenu>();
    myStateSetting = std::make_shared<StateSettings>();
    myStateSplash = std::make_shared<StateSplash>();
    myStatePlayerGym = std::make_shared<StateTestingGym>(true);
    myStateAssetGym = std::make_shared<StateTestingGym>(false);
    myStateLoadingScreen = std::make_shared<StateLoadingScreen>();


    // Level 1
    //if (myBinaryExporter.BinaryOrJson("LevelOne.bin", "Json/Levelone.json"))
    {
        myLevelOne = UnityLoader::LoadLevel(DreamEngine::Settings::ResolveAssetPath("Json/LevelOne.json"));
        myBinaryExporter.ExportLevelDataToBinary(myLevelOne, "LevelOne.bin");
    }
    //else
    {
        //myBinaryExporter.ImportLevelDataFromBinary(myLevelOne, "LevelOne.bin");
    }

    // Level 2
    //if (myBinaryExporter.BinaryOrJson("LevelTwo.bin", "Json/LevelTwo.json"))
    {
        myLevelTwo = UnityLoader::LoadLevel(DreamEngine::Settings::ResolveAssetPath("Json/LevelTwo.json"));
        myBinaryExporter.ExportLevelDataToBinary(myLevelTwo, "LevelTwo.bin");
    }
    //else
    {
        //myBinaryExporter.ImportLevelDataFromBinary(myLevelTwo, "LevelTwo.bin");
    }

    myPlayerGym = UnityLoader::LoadLevel(DreamEngine::Settings::ResolveAssetPath("Json/PlayerGym.json"));
    
    myAssetGym = UnityLoader::LoadLevel(DreamEngine::Settings::ResolveAssetPath("Json/AssetGym.json"));
  

    auto& postMaster = MainSingleton::GetInstance()->GetPostMaster();
    postMaster.Subscribe(eMessageType::LevelOne, this);
    postMaster.Subscribe(eMessageType::LevelTwo, this);
    postMaster.Subscribe(eMessageType::CreditsScene, this);
    postMaster.Subscribe(eMessageType::LevelSelectScene, this);
    postMaster.Subscribe(eMessageType::MainMenuScene, this);
    postMaster.Subscribe(eMessageType::StartLevelScene, this);
    postMaster.Subscribe(eMessageType::SettingsScene, this);
    postMaster.Subscribe(eMessageType::PlayerGymScene, this);
    postMaster.Subscribe(eMessageType::AssetGymScene, this);
    postMaster.Subscribe(eMessageType::ExitProgram, this);
    postMaster.Subscribe(eMessageType::Return, this);
    postMaster.Subscribe(eMessageType::LoadingScreen, this);
}

StateStack::~StateStack()
{
    auto& postMaster = MainSingleton::GetInstance()->GetPostMaster();
    postMaster.Unsubscribe(eMessageType::LevelOne, this);
    postMaster.Unsubscribe(eMessageType::LevelTwo, this);
    postMaster.Unsubscribe(eMessageType::CreditsScene, this);
    postMaster.Unsubscribe(eMessageType::LevelSelectScene, this);
    postMaster.Unsubscribe(eMessageType::MainMenuScene, this);
    postMaster.Unsubscribe(eMessageType::StartLevelScene, this);
    postMaster.Unsubscribe(eMessageType::SettingsScene, this);
    postMaster.Unsubscribe(eMessageType::PlayerGymScene, this);
    postMaster.Unsubscribe(eMessageType::AssetGymScene, this);
    postMaster.Unsubscribe(eMessageType::ExitProgram, this);
    postMaster.Unsubscribe(eMessageType::Return, this);
    postMaster.Unsubscribe(eMessageType::LoadingScreen, this);
}

void StateStack::PushEnum(State::eStateName aEnum)
{
    switch(aEnum)
    {
    case State::eStateName::Menu:
    {
        if (myStates.top()->myStateName != State::eStateName::Splash)
        {
            ClearPhysXScene();
        }
        if(GetState(myCurrentState) != myStateMainMenu) { Pop(); }
        myStateMainMenu->Init();
        myStates.push(myStateMainMenu);
        MainSingleton::GetInstance()->GetAudioManager().StopNonMenuAudio();
        MainSingleton::GetInstance()->SetLastStateWasSettings(false);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
        MainSingleton::GetInstance()->GetPostMaster().TriggerMessage({ nullptr, eMessageType::ResetIntroMenu });
        break;
    }
    case State::eStateName::Settings:
    {
        myStateSetting->Init();
        myStates.push(myStateSetting);
        break;
    }
    case State::eStateName::InGame:
    {
        if(GetState(myCurrentState) != myStateMainMenu) { Pop(); }
        myStateInGame->SelectLevel((int)State::eStateName::LevelOne, myLevelOne);
        myStateInGame->Init();
        myStates.push(myStateInGame);
        break;
    }
    case State::eStateName::LevelSelect:
    {
        myStateLevelSelect->Init();
        myStates.push(myStateLevelSelect);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
        break;
    }
    case State::eStateName::Credit:
    {
        myStateCredit->Init();
        myStates.push(myStateCredit);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
        break;
    }
    case State::eStateName::LevelOne:
    {
        if(GetState(myCurrentState) != myStateMainMenu) { Pop(); }
        MainSingleton::GetInstance()->SetCurrentLevelNum(1);
        myStateInGame->SelectLevel((int)State::eStateName::LevelOne, myLevelOne);
        myStateInGame->Init();
        myStates.push(myStateInGame);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
        while (DE::Engine::GetInstance()->GetUpdateThreadPool().GetUnfinishedJobs() > 0)
        {
        }
        for (int i = 0; i < myModelInstancer.size(); i++)
        {
            myModelInstancer.at(i)->ClearInstances();
        }
        std::vector<int> setdressingIds = GetIdFromTag(myLevelOne.tags, "Setdressing");
        PushInstances(myLevelOne, &mySetdressingObjects, setdressingIds, &myModelInstancer, &myInstancersMap);
        setdressingIds = GetIdFromTag(myLevelOne.tags, "SetdressingWithCollider");
        PushInstances(myLevelOne, &mySetdressingObjects, setdressingIds, &myModelInstancer, &myInstancersMap);

        for (int i = 0; i < myModelInstancer.size(); i++)
	    {
            myModelInstancer.at(i)->RebuildInstances();
	    }
        break;
    }
    case State::eStateName::LevelTwo:
    {
        if(GetState(myCurrentState) != myStateMainMenu) { Pop(); }
        MainSingleton::GetInstance()->SetCurrentLevelNum(2);
        myStateInGame->SelectLevel((int)State::eStateName::LevelTwo, myLevelTwo);
        myStateInGame->Init();
        myStates.push(myStateInGame);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
        while (DE::Engine::GetInstance()->GetUpdateThreadPool().GetUnfinishedJobs() > 0)
        {
        }
        for (int i = 0; i < myModelInstancer.size(); i++)
        {
            myModelInstancer.at(i)->ClearInstances();
        }
        std::vector<int> setdressingIds = GetIdFromTag(myLevelTwo.tags, "Setdressing");
        PushInstances(myLevelTwo, &mySetdressingObjects, setdressingIds, &myModelInstancer, &myInstancersMap);
        setdressingIds = GetIdFromTag(myLevelTwo.tags, "SetdressingWithCollider");
        PushInstances(myLevelTwo, &mySetdressingObjects, setdressingIds, &myModelInstancer, &myInstancersMap);
        for (int i = 0; i < myModelInstancer.size(); i++)
        {
            myModelInstancer.at(i)->RebuildInstances();
        }
        break;
    }
    case State::eStateName::PlayerGym:
    {
        if(GetState(myCurrentState) != myStateMainMenu) { Pop(); }
        MainSingleton::GetInstance()->SetCurrentLevelNum(1);
        myStatePlayerGym->Init();
        myStates.push(myStatePlayerGym);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);

        std::vector<int> setdressingIds = GetIdFromTag(myPlayerGym.tags, "Setdressing");
        int splitAmount = GetSplitAmount(static_cast<int>(setdressingIds.size()));
        if (setdressingIds.size() > 0)
        {
            RunThreadsForModelLoading(myPlayerGym, &mySetdressingObjects, setdressingIds, splitAmount, "Setdressing", &myModelInstancer, &myInstancersMap);
        }

        std::vector<int> setdressingWithColliderIds = GetIdFromTag(myPlayerGym.tags, "SetdressingWithCollider");
        splitAmount = GetSplitAmount(static_cast<int>(setdressingWithColliderIds.size()));
        if (setdressingWithColliderIds.size() > 0)
        {
            RunThreadsForModelLoading(myPlayerGym, &mySetdressingObjectsWithCollision, setdressingWithColliderIds, splitAmount, "SetdressingWithCollider", &myModelInstancer, &myInstancersMap);
        }

        for (int i = 0; i < myModelInstancer.size(); i++)
        {
            myModelInstancer.at(i)->ClearInstances();
        }
        std::vector<int> setdressingsIds = GetIdFromTag(myPlayerGym.tags, "Setdressing");
        PushInstances(myPlayerGym, &mySetdressingObjects, setdressingsIds, &myModelInstancer, &myInstancersMap);
        setdressingsIds = GetIdFromTag(myPlayerGym.tags, "SetdressingWithCollider");
        PushInstances(myPlayerGym, &mySetdressingObjects, setdressingsIds, &myModelInstancer, &myInstancersMap);
        for (int i = 0; i < myModelInstancer.size(); i++)
        {
            myModelInstancer.at(i)->RebuildInstances();
        }

        break;
    }
    case State::eStateName::AssetGym:
    {
        if(GetState(myCurrentState) != myStateMainMenu) { Pop(); }
        MainSingleton::GetInstance()->SetCurrentLevelNum(1);
        myStateAssetGym->Init();
        myStates.push(myStateAssetGym);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);

        std::vector<int> setdressingIds = GetIdFromTag(myAssetGym.tags, "Setdressing");
        int splitAmount = GetSplitAmount(static_cast<int>(setdressingIds.size()));
        if (setdressingIds.size() > 0)
        {
            RunThreadsForModelLoading(myAssetGym, &mySetdressingObjects, setdressingIds, splitAmount, "Setdressing", &myModelInstancer, &myInstancersMap);
        }

        std::vector<int> setdressingWithColliderIds = GetIdFromTag(myAssetGym.tags, "SetdressingWithCollider");
        splitAmount = GetSplitAmount(static_cast<int>(setdressingWithColliderIds.size()));
        if (setdressingWithColliderIds.size() > 0)
        {
            RunThreadsForModelLoading(myAssetGym, &mySetdressingObjectsWithCollision, setdressingWithColliderIds, splitAmount, "SetdressingWithCollider", &myModelInstancer, &myInstancersMap);
        }

        for (int i = 0; i < myModelInstancer.size(); i++)
        {
            myModelInstancer.at(i)->ClearInstances();
        }
        std::vector<int> setdressingsIds = GetIdFromTag(myAssetGym.tags, "Setdressing");
        PushInstances(myAssetGym, &mySetdressingObjects, setdressingsIds, &myModelInstancer, &myInstancersMap);
        setdressingsIds = GetIdFromTag(myAssetGym.tags, "SetdressingWithCollider");
        PushInstances(myAssetGym, &mySetdressingObjects, setdressingsIds, &myModelInstancer, &myInstancersMap);
        for (int i = 0; i < myModelInstancer.size(); i++)
        {
            myModelInstancer.at(i)->RebuildInstances();
        }

        break;
    }
    case State::eStateName::Splash:
    {
        DE::Engine::GetInstance()->GetUpdateThreadPool().PushJob([this]
        {
            //Level 1
            std::vector<int> setdressingIds = GetIdFromTag(myLevelOne.tags, "Setdressing");
            int splitAmount = GetSplitAmount(static_cast<int>(setdressingIds.size()));
            if (setdressingIds.size() > 0)
            {
                RunThreadsForModelLoading(myLevelOne, &mySetdressingObjects, setdressingIds, splitAmount, "Setdressing", &myModelInstancer, &myInstancersMap);
            }

            std::vector<int> setdressingWithColliderIds = GetIdFromTag(myLevelOne.tags, "SetdressingWithCollider");
            splitAmount = GetSplitAmount(static_cast<int>(setdressingWithColliderIds.size()));
            if (setdressingWithColliderIds.size() > 0)
            {
                RunThreadsForModelLoading(myLevelOne, &mySetdressingObjectsWithCollision, setdressingWithColliderIds, splitAmount, "SetdressingWithCollider", &myModelInstancer, &myInstancersMap);
            }

            //Level 2
            setdressingIds = GetIdFromTag(myLevelTwo.tags, "Setdressing");
            splitAmount = GetSplitAmount(static_cast<int>(setdressingIds.size()));
            if (!setdressingIds.empty())
            {
                RunThreadsForModelLoading(myLevelTwo, &mySetdressingObjects, setdressingIds, splitAmount, "Setdressing", &myModelInstancer, &myInstancersMap);
            }

            setdressingWithColliderIds = GetIdFromTag(myLevelTwo.tags, "SetdressingWithCollider");
            splitAmount = GetSplitAmount(static_cast<int>(setdressingWithColliderIds.size()));
            if (setdressingWithColliderIds.size() > 0)
            {
                RunThreadsForModelLoading(myLevelTwo, &mySetdressingObjectsWithCollision, setdressingWithColliderIds, splitAmount, "SetdressingWithCollider", &myModelInstancer, &myInstancersMap);
            }
        });


        myCurrentState = State::eStateName::Splash;
        myStateSplash->Init();
        myStates.push(myStateSplash);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
        break;
    }
    case State::eStateName::LoadingScreen:
    {
        if(GetState(myCurrentState) != myStateMainMenu) { Pop(); }
        myCurrentState = State::eStateName::LoadingScreen;
        myStateLoadingScreen->Init();
        myStates.push(myStateLoadingScreen);
        MainSingleton::GetInstance()->SetShouldCaptureCursor(false);
        MainSingleton::GetInstance()->SetShouldRenderCursor(false);
        break;
    }
    case State::eStateName::Back:
    {
        Pop();
        break;
    }
    default:
    break;
    }

}

void StateStack::Pop()
{
    myStates.pop();
}

size_t StateStack::Size()
{
    return myStates.size();
}

std::shared_ptr<State> StateStack::GetCurrentState()
{
    if(!myStates.empty())
    {
        return myStates.top();
    }
    else
    {
        return nullptr;
    }
}

std::shared_ptr<State> StateStack::GetState(State::eStateName aName)
{
    switch(aName)
    {
    case State::eStateName::LevelOne:
    case State::eStateName::LevelTwo:
    case State::eStateName::InGame:
    {
        return myStateInGame;
        break;
    }
    case State::eStateName::PlayerGym:
    {
        return myStatePlayerGym;
        break;
    }
    case State::eStateName::Menu:
    {
        return myStateMainMenu;
        break;
    }
    case State::eStateName::Settings:
    {
        return myStateSetting;
        break;
    }
    case State::eStateName::LevelSelect:
    {
        return myStateLevelSelect;
        break;
    }
    case State::eStateName::Credit:
    {
        return myStateCredit;
        break;
    }
    case State::eStateName::Splash:
    {
        return myStateSplash;
        break;
    }
    case State::eStateName::LoadingScreen:
    {
        return myStateLoadingScreen;
        break;
    }
    }
    return std::shared_ptr<State>();
}

void StateStack::Receive(const Message& aMsg)
{
    switch(aMsg.messageType)
    {
    case eMessageType::StartLevelScene:
    {
        PushEnum(State::eStateName::InGame);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::InGame;
        break;
    }
    case eMessageType::MainMenuScene:
    {
        myStates.top()->DeleteState();
        auto& engine = *DreamEngine::Engine::GetInstance();
        while(engine.GetUpdateThreadPool().GetUnfinishedJobs() > 0)
        {
              //wait for threads to finish;
        }
        MainSingleton::GetInstance()->SetIsInCutscene(false);
        PushEnum(State::eStateName::Menu);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::Menu;
        break;
    }
    case eMessageType::LevelSelectScene:
    {
        PushEnum(State::eStateName::LevelSelect);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::LevelSelect;
        break;
    }
    case eMessageType::SettingsScene:
    {
        PushEnum(State::eStateName::Settings);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::Settings;
        break;
    }
    case eMessageType::CreditsScene:
    {
        PushEnum(State::eStateName::Credit);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::Credit;
        break;
    }
    case eMessageType::LevelOne:
    {
        PushEnum(State::eStateName::LevelOne);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::LevelOne;
        break;
    }
    case eMessageType::LevelTwo:
    {
        PushEnum(State::eStateName::LevelTwo);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::LevelTwo;
        break;
    }
    case eMessageType::PlayerGymScene:
    {
        PushEnum(State::eStateName::PlayerGym);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::PlayerGym;
        break;
    }
    case eMessageType::AssetGymScene:
    {
        PushEnum(State::eStateName::AssetGym);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::AssetGym;
        break;
    }
    case eMessageType::Return:
    {
        PushEnum(State::eStateName::Back);
        break;
    }
    case eMessageType::ExitProgram:
    {
        size_t size = Size();
        for(size_t i = 0; i < size; i++)
        {
            Pop();
        }
        break;
    }
    case eMessageType::LoadingScreen:
    {
        myStateLoadingScreen->SetStateToLoad(*static_cast<State::eStateName*>(aMsg.messageData));
        PushEnum(State::eStateName::LoadingScreen);
        myLastState = myCurrentState;
        myCurrentState = State::eStateName::LoadingScreen;
        break;
    }
    default:
    {
#ifndef _RETAIL
        std::cout << "Did not find the right Recived Message in StateStack" << std::endl;
#endif 
        break;
    }
    }
}

void StateStack::ClearPhysXScene()
{
    auto* physXScene = MainSingleton::GetInstance()->GetPhysXScene();
    std::vector<physx::PxActor*> actors;
    actors.resize(1000);
    unsigned int fetchedAmount;
    while (true)
    {
        fetchedAmount = UtilityFunctions::Min(physXScene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC), 1000u);
        physXScene->getActors(physx::PxActorTypeFlag::eRIGID_STATIC, &actors[0], fetchedAmount);
        physXScene->removeActors(&actors[0], fetchedAmount);
        if (fetchedAmount < 1000)
        {
            break;
        }
    }
    while (true)
    {
        fetchedAmount = UtilityFunctions::Min(physXScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC), 1000u);
        physXScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, &actors[0], fetchedAmount);
        physXScene->removeActors(&actors[0], fetchedAmount);
        if (fetchedAmount < 1000)
        {
            break;
        }
    }
}

void StateStack::Render(DE::ModelDrawer& aModelDrawer , bool shouldRenderShadows)
{
    for (int i = 0; i < myModelInstancer.size(); i++)
    {
        if (shouldRenderShadows == false)
        {
            aModelDrawer.DrawGBCalc(*myModelInstancer[i]);
        }
        else
        {
            aModelDrawer.DrawGBCalc(*myModelInstancer[i]);
        }
    }
}
