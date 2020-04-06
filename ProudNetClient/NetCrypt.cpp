#include <openssl/rand.h>
#include <exception>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rc4.h>
#include <limits>
#include <fmt/format.h>
#include <algorithm>

#include "NetCrypt.h"
#include "NetMessage.h"
#include "../BaseLib/CRC32.h"

namespace Nettention
{
	namespace Proud
	{
		NetCrypt::NetCrypt(size_t keySize, size_t fastKeySize)
		{
			if (keySize == 0)
			{
				KeyLength = 16;
				Key = new byte[KeyLength];
				memcpy(Key, s_DefaultKey, KeyLength);
			}
			else
			{
				size_t byteLength = keySize / 8;
				KeyLength = byteLength;
				Key = new byte[KeyLength];
				if (!RAND_bytes(Key, KeyLength))
				{
					throw std::exception("AES key generation failed");
				}
			}

			if (fastKeySize == 0)
			{
				FastKeyLength = 16;
				FastKey = new byte[FastKeyLength];
				memcpy(FastKey, s_DefaultKey, FastKeyLength);
			}
			else
			{
				size_t byteLength = keySize / 8;
				FastKeyLength = byteLength;
				ServerFastKeyLength = byteLength;
				FastKey = new byte[FastKeyLength];
				if (!RAND_bytes(FastKey, FastKeyLength))
				{
					throw std::exception("RC4 key generation failed");
				}
			}
		}

		byte* NetCrypt::Encrypt(byte* data, size_t incSize, size_t& outSize, EncryptMode mode, bool& result, bool reliable)
		{
			result = false;

			switch (mode)
			{
			case EncryptMode::Secure:
			{
				uint32_t blockSize_bytes = 128 / 8;
				uint32_t padding = blockSize_bytes - (incSize + 1 + 4) % blockSize_bytes;
				if (reliable)
					padding = blockSize_bytes - (incSize + 1 + 4 + 2) % blockSize_bytes;

				NetMessage stream;
				NetMessage message;
				stream.ByteArray::Write((byte)padding);
				if (reliable)
				{
					_encryptCounter++;
					message.ByteArray::Write((uint16_t)(_encryptCounter - 1));
				}
				message.ByteArray::Write(data, incSize);
				stream.ByteArray::Write(BaseLib::CRC32::Hash(message.GetBuffer(), message.GetLength()));
				stream.ByteArray::Write(message.GetBuffer(), message.GetLength());
				for (uint32_t i = 0; i < padding; i++)
					stream.ByteArray::Write((byte)0);

				EVP_CIPHER_CTX* ctx;
				ctx = EVP_CIPHER_CTX_new();
				if (ctx == NULL)
					throw std::exception(fmt::format("NetCrypt::Encrypt | EVP_CIPHER_CTX_new {0}", ERR_get_error()).c_str());
				if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, Key, NULL) == 0)
					throw std::exception(fmt::format("NetCrypt::Encrypt | EVP_EncryptInit_ex {0}", ERR_get_error()).c_str());
				EVP_CIPHER_CTX_set_padding(ctx, false);
				byte* out = new byte[stream.GetLength()];
				int outSize2 = 0;
				if (EVP_EncryptUpdate(ctx, out, &outSize2, stream.GetBuffer(), stream.GetLength()) == 0)
					throw std::exception(fmt::format("NetCrypt::Encrypt | EVP_EncryptUpdate {0}", ERR_get_error()).c_str());
				outSize += outSize2;
				outSize2 = 0;
				if (EVP_EncryptFinal_ex(ctx, stream.GetBuffer() + outSize, &outSize2) == 0)
					throw std::exception(fmt::format("NetCrypt::Encrypt | EVP_EncryptFinal_ex {0}", ERR_get_error()).c_str());
				outSize += outSize2;
				EVP_CIPHER_CTX_free(ctx);

				result = true;
				return out;
			}

