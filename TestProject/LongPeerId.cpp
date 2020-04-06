#include "LongPeerId.h"

LongPeerId::LongPeerId(uint64_t value)
	: m_PeerId((uint16_t)(value >> 48))
{
	AccountId = value & 0x0000FFFFFFFFFFFF;
}

LongPeerId::LongPeerId(uint64_t accId, uint32_t value)
	: m_PeerId((uint16_t)(value >> 16))
{
	AccountId = accId;
}

LongPeerId::LongPeerId(uint64_t accId, PeerId peerId)
	: m_PeerId(peerId)
{
	AccountId = accId;
}

LongPeerId::LongPeerId(uint64_t accId, byte id, byte slot, byte unk)
	: m_PeerId(id, slot, unk)
{
	AccountId = accId;
}

uint16_t LongPeerId::GetValue() const
{
	return AccountId | ((uint64_t)m_PeerId << 48);
}

LongPeerId::operator uint64_t() const
{
	return GetValue();
}

bool LongPeerId::operator==(const LongPeerId& longPeerIdB) const
{
	return GetValue() == longPeerIdB.GetValue();
}

bool LongPeerId::operator!=(const LongPeerId& longPeerIdB) const
{
	return !(*this == longPeerIdB);
}
