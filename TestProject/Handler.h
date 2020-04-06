#pragma once
#include "../ProudNetClient/NetClient.h" //cant be included below because of shitty windows header
#include <Windows.h>
#include <cstdint>

#include "../ProudNetClient/NetMessage.h"

class Handler
{
public:
	virtual void InitHandler() = 0;
	virtual void HandleRMI(Nettention::Proud::NetMessage& message, uint16_t rmiID) = 0;

protected:
	Handler(Nettention::Proud::NetClient* instance);
	Nettention::Proud::NetClient& GetClient();

private:
	Nettention::Proud::NetClient* Instance;
};