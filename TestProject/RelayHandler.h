#pragma once
#include "../ProudNetClient/NetClient.h" //cant be included below because of shitty windows header
#include <Windows.h>
#include <cstdint>

#include "Handler.h"
#include "../ProudNetClient/NetMessage.h"

class RelayHandler : public Handler
{
public:
	RelayHandler(Nettention::Proud::NetClient* client);

	void InitHandler();
	void HandleRMI(Nettention::Proud::NetMessage& message, uint16_t rmiID);
};