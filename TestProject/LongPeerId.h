#pragma once
#include <Windows.h>
#include <cstdint>

#include "PeerId.h"

class LongPeerId
{
public:
	LongPeerId(uint64_t value);
	LongPeerId(uint64_t accId, uint32_t value);
	LongPeerId(uint64_t accId, PeerId peerId);
	LongPeerId(uint64_t accId, byte id, byte slot, byte unk);

	uint64_t AccountId = 0;
	PeerId m_PeerId;

	operator uint64_t() const;

	bool operator==(const LongPeerId& longPeerIdB) const;
	bool operator!=(const LongPeerId& longPeerIdB) const;

private:
	uint16_t GetValue() const;
};