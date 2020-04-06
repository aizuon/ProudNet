#pragma once
#include <Windows.h>
#include <cstdint>

class PeerId
{
public:
	PeerId(uint16_t value);
	PeerId(byte id, byte slot, byte unk);

	byte Id = 0;
	byte Slot = 0;
	byte Category = 0;

	operator uint16_t() const;

	bool operator==(const PeerId& peerIdB) const;
	bool operator!=(const PeerId& peerIdB) const;

private:
	uint16_t GetValue() const;
};