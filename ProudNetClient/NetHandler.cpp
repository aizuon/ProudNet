#include <fmt/format.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <thread>
#include <chrono>

#include "NetHandler.h"
#include "../BaseLib/GuidUtils.h"
#include "Constants.h"
#include "../BaseLib/ByteArrayUtils.h"
#include "NetZip.h"
#include "NetUtil.h"
#include "SysUtil.h"
#include "../BaseLib/SocketUtils.h"

namespace Nettention
{
	namespace Proud
	{
		void NetHandler::SplitAndProcessStream(NetClient& client, byte* buffer, size_t length, const BaseLib::IPEndPoint& udpEndPoint)
		{
			ByteArray arr(buffer, length);

			int it = 0;
			while (arr.ReadOffset != length)
			{
				arr.ReadOffset += sizeof(uint16_t);
				int64_t locallength = 0;
				sbyte prefix = 0;
				if (arr.GetScalar(locallength, prefix))
				{
					arr.ReadOffset += sizeof(sbyte) + prefix + locallength;
					NetMessage message(buffer + it, sizeof(uint16_t) + sizeof(sbyte) + prefix + locallength);
					if (message.GetLength() != length)
						client.m_Logger.Verbose(fmt::format("Splitted message | {0} => {1}", message.GetLength(), BaseLib::Utils::ByteArrayUtils::ToHexString(message.GetBuffer(), message.GetLength())));
					NetHandler::ReadFrame(client, message, udpEndPoint);
					it = arr.ReadOffset;
				}
				else
					break;
			}
		}

		void NetHandler::ReadFrame(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint)
		{
			uint16_t magic = 0;
			if (!message.ByteArray::Read(magic) || magic != Constants::Magic)
				return;

			bool arrResult = false;
			ByteArray arr = message.ReadArray(arrResult);
			if (!arrResult)
				return;

			NetMessage message2(arr);
			message2.RelayFrom = message.RelayFrom;

			ReadMessage(client, message2, udpEndPoint);
		}

		void NetHandler::ReadMessage(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint)
		{
			message.WriteOffset = message.GetLength();
			MessageType messageType;
			if (!message.Read(messageType))
				return;

			switch (messageType)
			{
			case MessageType::RMI:
			{
				RMIHandler(client, message, udpEndPoint);
				break;
			}

			case MessageType::NotifyServerConnectionHint:
			{
				NotifyServerConnectionHintHandler(client, message);
				break;
			}

			case MessageType::NotifyCSSessionKeySuccess:
			{
				NotifyCSSessionKeySuccessHandler(client);
				break;
			}

			case MessageType::NotifyServerConnectSuccess:
			{
				NotifyServerConnectSuccessHandler(client, message);
				break;
			}

			case MessageType::Encrypted_Reliable:
			{
				EncryptedHandler(client, message);
				break;
			}

			case MessageType::UnreliableRelay2:
			{
				UnreliableRelay2Handler(client, message);
				break;
			}

			case MessageType::ReliableRelay2:
			{
				ReliableRelay2Handler(client, message);
				break;
			}

			case MessageType::Encrypted_UnReliable:
			{
				EncryptedHandler(client, message, false);
				break;
			}

			case MessageType::Compressed:
			{
				CompressedHandler(client, message);
				break;
			}

			case MessageType::RequestStartServerHolepunch:
			{
				RequestStartServerHolepunchHandler(client, message);
				break;
			}

			case MessageType::ServerHolepunchAck:
			{
				ServerHolepunchAckHandler(client, message);
				break;
			}

			case MessageType::NotifyClientServerUdpMatched:
			{
				NotifyClientServerUdpMatchedHandler(client, message);
				break;
			}

			case MessageType::PeerUdp_ServerHolepunchAck:
			{
				PeerUdpServerHolepunchAckHandler(client, message);
				break;
			}

			case MessageType::PeerUdp_PeerHolepunch:
			{
				PeerUdpPeerHolepunchHandler(client, message, udpEndPoint);
				break;
			}

			case MessageType::PeerUdp_PeerHolepunchAck:
			{
				PeerUdpPeerHolepunchAckHandler(client, message, udpEndPoint);
				break;
			}

			case MessageType::P2PRequestIndirectServerTimeAndPing:
			{
				P2PRequestIndirectServerTimeAndPingHandler(client, message, udpEndPoint);
				break;
			}

			case MessageType::P2PReplyIndirectServerTimeAndPong:
			{
				P2PReplyIndirectServerTimeAndPongHandler(client, message, udpEndPoint);
				break;
			}

			case MessageType::ReliableUdp_Frame:
			{
				ReliableUdpFrameHandler(client, message, udpEndPoint);
				break;
			}

			case MessageType::ConnectServerTimedout:
			case MessageType::NotifyProtocolVersionMismatch:
			case MessageType::NotifyServerDeniedConnection:
			case MessageType::S2CRoutedMulticast1:
			case MessageType::S2CRoutedMulticast2:
				break;

			default:
			{
				if (messageType != MessageType::UnreliablePong)
					client.m_Logger.Warning(fmt::format("Recieved unknown Core ID {0}", messageType));
				break;
			}
			}
		}

