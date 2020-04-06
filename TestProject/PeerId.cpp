#include "PeerId.h"

PeerId::PeerId(uint16_t value)
{
    Id = (byte)((uint32_t)value >> 8);
    Slot = (byte)((value >> 3) & 31);
    Category = (byte)(value & 7);
}

PeerId::PeerId(byte id, byte slot, byte unk)
{
    Id = id;
    Slot = slot;
    Category = unk;
}

uint16_t PeerId::GetValue() const
{
    return (uint16_t)(((uint32_t)Id << 8) | (uint16_t)((uint32_t)(8 * (Slot & 31)) | (uint16_t)(Category & 7U)));
}

PeerId::operator uint16_t() const
{
    return GetValue();
}

bool PeerId::operator==(const PeerId& peerIdB) const
{
    return GetValue() == peerIdB.GetValue();
}

bool PeerId::operator!=(const PeerId& peerIdB) const
{
    return !(*this == peerIdB);
}
