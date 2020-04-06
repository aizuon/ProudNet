#pragma once
#include <Windows.h>
#include <atomic>

#include "Enums.h"

namespace Nettention
{
    namespace Proud
    {
        class NetCrypt
        {
        public:
            NetCrypt(size_t keySize, size_t fastKeySize);
            byte* Encrypt(byte* data, size_t incSize, size_t& outSize, EncryptMode mode, bool& result, bool reliable = true);
            byte* Decrypt(byte* data, size_t incSize, size_t& outSize, EncryptMode mode, bool& result, bool reliable = true);

            ~NetCrypt();

            NetCrypt(const NetCrypt& obj);
            NetCrypt(NetCrypt&& obj) noexcept;
            NetCrypt& operator=(const NetCrypt& obj);
            NetCrypt& operator=(NetCrypt&& obj) noexcept;

            size_t GetKeyLength()
            {
                return KeyLength;
            }

            size_t GetFastKeyLength()
            {
                return FastKeyLength;
            }

            byte* GetKey()
            {
                return Key;
            }

            byte* GetFastKey()
            {
                return FastKey;
            }

        private:
            constexpr static byte s_DefaultKey[16] = { 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
            std::atomic<int> _decryptCounter = 0;
            std::atomic<int> _encryptCounter = 0;

            byte* Key = nullptr;
            size_t KeyLength = 0;
            byte* FastKey = nullptr;
            size_t FastKeyLength = 0;
            size_t ServerFastKeyLength = 0;

            void swap(NetCrypt& obj) noexcept;
        };
    }
}