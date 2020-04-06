#pragma once
#include <Windows.h>
#include <cstdint>
#include <mutex>

#include "../BaseLib/typedef.h"

namespace Nettention
{
    namespace Proud
    {
        class ByteArray
        {
        public:
            ByteArray();
            ByteArray(const ByteArray& byteArray);
            ByteArray(byte* buffer, size_t length);

            ~ByteArray();

            size_t GetLength()
            {
                return Length;
            }

            uint32_t ReadOffset = 0;
            uint32_t WriteOffset = 0;

            byte* GetBuffer()
            {
                return Buffer;
            }

            void Write(byte* obj, size_t length);
            void WriteScalar(int64_t obj);
            void WriteScalar(int32_t obj);
            void WriteScalar(int16_t obj);
            void WriteScalar(byte obj);
            void Write(bool obj);
            void Write(byte obj);
			void Write(sbyte obj);
            void Write(int16_t obj);
            void Write(uint16_t obj);
            void Write(int32_t obj);
            void Write(uint32_t obj);
            void Write(int64_t obj);
            void Write(uint64_t obj);
            void Write(float obj);
            void Write(double obj);
            void Write(const ByteArray& obj);
            template<typename T>
            void WriteEnum(T obj)
            {
                typedef std::underlying_type<T>::type utype;

                if (std::is_same<utype, byte>::value)
                {
                    Write((byte*)&obj, sizeof(byte));
                }
                else if (std::is_same<utype, int16_t>::value)
                {
                    Write((byte*)&obj, sizeof(int16_t));
                }
                else if (std::is_same<utype, uint16_t>::value)
                {
                    Write((byte*)&obj, sizeof(uint16_t));
                }
                else if (std::is_same<utype, int32_t>::value)
                {
                    Write((byte*)&obj, sizeof(int32_t));
                }
                else if (std::is_same<utype, uint32_t>::value)
                {
                    Write((byte*)&obj, sizeof(uint32_t));
                }
                else if (std::is_same<utype, int64_t>::value)
                {
                    Write((byte*)&obj, sizeof(int64_t));
                }
                else if (std::is_same<utype, uint64_t>::value)
                {
                    Write((byte*)&obj, sizeof(uint64_t));
                }
            }

            bool Read(byte* obj, size_t length);
            bool Read(bool& obj);
            bool Read(byte& obj);
            bool Read(sbyte& obj);
            bool GetSByte(sbyte& obj);
            bool Read(int16_t& obj);
            bool GetInt16(int16_t& obj);
            bool Read(uint16_t& obj);
            bool GetUInt16(uint16_t& obj);
            bool Read(int32_t& obj);
            bool GetInt32(int32_t& obj);
            bool Read(uint32_t& obj);
            bool Read(int64_t& obj);
            bool GetInt64(int64_t& obj);
            bool Read(uint64_t& obj);
            bool Read(float& obj);
            bool Read(double& obj);
            bool ReadScalar(int64_t& obj);
			bool ReadScalar(int64_t& obj, sbyte& prefix);
            bool GetScalar(int64_t& obj, sbyte& prefix);
            ByteArray ReadArray(bool& result);
            byte* ReadAll(size_t& length);
            byte* ReadArray(size_t length, bool& result);

        protected:
            std::mutex mtx;
            byte* Buffer;
            size_t Length;
        };
    }
}