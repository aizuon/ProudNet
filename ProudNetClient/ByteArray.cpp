#include <limits>
#include <type_traits>

#include "ByteArray.h"
#include "../BaseLib/typedef.h"

namespace Nettention
{
	namespace Proud
	{
		ByteArray::ByteArray()
		{
			Buffer = new byte[0];
			Length = 0;
		}

		ByteArray::ByteArray(const ByteArray& byteArray)
		{
			Length = byteArray.Length;
			Buffer = new byte[Length];
			memcpy(Buffer, byteArray.Buffer, Length);
			WriteOffset = Length;
		}

		ByteArray::ByteArray(byte* buffer, size_t length)
		{
			Length = length;
			Buffer = new byte[Length];
			memcpy(Buffer, buffer, Length);
			WriteOffset = Length;
		}

		ByteArray::~ByteArray()
		{
			if (Buffer != nullptr)
			{
				delete[] Buffer;
				Buffer = nullptr;
			}
		}

		void ByteArray::Write(byte* obj, size_t length)
		{
			std::lock_guard<std::mutex> lock(mtx);

			if (Length <= WriteOffset + length)
			{
				size_t newLength = WriteOffset + length;
				byte* newBuffer = new byte[newLength];
				memcpy(newBuffer, Buffer, Length);
				delete[] Buffer;
				Buffer = newBuffer;
				Length = newLength;
			}

			memcpy(Buffer + WriteOffset, obj, length);
			WriteOffset += length;
		}

		void ByteArray::WriteScalar(int64_t obj)
		{
			if ((std::numeric_limits<sbyte>::min)() <= obj && obj <= (std::numeric_limits<sbyte>::max)())
			{
				sbyte scalar = 1;
				Write((byte*)&scalar, sizeof(sbyte));
				Write((byte*)&obj, sizeof(sbyte));
			}
			else if ((std::numeric_limits<int16_t>::min)() <= obj && obj <= (std::numeric_limits<int16_t>::max)())
			{
				sbyte scalar = 2;
				Write((byte*)&scalar, sizeof(sbyte));
				Write((byte*)&obj, sizeof(int16_t));
			}
			else if ((std::numeric_limits<int32_t>::min)() <= obj && obj <= (std::numeric_limits<int32_t>::max)())
			{
				sbyte scalar = 4;
				Write((byte*)&scalar, sizeof(sbyte));
				Write((byte*)&obj, sizeof(int32_t));
			}
			else
			{
				sbyte scalar = 8;
				Write((byte*)&scalar, sizeof(sbyte));
				Write((byte*)&obj, sizeof(int64_t));
			}
		}

		void ByteArray::WriteScalar(int32_t obj)
		{
			WriteScalar((int64_t)obj);
		}

		void ByteArray::WriteScalar(int16_t obj)
		{
			WriteScalar((int64_t)obj);
		}

		void ByteArray::WriteScalar(byte obj)
		{
			WriteScalar((int64_t)obj);
		}

		void ByteArray::Write(bool obj)
		{
			Write((byte*)&obj, sizeof(bool));
		}

		void ByteArray::Write(byte obj)
		{
			Write(&obj, sizeof(byte));
		}

		void ByteArray::Write(sbyte obj)
		{
			Write((byte*)&obj, sizeof(sbyte));
		}

		void ByteArray::Write(int16_t obj)
		{
			Write((byte*)&obj, sizeof(int16_t));
		}

		void ByteArray::Write(uint16_t obj)
		{
			Write((byte*)&obj, sizeof(uint16_t));
		}

		void ByteArray::Write(int32_t obj)
		{
			Write((byte*)&obj, sizeof(int32_t));
		}

		void ByteArray::Write(uint32_t obj)
		{
			Write((byte*)&obj, sizeof(uint32_t));
		}

		void ByteArray::Write(int64_t obj)
		{
			Write((byte*)&obj, sizeof(int64_t));
		}

		void ByteArray::Write(uint64_t obj)
		{
			Write((byte*)&obj, sizeof(uint64_t));
		}

		void ByteArray::Write(float obj)
		{
			Write((byte*)&obj, sizeof(float));
		}

		void ByteArray::Write(double obj)
		{
			Write((byte*)&obj, sizeof(double));
		}

		void ByteArray::Write(const ByteArray& obj)
		{
			WriteScalar((int64_t)obj.Length);
			Write(obj.Buffer, obj.Length);
		}