		void NetHandler::RMIHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint)
		{
			uint16_t rmiID = 0;
			if (!message.ByteArray::Read(rmiID))
				return;

			switch ((ProudOpCode)rmiID)
			{
			case ProudOpCode::P2PGroup_MemberJoin:
			{
				uint32_t grouphostid = 0;
				uint32_t memberid = 0;
				if (!message.ByteArray::Read(grouphostid) || !message.ByteArray::Read(memberid))
					return;

				bool bytearray1Result = false;
				ByteArray bytearray1 = message.ReadArray(bytearray1Result);
				if (!bytearray1Result)
					return;

				uint32_t eventid = 0;
				if (!message.ByteArray::Read(eventid))
					return;

				bool bytearray2Result = false;
				ByteArray bytearray2 = message.ReadArray(bytearray2Result);
				if (!bytearray2Result)
					return;

				bool bytearray3Result = false;
				ByteArray bytearray3 = message.ReadArray(bytearray3Result);
				if (!bytearray3Result)
					return;

				uint32_t num5 = 0;
				GUID b1;
				bool flag1 = false;
				uint16_t num6 = 0;

				if (!message.ByteArray::Read(num5) || !message.Read(b1) ||
					!message.ByteArray::Read(flag1) || !message.ByteArray::Read(num6))
					return;

				client.m_Logger.Information(fmt::format("P2PGroup_MemberJoin => {0}", memberid));

				if (memberid == client.HostId)
				{
					client.m_P2PGroup.HostId = grouphostid;
					client.PeerUdpMagicNumber = b1;

					NetMessage c2srequestcreateudpsocket;
					client.RmiToServer((uint16_t)ProudOpCode::C2S_RequestCreateUdpSocket, c2srequestcreateudpsocket);
				}
				else
				{
					P2PMember* p2pmember = new P2PMember(&client, client.m_P2PGroup.HostId, memberid);
					p2pmember->PeerUdpMagicNumber = b1;
					client.m_P2PGroup.P2PMembers.insert(std::make_pair(memberid, p2pmember));
				}

				NetMessage p2pjoinack;
				p2pjoinack.ByteArray::Write(grouphostid);
				p2pjoinack.ByteArray::Write(memberid);
				p2pjoinack.ByteArray::Write(eventid);
				p2pjoinack.ByteArray::Write(false);

				client.RmiToServer((uint16_t)ProudOpCode::P2PGroup_MemberJoin_Ack, p2pjoinack);

				break;
			}

			case ProudOpCode::P2PGroup_MemberJoin_Unencrypted:
			{
				uint32_t grouphostid = 0;
				uint32_t memberid = 0;
				if (!message.ByteArray::Read(grouphostid) || !message.ByteArray::Read(memberid))
					return;

				bool bytearray1Result = false;
				ByteArray bytearray1 = message.ReadArray(bytearray1Result);
				if (!bytearray1Result)
					return;

				uint32_t eventid = 0;
				if (!message.ByteArray::Read(eventid))
					return;

				uint32_t num5 = 0;
				GUID b1;
				bool flag1 = false;
				uint16_t num6 = 0;

				if (!message.ByteArray::Read(num5) || !message.Read(b1) ||
					!message.ByteArray::Read(flag1) || !message.ByteArray::Read(num6))
					return;

				client.m_Logger.Information(fmt::format("P2PGroup_MemberJoin_Unencrypted => {0}", memberid));

				if (memberid == client.HostId)
				{
					client.m_P2PGroup.HostId = grouphostid;
					client.PeerUdpMagicNumber = b1;

					NetMessage c2srequestcreateudpsocket;
					client.RmiToServer((uint16_t)ProudOpCode::C2S_RequestCreateUdpSocket, c2srequestcreateudpsocket);
				}
				else
				{
					P2PMember* p2pmember = new P2PMember(&client, client.m_P2PGroup.HostId, memberid);
					p2pmember->PeerUdpMagicNumber = b1;
					client.m_P2PGroup.P2PMembers.insert(std::make_pair(memberid, p2pmember));
				}

				NetMessage p2pjoinack;
				p2pjoinack.ByteArray::Write(grouphostid);
				p2pjoinack.ByteArray::Write(memberid);
				p2pjoinack.ByteArray::Write(eventid);
				p2pjoinack.ByteArray::Write(false);

				client.RmiToServer((uint16_t)ProudOpCode::P2PGroup_MemberJoin_Ack, p2pjoinack);

				break;
			}

			case ProudOpCode::P2PGroup_MemberLeave:
			{
				uint32_t grouphostid = 0;
				uint32_t memberid = 0;
				if (!message.ByteArray::Read(grouphostid) || !message.ByteArray::Read(memberid))
					return;

				client.m_Logger.Information(fmt::format("P2PGroup_MemberLeave => {0}", memberid));

				if (grouphostid == client.m_P2PGroup.HostId)
				{
					auto it = client.m_P2PGroup.P2PMembers.find(memberid);
					if (it != client.m_P2PGroup.P2PMembers.end())
					{
						P2PMember* p2pmember = it->second;
						p2pmember->Close();
						delete p2pmember;

						client.m_P2PGroup.P2PMembers.unsafe_erase(memberid);
					}
				}

				break;
			}

			case ProudOpCode::P2P_NotifyDirectP2PDisconnected2:
			{
				uint32_t memberid = 0;
				uint32_t reason = 0;
				if (!message.ByteArray::Read(memberid) || !message.ByteArray::Read(reason))
					return;

				client.m_Logger.Information(fmt::format("P2P_NotifyDirectP2PDisconnected2 => hostid = {0}, reason = {1}", memberid, (ErrorType)reason));

				break;
			}

			case ProudOpCode::S2C_RequestCreateUdpSocket:
			{
				BaseLib::IPEndPoint udpsocket;
				if (!message.ReadStringEndPoint(udpsocket))
					return;

				client.m_Logger.Information(fmt::format("S2C_RequestCreateUdpSocket => {0}", udpsocket.ToString()));

				std::thread t = std::thread([&client, udpsocket]()
					{
						if (client.UdpChannel != INVALID_SOCKET)
							client.CloseUdp();

						client.ConnectUdp();

						client.ServerUdpSocket = udpsocket;

						NetMessage c2screateudpsocketack;
						c2screateudpsocketack.ByteArray::Write(true);

						client.RmiToServer((uint16_t)ProudOpCode::C2S_CreateUdpSocketAck, c2screateudpsocketack);
					});
				t.detach();

				break;
			}

			case ProudOpCode::P2PRecycleComplete:
			{
				uint32_t hostid = 0;
				if (!message.ByteArray::Read(hostid))
					return;

				client.m_Logger.Information(fmt::format("P2PRecycleComplete => {0}", hostid));

				NetMessage notifyjitdirectp2ptriggered;
				notifyjitdirectp2ptriggered.ByteArray::Write(hostid);

				client.RmiToServer((uint16_t)ProudOpCode::NotifyJitDirectP2PTriggered, notifyjitdirectp2ptriggered);

				break;
			}

			case ProudOpCode::NewDirectP2PConnection:
			{
				uint32_t hostid = 0;
				if (!message.ByteArray::Read(hostid))
					return;

				client.m_Logger.Information(fmt::format("NewDirectP2PConnection => {0}", hostid));

				NetMessage peerudpserverholepunch;
				peerudpserverholepunch.WriteEnum(MessageType::PeerUdp_ServerHolepunch);
				peerudpserverholepunch.Write(client.UdpMagicNumber);
				peerudpserverholepunch.ByteArray::Write(hostid);

				client.ProudToServerUdpIfAvaliable(peerudpserverholepunch, true);

				break;
			}

			case ProudOpCode::RequestP2PHolepunch:
			{
				uint32_t hostid = 0;
				if (!message.ByteArray::Read(hostid))
					return;

				BaseLib::IPEndPoint localendpoint;
				BaseLib::IPEndPoint endpoint;
				if (!message.ReadEndPoint(localendpoint) || !message.ReadEndPoint(endpoint))
					return;

				client.m_Logger.Information(fmt::format("RequestP2PHolepunch => {0}, {1}, {2}", hostid, localendpoint.ToString(), endpoint.ToString()));

				auto it = client.m_P2PGroup.P2PMembers.find(hostid);
				if (it != client.m_P2PGroup.P2PMembers.end())
				{
					P2PMember* p2pmember = it->second;
					p2pmember->PeerUdpSocket = endpoint;
					p2pmember->PeerUdpLocalSocket = localendpoint;

					std::thread t([&client, p2pmember]()
						{
							for (int i = 0; i < 10; i++)
							{
								std::this_thread::sleep_for(std::chrono::milliseconds(500));

								if (p2pmember->PeerUdpChannel != INVALID_SOCKET)
								{
									NetMessage peerudppeerholepunch;
									peerudppeerholepunch.WriteEnum(MessageType::PeerUdp_PeerHolepunch);
									peerudppeerholepunch.ByteArray::Write(client.HostId);
									peerudppeerholepunch.Write(p2pmember->PeerUdpMagicNumber);
									peerudppeerholepunch.Write(client.ServerGuid);
									peerudppeerholepunch.Write(p2pmember->PeerUdpSocket);

									p2pmember->ProudToPeer(peerudppeerholepunch, p2pmember->PeerUdpSocket);

									if (NetUtil::IsSameHost(p2pmember->SelfUdpSocket, p2pmember->PeerUdpSocket) &&
										NetUtil::IsSameLan(p2pmember->PeerUdpLocalSocket, p2pmember->SelfUdpLocalSocket))
									{
										NetMessage peerudppeerholepunch2;
										peerudppeerholepunch2.WriteEnum(MessageType::PeerUdp_PeerHolepunch);
										peerudppeerholepunch2.ByteArray::Write(client.HostId);
										peerudppeerholepunch2.Write(p2pmember->PeerUdpMagicNumber);
										peerudppeerholepunch2.Write(client.ServerGuid);
										peerudppeerholepunch2.Write(p2pmember->PeerUdpLocalSocket);

										p2pmember->ProudToPeer(peerudppeerholepunch, p2pmember->PeerUdpLocalSocket);
									}

									break;
								}
							}
						});
					t.detach();
				}

				break;
			}

			case ProudOpCode::NotifyDirectP2PEstablish:
			{
				uint32_t hostid4 = 0;
				uint32_t hostid5 = 0;
				if (!message.ByteArray::Read(hostid4) || !message.ByteArray::Read(hostid5))
					return;

				BaseLib::IPEndPoint endpoint2;
				BaseLib::IPEndPoint endpoint3;
				BaseLib::IPEndPoint endpoint4;
				BaseLib::IPEndPoint endpoint5;

				if (!message.ReadEndPoint(endpoint2) || !message.ReadEndPoint(endpoint3) ||
					!message.ReadEndPoint(endpoint4) || !message.ReadEndPoint(endpoint5))
					return;

				client.m_Logger.Information(fmt::format("NotifyDirectP2PEstablish => {0}, {1}, {2}, {3}, {4}, {5}",
					hostid4, hostid5, endpoint2.ToString(), endpoint3.ToString(), endpoint4.ToString(), endpoint5.ToString()));

				if (client.HostId == hostid5)
				{
					SysUtil::Swap(hostid4, hostid5);
					SysUtil::Swap(endpoint2, endpoint4);
					SysUtil::Swap(endpoint5, endpoint3);
				}
				auto it = client.m_P2PGroup.P2PMembers.find(hostid5);
				if (it != client.m_P2PGroup.P2PMembers.end())
				{
					P2PMember* p2pmember4 = it->second;
					p2pmember4->DirectP2P = true;

					p2pmember4->PeerLocalToRemoteSocket = endpoint2;
					p2pmember4->PeerRemoteToLocalSocket = endpoint5;
					p2pmember4->SelfLocalToRemoteSocket = endpoint4;
					p2pmember4->SelfRemoteToLocalSocket = endpoint3;
				}

				break;
			}

			case ProudOpCode::NotifyUdpToTcpFallbackByServer:
			{
				client.CloseUdp();
				for (auto& x : client.m_P2PGroup.P2PMembers)
				{
					x.second->Close();
				}

				client.UdpEnabled = false;

				NetMessage notifyudptotcpfallbackbyclient;
				client.RmiToServer((uint16_t)ProudOpCode::NotifyUdpToTcpFallbackByClient, notifyudptotcpfallbackbyclient);

				break;
			}

			case ProudOpCode::RenewP2PConnectionState:
			{
				uint32_t hostid6 = 0;
				if (!message.ByteArray::Read(hostid6))
					return;

				client.m_Logger.Information(fmt::format("RenewP2PConnectionState => {0}", hostid6));

				auto it = client.m_P2PGroup.P2PMembers.find(hostid6);
				if (it != client.m_P2PGroup.P2PMembers.end())
				{
					P2PMember* p2pmember3 = it->second;
					p2pmember3->DirectP2P = false;

					p2pmember3->Close();

					NetMessage notifyjitdirectp2ptriggered2;
					notifyjitdirectp2ptriggered2.ByteArray::Write(hostid6);

					client.RmiToServer((uint16_t)ProudOpCode::NotifyJitDirectP2PTriggered, notifyjitdirectp2ptriggered2);
				}

				break;
			}

			case ProudOpCode::ShutdownTcpAck:
			{
				client.m_Logger.Information("ShutdownTcpAck");

				NetMessage message;
				client.RmiToServer((uint16_t)ProudOpCode::ShutdownTcpHandshake, message);

				break;
			}

			case ProudOpCode::ReportServerTimeAndFrameRateAndPing:
			{
				double time = 0.0;
				float framerate = 0.0;
				if (!message.ByteArray::Read(time) || !message.ByteArray::Read(framerate))
					return;

				P2PMember* p2pmember6 = nullptr;
				if (!udpEndPoint.IsEmpty())
				{
					auto it = std::find_if(client.m_P2PGroup.P2PMembers.begin(), client.m_P2PGroup.P2PMembers.end(),
						[&udpEndPoint](const std::pair<uint32_t, P2PMember*>& t) -> bool
						{
							return t.second->PeerRemoteToLocalSocket == udpEndPoint;
						});
					if (it != client.m_P2PGroup.P2PMembers.end())
						p2pmember6 = it->second;
				}
				else
				{
					auto it = client.m_P2PGroup.P2PMembers.find(message.RelayFrom);
					if (it != client.m_P2PGroup.P2PMembers.end())
						p2pmember6 = it->second;
				}

				if (p2pmember6 != nullptr)
				{
					client.m_Logger.Verbose(fmt::format("ReportServerTimeAndFrameRateAndPing => hostid = {0}", p2pmember6->HostId));

					NetMessage selfping;
					selfping.ByteArray::Write(time);
					selfping.ByteArray::Write(framerate);
					selfping.Reliable = true;

					//p2pmember6.RmiToPeer((ushort)ProudOpCode.ReportServerTimeAndFrameRateAndPing, selfping, true);
				}

				break;
			}

			case ProudOpCode::ReportServerTimeAndFrameRateAndPong:
			{
				double clienttime = 0.0;
				double servertime = 0.0;
				double serverping = 0.0;
				float framerate2 = 0.0;
				if (!message.ByteArray::Read(clienttime) || !message.ByteArray::Read(servertime) || !message.ByteArray::Read(serverping) || message.ByteArray::Read(framerate2))
					return;

				P2PMember* p2pmember7 = nullptr;
				if (!udpEndPoint.IsEmpty())
				{
					auto it = std::find_if(client.m_P2PGroup.P2PMembers.begin(), client.m_P2PGroup.P2PMembers.end(),
						[&udpEndPoint](const std::pair<uint32_t, P2PMember*>& t) -> bool
						{
							return t.second->PeerRemoteToLocalSocket == udpEndPoint;
						});
					if (it != client.m_P2PGroup.P2PMembers.end())
						p2pmember7 = it->second;
				}
				else
				{
					auto it = client.m_P2PGroup.P2PMembers.find(message.RelayFrom);
					if (it != client.m_P2PGroup.P2PMembers.end())
						p2pmember7 = it->second;
				}

				if (p2pmember7 != nullptr)
				{
					client.m_Logger.Verbose(fmt::format("ReportServerTimeAndFrameRateAndPong => hostid = {0}", p2pmember7->HostId));

					NetMessage selfpong;
					selfpong.ByteArray::Write(clienttime);
					selfpong.ByteArray::Write(servertime);
					selfpong.ByteArray::Write(serverping);
					selfpong.ByteArray::Write(framerate2);
					selfpong.Reliable = true;

					//p2pmember7.RmiToPeer((ushort)ProudOpCode.ReportServerTimeAndFrameRateAndPong, selfpong, true);
				}

				break;
			}

			default:
			{
				client.OnRMIRecieve(message, rmiID);
				break;
			}
			}
		}

