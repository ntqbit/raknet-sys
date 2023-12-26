#include <RakNet/RakPeerInterface.h>
#include <RakNet/RakNetStatistics.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/RakNetTypes.h>

using namespace RakNet;

#include "rakpeer.h"

RakPeerInterface *RakPeer_new()
{
    return RakNet::RakPeerInterface::GetInstance();
}

void RakPeer_drop(RakPeerInterface *rakpeer)
{
    RakNet::RakPeerInterface::DestroyInstance(rakpeer);
}

Packet *RakPeer_Receive(RakPeerInterface *self)
{
    return self->Receive();
}

void RakPeer_Ping(RakPeerInterface *self, const SystemAddress *target)
{
    self->Ping(*target);
}

int RakPeer_GetAveragePing(RakPeerInterface *self, const AddressOrGUID *systemIdentifier)
{
    return self->GetAveragePing(*systemIdentifier);
}

int RakPeer_GetLastPing(RakPeerInterface *self, const AddressOrGUID *systemIdentifier)
{
    return self->GetLastPing(*systemIdentifier);
}

SystemAddress RakPeer_GetSystemAddressFromIndex(RakPeerInterface *self, unsigned int index)
{
    return self->GetSystemAddressFromIndex(index);
}

void RakPeer_DeallocatePacket(RakPeerInterface *self, Packet *packet)
{
    self->DeallocatePacket(packet);
}

uint32_t RakPeer_Send_data(RakPeerInterface *self, const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID *systemIdentifier, bool broadcast, uint32_t forceReceiptNumber)
{
    return self->Send(data, length, priority, reliability, orderingChannel, *systemIdentifier, broadcast, forceReceiptNumber);
}

void RakPeer_Shutdown(RakPeerInterface *self, unsigned int blockDuration, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority)
{
    self->Shutdown(blockDuration, orderingChannel, disconnectionNotificationPriority);
}

void RakPeer_CloseConnection(RakPeerInterface *self, const AddressOrGUID *target, bool sendDisconnectionNotification, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority)
{
    self->CloseConnection(*target, sendDisconnectionNotification, orderingChannel, disconnectionNotificationPriority);
}

RakNetStatistics *RakPeer_GetStatistics_address(RakPeerInterface *self, const SystemAddress *systemAddress, RakNetStatistics *rns)
{
    return self->GetStatistics(*systemAddress, rns);
}

const RakNetGUID *RakPeer_GetGuidFromSystemAddress(RakPeerInterface *self, const SystemAddress *input)
{
    return &self->GetGuidFromSystemAddress(*input);
}

ConnectionAttemptResult RakPeer_Connect(RakPeerInterface *self, const char *host, unsigned short remotePort, const char *passwordData, int passwordDataLength, PublicKey *publicKey, unsigned connectionSocketIndex, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, TimeMS timeoutTime)
{
    return self->Connect(host, remotePort, passwordData, passwordDataLength, publicKey, connectionSocketIndex, sendConnectionAttemptCount, timeBetweenSendConnectionAttemptsMS, timeoutTime);
}

unsigned RakPeer_GetNumberOfAddresses(RakPeerInterface *self)
{
    return self->GetNumberOfAddresses();
}

void RakPeer_SetOccasionalPing(RakPeerInterface *self, bool doPing)
{
    return self->SetOccasionalPing(doPing);
}

void RakPeer_AllowConnectionResponseIPMigration(RakPeerInterface *self, bool allow)
{
    return self->AllowConnectionResponseIPMigration(allow);
}

StartupResult RakPeer_Startup(RakPeerInterface *self, unsigned int maxConnections, SocketDescriptor *socketDescriptors, unsigned socketDescriptorCount, int threadPriority)
{
    return self->Startup(maxConnections, socketDescriptors, socketDescriptorCount, threadPriority);
}

const char *RakPeer_GetLocalIP(RakPeerInterface *self, unsigned int index)
{
    return self->GetLocalIP(index);
}

void RakPeer_SetUnreliableTimeout(RakPeerInterface *self, TimeMS timeoutMS)
{
    self->SetUnreliableTimeout(timeoutMS);
}

void RakPeer_SetMaximumIncomingConnections(RakPeerInterface *self, unsigned short numberAllowed)
{
    self->SetMaximumIncomingConnections(numberAllowed);
}

void RakPeer_SetTimeoutTime(RakPeerInterface *self, TimeMS timeMS, const SystemAddress *target)
{
    self->SetTimeoutTime(timeMS, *target);
}

SystemAddress RakPeer_GetInternalID(RakPeerInterface *self, const SystemAddress *systemAddress, const int index)
{
    return self->GetInternalID(*systemAddress, index);
}

bool RakPeer_InitializeSecurity(RakPeerInterface *self, const char *publicKey, const char *privateKey, bool bRequireClientKey)
{
    return self->InitializeSecurity(publicKey, privateKey, bRequireClientKey);
}