#include <locale>

#include "NetMessage.h"
#include "../BaseLib/StringUtils.h"

namespace Nettention
{
    namespace Proud
    {
        NetMessage::NetMessage()
            : ByteArray()
        {
        }

        NetMessage::NetMessage(const ByteArray& byteArray)
            : ByteArray(byteArray)
        {
        }

        NetMessage::NetMessage(byte* buffer, size_t length)
            : ByteArray(buffer, length)
        {
        }

        bool NetMessage::Read(MessageType& obj)
        {
            byte _byte = 0;
            if (!ByteArray::Read(_byte))
                return false;
            obj = (MessageType)_byte;
            return true;
        }

        void NetMessage::Write(const NetMessage& obj)
        {
            ByteArray::Write(obj.Buffer, obj.Length);
            Compress = obj.Compress;
            Encrypt = obj.Encrypt;
            m_EncryptMode = obj.m_EncryptMode;
        }

        void NetMessage::Write(const std::string& obj, bool unicode)
        {
            ByteArray::Write(unicode ? (byte)2 : (byte)1);
            WriteScalar((int64_t)obj.length());
            if (unicode)
            {
                std::wstring unicode = BaseLib::Utils::StringUtils::ANSI2UNICODE(obj);
                ByteArray::Write((byte*)unicode.data(), unicode.length() * 2);
            }
            else
                ByteArray::Write((byte*)obj.data(), obj.length());
        }

        void NetMessage::Write(const GUID& obj)
        {
            ByteArray::Write((byte*)reinterpret_cast<const byte*>(&obj), sizeof(GUID));
        }

        bool NetMessage::Read(GUID& obj)
        {
            std::lock_guard<std::mutex> lock(mtx);
            bool result = false;

            size_t length = sizeof(GUID);

            if (Length >= ReadOffset + length)
            {
                memcpy(&obj, Buffer + ReadOffset, length);
                ReadOffset += length;

                result = true;
            }

            return result;
        }

        void NetMessage::Write(const BaseLib::Version& obj)
        {
            ByteArray::Write(4);
            ByteArray::Write((uint16_t)obj.Major);
            ByteArray::Write((uint16_t)obj.Minor);
            ByteArray::Write((uint16_t)obj.Build);
            ByteArray::Write((uint16_t)obj.Revision);
        }

        bool NetMessage::Read(BaseLib::Version& obj)
        {
            int _int = 0;
            uint16_t major = 0;
            uint16_t minor = 0;
            uint16_t build = 0;
            uint16_t revision = 0;
            if (ByteArray::Read(_int) && ByteArray::Read(major) &&
                ByteArray::Read(minor) && ByteArray::Read(build) &&
                ByteArray::Read(revision))
            {
                obj.Major = major;
                obj.Minor = minor;
                obj.Build = build;
                obj.Revision = revision;

                return true;
            }

            return false;
        }

        void NetMessage::WriteStringEndPoint(const BaseLib::IPEndPoint& obj)
        {
            Write(obj.IP, false);
            ByteArray::Write(obj.Port);
        }

        void NetMessage::Write(const BaseLib::IPEndPoint& obj)
        {
            byte* addressBytes = obj.GetAddressBytes();
            ByteArray::Write((uint32_t)((addressBytes[3] << 24) + (addressBytes[2] << 16) + (addressBytes[1] << 8)) + addressBytes[0]);
            delete[] addressBytes;
            ByteArray::Write(obj.Port);
        }

        std::string NetMessage::ReadString(bool& result)
        {
            std::string ret;
            result = false;

            byte prefix = 0;
            int64_t length = 0;

            if (!ByteArray::Read(prefix) || !ReadScalar(length))
                goto end;
            if (prefix == 2)
            {
                length = 2 * length;

                if (ReadOffset + length > Length)
                    goto end;

                byte* buff = new byte[length + 1];
                ByteArray::Read(buff, length);
                buff[length] = '\0';
                std::wstring unicode((const wchar_t*)buff);
                delete[] buff;
                ret = BaseLib::Utils::StringUtils::UNICODE2ANSI(unicode);
            }
            else
            {
                if (ReadOffset + length > Length)
                    goto end;

                byte* buff = new byte[length + 1];
                ByteArray::Read(buff, length);
                buff[length] = '\0';
                ret = std::string((const char*)buff);
                delete[] buff;
            }

            result = true;

        end:
            return ret;
        }

        bool NetMessage::ReadStringEndPoint(BaseLib::IPEndPoint& obj)
        {
            bool result1 = false;
            obj.IP = ReadString(result1);
            if (!result1 || !ByteArray::Read(obj.Port))
                return false;

            return true;
        }

        bool NetMessage::ReadEndPoint(BaseLib::IPEndPoint& obj)
        {
            uint32_t address = 0;
            if (!ByteArray::Read(address))
                return false;
            uint16_t port = 0;
            if (!ByteArray::Read(port))
                return false;
            obj.IP = BaseLib::IPEndPoint::AddressFromUInt32(address);
            obj.Port = port;
            return true;
        }

        NetMessage::NetMessage(const NetMessage& obj)
            : NetMessage(obj.Buffer, obj.Length)
        {
            std::copy(obj.Buffer, obj.Buffer + obj.Length, Buffer);
        }

        NetMessage::NetMessage(NetMessage&& obj) noexcept
            : NetMessage(obj.Buffer, obj.Length)
        {
            obj.Buffer = nullptr;
            obj.Length = 0;
        }

        NetMessage& NetMessage::operator=(const NetMessage& obj)
        {
            NetMessage temp(obj);

            temp.swap(*this);

            return *this;
        }

        NetMessage& NetMessage::operator=(NetMessage&& obj) noexcept
        {
            NetMessage temp(std::move(obj));

            temp.swap(*this);

            return *this;
        }

        void NetMessage::swap(NetMessage& obj) noexcept
        {
            std::swap(Buffer, obj.Buffer);
            std::swap(Length, obj.Length);
            std::swap(ReadOffset, obj.ReadOffset);
            std::swap(WriteOffset, obj.WriteOffset);
            std::swap(Compress, obj.Compress);
            std::swap(Encrypt, obj.Encrypt);
            std::swap(m_EncryptMode, obj.m_EncryptMode);
            std::swap(Reliable, obj.Reliable);
            std::swap(RelayFrom, obj.RelayFrom);
        }
    }
}