		void NetHandler::NotifyServerConnectionHintHandler(NetClient& client, NetMessage& message)
		{
			client.m_Logger.Information("NotifyServerConnectionHint");

			NetSettings settings;
			if (message.ByteArray::Read(settings.EnableServerLog) && message.ByteArray::Read(settings.FallbackMethod) &&
				message.ByteArray::Read(settings.MessageMaxLength) && message.ByteArray::Read(settings.IdleTimeout) &&
				message.ByteArray::Read(settings.DirectP2PStartCondition) &&
				message.ByteArray::Read(settings.OverSendSuspectingThresholdInBytes) &&
				message.ByteArray::Read(settings.EnableNagleAlgorithm) &&
				message.ByteArray::Read(settings.EncryptedMessageKeyLength) &&
				message.ByteArray::Read(settings.FastEncryptedMessageKeyLength) &&
				message.ByteArray::Read(settings.AllowServerAsP2PGroupMember) &&
				message.ByteArray::Read(settings.EnableP2PEncryptedMessaging) &&
				message.ByteArray::Read(settings.UpnpDetectNatDevice) && message.ByteArray::Read(settings.UpnpTcpAddrPortMapping) &&
				message.ByteArray::Read(settings.EnableLookaheadP2PSend) && message.ByteArray::Read(settings.EnablePingTest) &&
				message.ByteArray::Read(settings.EmergencyLogLineCount))
			{
				client.m_NetSettings = settings;
				client.m_NetCrypt = NetCrypt(settings.EncryptedMessageKeyLength, settings.FastEncryptedMessageKeyLength);

				bool arrResult = false;
				ByteArray arr = message.ReadArray(arrResult);
				if (!arrResult)
					return;

				byte* buff = arr.GetBuffer();
				client.m_RSA = d2i_RSAPublicKey(NULL, (const unsigned char**)&buff, arr.GetLength());

				NetMessage data;
				data.WriteEnum(MessageType::NotifyCSEncryptedSessionKey);

				byte* out = new byte[RSA_size(client.m_RSA)];
				int outSize = RSA_public_encrypt(client.m_NetCrypt.GetKeyLength(), client.m_NetCrypt.GetKey(), out, client.m_RSA, RSA_PKCS1_OAEP_PADDING);
				if (outSize == -1)
					client.m_Logger.Error(fmt::format("NotifyServerConnectionHintHandler | RSA_public_encrypt {0}", ERR_get_error()).c_str());
				data.ByteArray::Write(ByteArray(out, outSize));
				delete[] out;

				EVP_CIPHER_CTX* ctx;
				ctx = EVP_CIPHER_CTX_new();
				if (ctx == NULL)
					client.m_Logger.Error(fmt::format("NotifyServerConnectionHintHandler | EVP_CIPHER_CTX_new {0}", ERR_get_error()).c_str());
				if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, client.m_NetCrypt.GetKey(), NULL) == 0)
					client.m_Logger.Error(fmt::format("NotifyServerConnectionHintHandler | EVP_EncryptInit_ex {0}", ERR_get_error()).c_str());
				EVP_CIPHER_CTX_set_padding(ctx, false);
				byte* out2 = new byte[client.m_NetCrypt.GetFastKeyLength()];
				int out2Size = 0;
				size_t out2Size2 = 0;
				if (EVP_EncryptUpdate(ctx, out2, &out2Size, client.m_NetCrypt.GetFastKey(), client.m_NetCrypt.GetFastKeyLength()) == 0)
					client.m_Logger.Error(fmt::format("NotifyServerConnectionHintHandler | EVP_EncryptUpdate {0}", ERR_get_error()).c_str());
				out2Size2 += out2Size;
				out2Size = 0;
				if (EVP_EncryptFinal_ex(ctx, client.m_NetCrypt.GetFastKey() + out2Size2, &out2Size) == 0)
					client.m_Logger.Error(fmt::format("NotifyServerConnectionHintHandler | EVP_EncryptFinal_ex {0}", ERR_get_error()).c_str());
				out2Size2 += out2Size;
				EVP_CIPHER_CTX_free(ctx);
				data.ByteArray::Write(ByteArray(out2, out2Size2));
				delete[] out2;

