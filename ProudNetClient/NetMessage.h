#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

#include "Enums.h"
#include "ByteArray.h"
#include "../BaseLib/Version.h"
#include "../BaseLib/IPEndPoint.h"

namespace Nettention
{
    namespace Proud
    {
        class NetMessage : public ByteArray
        {
        public:
            bool Compress = false;
            bool Encrypt = false;
            EncryptMode m_EncryptMode = EncryptMode::None;
            bool Reliable = true;
            uint32_t RelayFrom = 0;

            NetMessage();
            NetMessage(const ByteArray& byteArray);
            NetMessage(byte* buffer, size_t length);

            bool Read(MessageType& obj);
            void Write(const NetMessage& obj);
            void Write(const std::string& obj, bool unicode = false);
            void Write(const GUID& obj);
            bool Read(GUID& obj);
            void Write(const BaseLib::Version& obj);
            bool Read(BaseLib::Version& obj);
            void WriteStringEndPoint(const BaseLib::IPEndPoint& obj);
            void Write(const BaseLib::IPEndPoint& obj);
            std::string ReadString(bool& result);
            bool ReadStringEndPoint(BaseLib::IPEndPoint& obj);
            bool ReadEndPoint(BaseLib::IPEndPoint& obj);

            NetMessage(const NetMessage& obj);
            NetMessage(NetMessage&& obj) noexcept;
            NetMessage& operator=(const NetMessage& obj);
            NetMessage& operator=(NetMessage&& obj) noexcept;

        private:
            void swap(NetMessage& obj) noexcept;
        };
    }
}