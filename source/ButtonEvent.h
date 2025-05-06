#pragma once
#include "UnityLoader.h"
#include "Observer.h"
#include "Message.h" 

class ButtonEvent : public Observer
{
public:
	ButtonEvent();
	~ButtonEvent();

	void Init(eUIButton aState); 

	void Receive(const Message& aMsg) override;
private:

	eMessageType myMessageType; 
};

