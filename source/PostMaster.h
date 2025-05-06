#pragma once
#include <vector>
#include <array>
#include "Message.h"

class Observer;

class PostMaster
{
public:
	PostMaster() = default;
	void Subscribe(const eMessageType aMessageType, Observer* anObserver);
	void Unsubscribe(const eMessageType aMessageType, Observer* anObserver);
	void TriggerMessage(const Message aMessage);

private:
	std::array<std::vector<Observer*>, static_cast<int>(eMessageType::DefaultTypeAndCount)> myObservers;
};