		bool ByteArray::Read(byte* obj, size_t length)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			if (Length >= ReadOffset + length)
			{
				memcpy(obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(bool& obj)
		{
			bool result = Read((byte&)obj);

			return result;
		}

		bool ByteArray::Read(byte& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			if (Length >= ReadOffset)
			{
				obj = Buffer[ReadOffset];
				ReadOffset++;

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(sbyte& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			if (Length >= ReadOffset)
			{
				obj = Buffer[ReadOffset];
				ReadOffset++;

				result = true;
			}

			return result;
		}

		bool ByteArray::GetSByte(sbyte& obj)
		{
			bool result = false;

			if (Length >= ReadOffset)
			{
				obj = Buffer[ReadOffset];

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(int16_t& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			size_t length = sizeof(int16_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::GetInt16(int16_t& obj)
		{
			bool result = false;

			size_t length = sizeof(int16_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(uint16_t& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			size_t length = sizeof(uint16_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::GetUInt16(uint16_t& obj)
		{
			bool result = false;

			size_t length = sizeof(uint16_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(int32_t& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			size_t length = sizeof(int32_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::GetInt32(int32_t& obj)
		{
			bool result = false;

			size_t length = sizeof(int32_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(uint32_t& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			size_t length = sizeof(uint32_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(int64_t& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			size_t length = sizeof(int64_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::GetInt64(int64_t& obj)
		{
			bool result = false;

			size_t length = sizeof(int64_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(uint64_t& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			size_t length = sizeof(uint64_t);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(float& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			size_t length = sizeof(float);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::Read(double& obj)
		{
			std::lock_guard<std::mutex> lock(mtx);
			bool result = false;

			size_t length = sizeof(double);

			if (Length >= ReadOffset + length)
			{
				memcpy(&obj, Buffer + ReadOffset, length);
				ReadOffset += length;

				result = true;
			}

			return result;
		}

		bool ByteArray::ReadScalar(int64_t& obj)
		{
			sbyte prefix = 0;
			if (!Read(prefix))
				return false;

			switch (prefix)
			{
			case 1:
			{
				sbyte _sbyte = 0;
				if (!Read(_sbyte))
					return false;
				obj = _sbyte;
				break;
			}
			case 2:
			{
				int16_t _int16_t = 0;
				if (!Read(_int16_t))
					return false;
				obj = _int16_t;
				break;
			}
			case 4:
			{
				int32_t _int32_t = 0;
				if (!Read(_int32_t))
					return false;
				obj = _int32_t;
				break;
			}
			case 8:
			{
				int64_t _int64_t = 0;
				if (!Read(_int64_t))
					return false;
				obj = _int64_t;
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool ByteArray::ReadScalar(int64_t& obj, sbyte& prefix)
		{
			prefix = 0;
			if (!Read(prefix))
				return false;

			switch (prefix)
			{
			case 1:
			{
				sbyte _sbyte = 0;
				if (!Read(_sbyte))
					return false;
				obj = _sbyte;
				break;
			}
			case 2:
			{
				int16_t _int16_t = 0;
				if (!Read(_int16_t))
					return false;
				obj = _int16_t;
				break;
			}
			case 4:
			{
				int32_t _int32_t = 0;
				if (!Read(_int32_t))
					return false;
				obj = _int32_t;
				break;
			}
			case 8:
			{
				int64_t _int64_t = 0;
				if (!Read(_int64_t))
					return false;
				obj = _int64_t;
				break;
			}
			default:
				return false;
			}

			return true;
		}

		bool ByteArray::GetScalar(int64_t& obj, sbyte& prefix)
		{
			prefix = 0;
			if (!GetSByte(prefix))
				return false;

			ReadOffset++;

			switch (prefix)
			{
			case 1:
			{
				sbyte _sbyte = 0;
				if (!GetSByte(_sbyte))
					return false;
				obj = _sbyte;
				break;
			}
			case 2:
			{
				int16_t _int16_t = 0;
				if (!GetInt16(_int16_t))
					return false;
				obj = _int16_t;
				break;
			}
			case 4:
			{
				int32_t _int32_t = 0;
				if (!GetInt32(_int32_t))
					return false;
				obj = _int32_t;
				break;
			}
			case 8:
			{
				int64_t _int64_t = 0;
				if (!GetInt64(_int64_t))
					return false;
				obj = _int64_t;
				break;
			}
			default:
				ReadOffset--;
				return false;
			}

			ReadOffset--;
			return true;
		}

		ByteArray ByteArray::ReadArray(bool& result)
		{
			result = false;

			int64_t length = 0;
			if (ReadScalar(length))
			{
				byte* data = new byte[length];
				if (Read(data, length))
				{
					result = true;

					ByteArray ret = ByteArray(data, length);
					delete[] data;
					return ret;
				}
				else
				{
					delete[] data;
				}
			}

			return ByteArray();
		}

		byte* ByteArray::ReadAll(size_t& length)
		{
			length = Length - ReadOffset;
			byte* buff = new byte[length];

			Read(buff, length);

			return buff;
		}

		byte* ByteArray::ReadArray(size_t length, bool& result)
		{
			byte* buff = new byte[length];

			Read(buff, length);

			return buff;
		}
	}
}