			case EncryptMode::Fast:
			{
				RC4_KEY rc4key;
				RC4_set_key(&rc4key, FastKeyLength, FastKey);
				NetMessage stream;
				if (reliable)
				{
					_encryptCounter++;
					stream.ByteArray::Write((uint16_t)(_encryptCounter - 1));
				}
				stream.ByteArray::Write(data, incSize);
				byte* out = new byte[stream.GetLength()];
				RC4(&rc4key, stream.GetLength(), stream.GetBuffer(), out);

				result = true;
				return out;
			}
			default:
			{
				result = false;
				return nullptr;
			}
			}
		}

		byte* NetCrypt::Decrypt(byte* data, size_t incSize, size_t& outSize, EncryptMode mode, bool& result, bool reliable)
		{
			result = false;

			switch (mode)
			{
			case EncryptMode::Secure:
			{
				EVP_CIPHER_CTX* ctx;
				ctx = EVP_CIPHER_CTX_new();
				if (ctx == NULL)
					throw std::exception(fmt::format("NetCrypt::Decrypt | EVP_CIPHER_CTX_new {0}", ERR_get_error()).c_str());
				if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, Key, NULL) == 0)
					throw std::exception(fmt::format("NetCrypt::Decrypt | EVP_DecryptInit_ex {0}", ERR_get_error()).c_str());
				EVP_CIPHER_CTX_set_padding(ctx, false);
				byte* decrypted = new byte[incSize];
				int decryptedSize = 0;
				size_t decryptedSize2 = 0;
				if (EVP_DecryptUpdate(ctx, decrypted, &decryptedSize, data, incSize) == 0)
					throw std::exception(fmt::format("NetCrypt::Decrypt | EVP_DecryptUpdate {0}", ERR_get_error()).c_str());
				decryptedSize2 += decryptedSize;
				decryptedSize = 0;
				if (EVP_DecryptFinal_ex(ctx, decrypted + decryptedSize2, &decryptedSize) == 0)
					throw std::exception(fmt::format("NetCrypt::Decrypt | EVP_DecryptFinal_ex {0}", ERR_get_error()).c_str());
				decryptedSize2 += decryptedSize;
				EVP_CIPHER_CTX_free(ctx);

				NetMessage message(decrypted, decryptedSize2);
				delete[] decrypted;
				byte padding = 0;
				message.ByteArray::Read(padding);
				uint32_t checksum = 0;
				message.ByteArray::Read(checksum);
				if (reliable)
				{
					_decryptCounter++;
					uint16_t remoteCounter = 0;
					message.ByteArray::GetUInt16(remoteCounter);
					if (((uint16_t)(_decryptCounter - 1)) != remoteCounter)
						throw std::exception(fmt::format("Invalid decrypt counter! Remote: {0} Local: {1}", remoteCounter, _decryptCounter - 1).c_str());
				}
				size_t outSize2 = 0;
				byte* out = message.ByteArray::ReadAll(outSize2);

				outSize = outSize2 - padding;
				byte* out2 = new byte[outSize];
				memcpy(out2, out, outSize);
				delete[] out;

				if (BaseLib::CRC32::Hash(out2, outSize) != checksum)
					throw std::exception("Invalid checksum!");

				if (reliable)
				{
					outSize -= sizeof(uint16_t);
					byte* out3 = new byte[outSize];
					memcpy(out3, out2 + sizeof(uint16_t), outSize);
					delete[] out2;

					result = true;
					return out3;
				}

				result = true;
				return out2;
			}

			case EncryptMode::Fast:
			{
				RC4_KEY rc4key;
				RC4_set_key(&rc4key, FastKeyLength, FastKey);
				byte* decrypted = new byte[incSize];
				RC4(&rc4key, incSize, data, decrypted);

				NetMessage message(decrypted, incSize);
				delete[] decrypted;
				if (reliable)
				{
					_decryptCounter++;
					uint16_t remoteCounter = 0;
					message.ByteArray::Read(remoteCounter);
					if (_decryptCounter - 1 != remoteCounter)
						throw std::exception(fmt::format("Invalid decrypt counter! Remote: {0} Local: {1}", remoteCounter, _decryptCounter - 1).c_str());
				}
				byte* out = message.ByteArray::ReadAll(outSize);

				result = true;
				return out;
			}
			default:
			{
				result = false;
				return nullptr;
			}
			}
		}

		NetCrypt::~NetCrypt()
		{
			if (Key != nullptr)
			{
				delete[] Key;
				Key = nullptr;
			}
			if (FastKey != nullptr)
			{
				delete[] FastKey;
				FastKey = nullptr;
			}
		}

		NetCrypt::NetCrypt(const NetCrypt& obj)
			: NetCrypt(obj.KeyLength * 8, obj.ServerFastKeyLength * 8)
		{
			std::copy(obj.Key, obj.Key + obj.KeyLength, Key);
			std::copy(obj.FastKey, obj.FastKey + obj.FastKeyLength, FastKey);
		}

		NetCrypt::NetCrypt(NetCrypt&& obj) noexcept
			: NetCrypt(obj.KeyLength * 8, obj.ServerFastKeyLength * 8)
		{
			obj.Key = nullptr;
			obj.KeyLength = 0;
			obj.FastKey = nullptr;
			obj.FastKeyLength = 0;
			obj.ServerFastKeyLength = 0;
		}

		NetCrypt& NetCrypt::operator=(const NetCrypt& obj)
		{
			NetCrypt temp(obj);

			temp.swap(*this);

			return *this;
		}

		NetCrypt& NetCrypt::operator=(NetCrypt&& obj) noexcept
		{
			NetCrypt temp(std::move(obj));

			temp.swap(*this);

			return *this;
		}

		void NetCrypt::swap(NetCrypt& obj) noexcept
		{
			std::swap(Key, obj.Key);
			std::swap(KeyLength, obj.KeyLength);
			std::swap(FastKey, obj.FastKey);
			std::swap(FastKeyLength, obj.FastKeyLength);
			std::swap(ServerFastKeyLength, obj.ServerFastKeyLength);
		}
	}
}