				client.ProudToServer(data);
			}
		}

		void NetHandler::NotifyCSSessionKeySuccessHandler(NetClient& client)
		{
			client.m_Logger.Information("NotifyCSSessionKeySuccess");

			NetMessage data;
			data.WriteEnum(MessageType::NotifyServerConnectionRequestData);
			data.ByteArray::Write(ByteArray());

			//EU
			//switch (client.m_ServerType)
			//{
			//case ServerType::Auth:
			//{
			//	data.Write(BaseLib::Utils::GuidUtils::FromString("{3e23d770-bbb0-4eed-3363-356d7a767d16}"));
			//	break;
			//}

			//case ServerType::Game:
			//{
			//	data.Write(BaseLib::Utils::GuidUtils::FromString("{8fc9193e-a966-9910-290a-482c72237677}"));
			//	break;
			//}

			//case ServerType::Chat:
			//{
			//	data.Write(BaseLib::Utils::GuidUtils::FromString("{62a138c5-b7e2-b8ce-5422-426b68040855}"));
			//	break;
			//}

			//case ServerType::Relay:
			//{
			//	data.Write(BaseLib::Utils::GuidUtils::FromString("{4fe2aded-5c3c-702d-255c-27510b68734d}"));
			//	break;
			//}
			//}

			//S10
			switch (client.m_ServerType)
			{
			case ServerType::Auth:
			{
				data.Write(BaseLib::Utils::GuidUtils::FromString("{9be73c0b-3b10-403e-be7d-9f222702a38c}"));
				break;
			}

			case ServerType::Game:
			{
				data.Write(BaseLib::Utils::GuidUtils::FromString("{beb92241-8333-4117-ab92-9b4af78c688f}"));
				break;
			}

			case ServerType::Chat:
			{
				data.Write(BaseLib::Utils::GuidUtils::FromString("{97d36acf-8cc0-4dfb-bcc9-97cab255e2bc}"));
				break;
			}

			case ServerType::Relay:
			{
				data.Write(BaseLib::Utils::GuidUtils::FromString("{a43a97d1-9ec7-495e-ad5f-8fe45fde1151}"));
				break;
			}
			}

			data.ByteArray::Write(Constants::NetVersion);

			client.ProudToServer(data);
		}

		void NetHandler::NotifyServerConnectSuccessHandler(NetClient& client, NetMessage& message)
		{
			client.m_Logger.Information("NotifyServerConnectSuccess");

			uint32_t hostId = 0;
			GUID serverGuid;
			if (!message.ByteArray::Read(hostId) || !message.Read(serverGuid))
				return;

			bool arrResult = false;
			ByteArray byteArray = message.ReadArray(arrResult);
			if (!arrResult)
				return;

			BaseLib::IPEndPoint ip;
			if (!message.ReadEndPoint(ip))
				return;

			client.HostId = hostId;
			client.ServerGuid = serverGuid;

			client.OnConnectionComplete();
		}

		void NetHandler::UnreliableRelay2Handler(NetClient& client, NetMessage& message)
		{
			uint32_t hostId = 0;
			if (!message.ByteArray::Read(hostId)) //hostid
				return;

			bool packetResult = false;
			ByteArray packet = message.ReadArray(packetResult);
			if (!packetResult)
				return;

			NetMessage message2(packet);
			message2.RelayFrom = hostId;

			ReadFrame(client, message2);
		}

		void NetHandler::ReliableRelay2Handler(NetClient& client, NetMessage& message)
		{
			uint32_t hostId = 0;
			if (!message.ByteArray::Read(hostId)) //hostid
				return;

			uint32_t num1 = 0;
			if (!message.ByteArray::Read(num1))
				return;

			bool packetResult = false;
			ByteArray packet = message.ReadArray(packetResult);
			if (!packetResult)
				return;

			NetMessage message2(packet);
			message2.RelayFrom = hostId;

			ReadFrame(client, message2);
		}

		void NetHandler::EncryptedHandler(NetClient& client, NetMessage& message, bool reliable)
		{
			byte mode = 0;
			if (!message.ByteArray::Read(mode))
				return;

			bool arrResult = false;
			ByteArray byteArray = message.ReadArray(arrResult);
			if (!arrResult)
				return;

			size_t decryptedSize = 0;
			bool decryptResult = false;
			byte* decrypted = nullptr;
			try
			{
				decrypted = client.m_NetCrypt.Decrypt(byteArray.GetBuffer(), byteArray.GetLength(), decryptedSize, (EncryptMode)mode, decryptResult, reliable);
			}
			catch (std::exception ex)
			{
				client.m_Logger.Error(ex.what());
				return;
			}

			NetMessage message2(decrypted, decryptedSize);
			delete[] decrypted;
			ReadMessage(client, message2);
		}

		void NetHandler::CompressedHandler(NetClient& client, NetMessage& message)
		{
			int64_t num = 0;
			int64_t length = 0;
			if (message.ReadScalar(num) && message.ReadScalar(length))
			{
				bool arrResult = false;
				byte* data = message.ReadArray(num, arrResult);
				if (!arrResult)
				{
					client.m_Logger.Error("Corrupted compressed packet!");
				}
				else
				{
					NetMessage message2(data, num);
					NetMessage message1 = NetZip::DecompressPacket(message2);
					message1.Compress = true;
					message1.Encrypt = message.Encrypt;
					message1.m_EncryptMode = message.m_EncryptMode;
					ReadMessage(client, message1);
				}
			}
			else
			{
				client.m_Logger.Error("Corrupted compressed packet!");
			}
		}

		void NetHandler::RequestStartServerHolepunchHandler(NetClient& client, NetMessage& message)
		{
			GUID magicnumber;
			if (!message.Read(magicnumber))
				return;

			client.UdpMagicNumber = magicnumber;

			client.m_Logger.Information(fmt::format("RequestStartServerHolepunch => guid = {0}", BaseLib::Utils::GuidUtils::ToString(magicnumber)));

			NetMessage serverholepunch;
			serverholepunch.WriteEnum(MessageType::ServerHolepunch);
			serverholepunch.Write(magicnumber);

			client.ProudToServerUdpIfAvaliable(serverholepunch, true);
		}

		void NetHandler::ServerHolepunchAckHandler(NetClient& client, NetMessage& message)
		{
			GUID magicnumber;
			BaseLib::IPEndPoint selfudpsocket;

			if (!message.Read(magicnumber) || !message.ReadEndPoint(selfudpsocket))
				return;

			if (magicnumber != client.UdpMagicNumber)
				return;

			client.m_Logger.Information(fmt::format("ServerHolepunchAck => guid = {0}, endpoint = {1}", BaseLib::Utils::GuidUtils::ToString(magicnumber), selfudpsocket.ToString()));

			client.SelfUdpSocket = selfudpsocket;

			NetMessage notifyholepunchsuccess;
			notifyholepunchsuccess.WriteEnum(MessageType::NotifyHolepunchSuccess);
			notifyholepunchsuccess.Write(client.UdpMagicNumber);
			notifyholepunchsuccess.Write(BaseLib::Utils::SocketUtils::GetLocalIP(client.UdpChannel));
			notifyholepunchsuccess.Write(client.SelfUdpSocket);

			client.ProudToServer(notifyholepunchsuccess);

			if (!client.PingLoop.IsRunning())
			{
				if (client.PingLoop.IsEmpty())
				{
					client.PingLoop = BaseLib::ThreadLoop<NetClient>(&NetClient::SendUdpPing, &client, std::chrono::seconds(10));
				}

				client.PingLoop.Start();
			}
		}

		void NetHandler::NotifyClientServerUdpMatchedHandler(NetClient& client, NetMessage& message)
		{
			GUID magicnumber;
			if (!message.Read(magicnumber))
				return;

			client.UdpMagicNumber = magicnumber;

			client.UdpEnabled = true;

			client.m_Logger.Information(fmt::format("NotifyClientServerUdpMatched => guid = {0}", BaseLib::Utils::GuidUtils::ToString(magicnumber)));
		}

		void NetHandler::PeerUdpServerHolepunchAckHandler(NetClient& client, NetMessage& message)
		{
			GUID magicnumber;
			uint32_t hostid = 0;
			BaseLib::IPEndPoint selfudpsocket;

			if (!message.Read(magicnumber) || !message.ReadEndPoint(selfudpsocket) || !message.ByteArray::Read(hostid))
				return;

			if (magicnumber != client.UdpMagicNumber)
				return;

			client.m_Logger.Information(fmt::format("PeerUdpServerHolepunchAck => guid = {0}, endpoint = {1}, hostid = {2}", BaseLib::Utils::GuidUtils::ToString(magicnumber), selfudpsocket.ToString(), hostid));

			auto it = client.m_P2PGroup.P2PMembers.find(hostid);
			if (it != client.m_P2PGroup.P2PMembers.end())
			{
				P2PMember* p2pmember = it->second;
				std::thread t([&client, p2pmember, hostid]()
				{
						std::lock_guard<std::mutex> lock(p2pmember->mtx);

						if (p2pmember->PeerUdpChannel != INVALID_SOCKET)
							p2pmember->Close();

						p2pmember->ConnectUdp();

						NetMessage peerudpnotifyholepunchsuccess;
						peerudpnotifyholepunchsuccess.WriteEnum(MessageType::PeerUdp_NotifyHolepunchSuccess);
						p2pmember->SelfUdpLocalSocket = BaseLib::IPEndPoint(BaseLib::Utils::SocketUtils::GetLocalIP(client.Channel).IP, BaseLib::Utils::SocketUtils::GetLocalIP(p2pmember->PeerUdpChannel).Port);
						peerudpnotifyholepunchsuccess.Write(p2pmember->SelfUdpLocalSocket);
						p2pmember->SelfUdpSocket = BaseLib::IPEndPoint(client.SelfUdpSocket.IP, BaseLib::Utils::SocketUtils::GetRemoteIP(p2pmember->PeerUdpChannel).Port);
						peerudpnotifyholepunchsuccess.Write(p2pmember->SelfUdpSocket);
						peerudpnotifyholepunchsuccess.ByteArray::Write(hostid);

						client.ProudToServerUdpIfAvaliable(peerudpnotifyholepunchsuccess, true);
				});
				t.detach();
			}
		}

		void NetHandler::PeerUdpPeerHolepunchHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint)
		{
			uint32_t hostid = 0;
			GUID magicnumber;
			GUID magicnumber2;
			BaseLib::IPEndPoint selfudpsocket;

			if (!message.ByteArray::Read(hostid) || !message.Read(magicnumber) || !message.Read(magicnumber2) || !message.ReadEndPoint(selfudpsocket))
				return;

			if (magicnumber2 != client.ServerGuid)
				return;

			client.m_Logger.Information(fmt::format("PeerUdpPeerHolepunch => hostid = {0}, guid = {1}, guid2 = {2}, endpoint = {3}",
				hostid, BaseLib::Utils::GuidUtils::ToString(magicnumber), BaseLib::Utils::GuidUtils::ToString(magicnumber2), selfudpsocket.ToString()));

			auto it = client.m_P2PGroup.P2PMembers.find(hostid);
			if (it != client.m_P2PGroup.P2PMembers.end())
			{
				P2PMember* p2pmember = it->second;
				if (magicnumber != client.PeerUdpMagicNumber)
					return;

				std::thread t([&client, p2pmember, magicnumber, selfudpsocket, udpEndPoint]()
					{
						for (int i = 0; i < 10; i++)
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(500));

							if (!p2pmember->PeerUdpSocket.IsEmpty() && !p2pmember->PeerUdpLocalSocket.IsEmpty() &&
								!p2pmember->SelfUdpSocket.IsEmpty() && !p2pmember->SelfUdpLocalSocket.IsEmpty())
							{
								if (NetUtil::IsUnicastEndpoint(p2pmember->PeerUdpLocalSocket) &&
									NetUtil::IsUnicastEndpoint(p2pmember->SelfUdpLocalSocket) &&
									NetUtil::IsSameLan(p2pmember->SelfUdpSocket, p2pmember->PeerUdpSocket) &&
									NetUtil::IsSameLan(p2pmember->PeerUdpLocalSocket, p2pmember->SelfUdpLocalSocket))
								{
									NetMessage peerudppeerholepunchack;
									peerudppeerholepunchack.WriteEnum(MessageType::PeerUdp_PeerHolepunchAck);
									peerudppeerholepunchack.Write(magicnumber);
									peerudppeerholepunchack.ByteArray::Write(client.HostId);
									peerudppeerholepunchack.Write(selfudpsocket);
									peerudppeerholepunchack.Write(udpEndPoint);
									peerudppeerholepunchack.Write(p2pmember->PeerUdpLocalSocket);

									p2pmember->ProudToPeer(peerudppeerholepunchack, p2pmember->PeerUdpLocalSocket);
								}

								if (NetUtil::IsUnicastEndpoint(p2pmember->PeerUdpSocket))
								{
									NetMessage peerudppeerholepunchack;
									peerudppeerholepunchack.WriteEnum(MessageType::PeerUdp_PeerHolepunchAck);
									peerudppeerholepunchack.Write(magicnumber);
									peerudppeerholepunchack.ByteArray::Write(client.HostId);
									peerudppeerholepunchack.Write(selfudpsocket);
									peerudppeerholepunchack.Write(udpEndPoint);
									peerudppeerholepunchack.Write(p2pmember->PeerUdpSocket);

									p2pmember->ProudToPeer(peerudppeerholepunchack, p2pmember->PeerUdpSocket);
								}

								NetMessage peerudppeerholepunchack;
								peerudppeerholepunchack.WriteEnum(MessageType::PeerUdp_PeerHolepunchAck);
								peerudppeerholepunchack.Write(magicnumber);
								peerudppeerholepunchack.ByteArray::Write(client.HostId);
								peerudppeerholepunchack.Write(selfudpsocket);
								peerudppeerholepunchack.Write(udpEndPoint);
								peerudppeerholepunchack.Write(udpEndPoint);

								p2pmember->ProudToPeer(peerudppeerholepunchack, udpEndPoint);

								break;
							}
						}
					});
				t.detach();
			}
		}

		void NetHandler::PeerUdpPeerHolepunchAckHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint)
		{
			GUID magicnumber;
			uint32_t hostid = 0;
			BaseLib::IPEndPoint endpoint;
			BaseLib::IPEndPoint endpoint2;
			BaseLib::IPEndPoint endpoint3;

			if (!message.Read(magicnumber) || !message.ByteArray::Read(hostid) || !message.ReadEndPoint(endpoint) ||
				!message.ReadEndPoint(endpoint2) || !message.ReadEndPoint(endpoint3))
				return;

			client.m_Logger.Information(fmt::format("PeerUdpPeerHolepunchAck => guid = {0}, hostid = {1}, endpoint = {2}, endpoint2 = {3}, endpoint3 = {4}",
				BaseLib::Utils::GuidUtils::ToString(magicnumber), hostid, endpoint.ToString(), endpoint2.ToString(), endpoint3.ToString()));

			auto it = client.m_P2PGroup.P2PMembers.find(hostid);
			if (it != client.m_P2PGroup.P2PMembers.end())
			{
				P2PMember* p2pmember = it->second;
				if (magicnumber != p2pmember->PeerUdpMagicNumber)
					return;

				NetMessage notifyp2pholepunchsuccess;
				notifyp2pholepunchsuccess.ByteArray::Write(client.HostId);
				notifyp2pholepunchsuccess.ByteArray::Write(hostid);
				notifyp2pholepunchsuccess.Write(endpoint);
				notifyp2pholepunchsuccess.Write(endpoint2);
				notifyp2pholepunchsuccess.Write(endpoint3);
				notifyp2pholepunchsuccess.Write(udpEndPoint);

				client.RmiToServer((uint16_t)ProudOpCode::NotifyP2PHolepunchSuccess, notifyp2pholepunchsuccess);
			}
		}

		void NetHandler::P2PRequestIndirectServerTimeAndPingHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint)
		{
			double time = 0.0;
			if (!message.ByteArray::Read(time))
				return;

			P2PMember* p2pmember = nullptr;
			if (!udpEndPoint.IsEmpty())
			{
				auto it = std::find_if(client.m_P2PGroup.P2PMembers.begin(), client.m_P2PGroup.P2PMembers.end(),
					[&udpEndPoint](const std::pair<uint32_t, P2PMember*>& t) -> bool
					{
						return t.second->PeerRemoteToLocalSocket == udpEndPoint;
					});
				if (it != client.m_P2PGroup.P2PMembers.end())
					p2pmember = it->second;
			}

			if (p2pmember != nullptr)
			{
				client.m_Logger.Verbose(fmt::format("P2PRequestIndirectServerTimeAndPing => hostid = {0}", p2pmember->HostId));

				NetMessage p2preplyindirectservertimeandpong;
				p2preplyindirectservertimeandpong.WriteEnum(MessageType::P2PReplyIndirectServerTimeAndPong);
				p2preplyindirectservertimeandpong.ByteArray::Write(time);

				p2pmember->ProudToPeer(p2preplyindirectservertimeandpong);

				NetMessage p2preplyindirectservertimeandping;
				p2preplyindirectservertimeandping.WriteEnum(MessageType::P2PRequestIndirectServerTimeAndPing);
				p2preplyindirectservertimeandping.ByteArray::Write(time);

				p2pmember->ProudToPeer(p2preplyindirectservertimeandping);
			}
			else
				client.m_Logger.Warning(fmt::format("Recieved P2PRequestIndirectServerTimeAndPing from unknown ip => {0}", udpEndPoint.ToString()));
		}

		void NetHandler::P2PReplyIndirectServerTimeAndPongHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint)
		{
			double time = 0.0;
			if (!message.ByteArray::Read(time))
				return;

			P2PMember* p2pmember = nullptr;
			if (!udpEndPoint.IsEmpty())
			{
				auto it = std::find_if(client.m_P2PGroup.P2PMembers.begin(), client.m_P2PGroup.P2PMembers.end(),
					[&udpEndPoint](const std::pair<uint32_t, P2PMember*>& t) -> bool
					{
						return t.second->PeerRemoteToLocalSocket == udpEndPoint;
					});
				if (it != client.m_P2PGroup.P2PMembers.end())
					p2pmember = it->second;
			}

			if (p2pmember != nullptr)
				client.m_Logger.Verbose(fmt::format("P2PReplyIndirectServerTimeAndPong => hostid = {0}", p2pmember->HostId));
			else
				client.m_Logger.Warning(fmt::format("Recieved P2PReplyIndirectServerTimeAndPong from unknown ip => {0}", udpEndPoint.ToString()));
		}

		void NetHandler::ReliableUdpFrameHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint)
		{
			byte type = 0;
			if (!message.ByteArray::Read(type))
				return;

			switch (type)
			{
			case 1: //data
			{
				uint32_t framenumber = 0;
				if (!message.ByteArray::Read(framenumber))
					return;

				bool arrResult = false;
				ByteArray data = message.ReadArray(arrResult);
				if (!arrResult)
					return;

				NetMessage frame = NetMessage(data);
				frame.RelayFrom = message.RelayFrom;

				uint16_t magic = 0;
				if (!frame.ByteArray::Read(magic))
					return;

				bool arr2Result = false;
				ByteArray data2 = message.ReadArray(arr2Result);
				if (!arr2Result)
					return;

				NetMessage message2 = NetMessage(data2);
				message2.RelayFrom = frame.RelayFrom;

				ReadMessage(client, message2, udpEndPoint);

				break;
			}

			case 2: //ack
			{
				break;
			}
			}
		}
	}
}