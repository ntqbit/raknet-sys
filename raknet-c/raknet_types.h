#pragma once

#if defined(_WIN32)
#include <winsock2.h>
typedef SOCKET __UDPSOCKET__;
typedef SOCKET __TCPSOCKET__;
typedef int socklen_t;
#else
#include <netinet/in.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct _RakPeerInterface RakPeerInterface;
    typedef struct _EasyHandshake EasyHandshake;
    typedef struct _ClientEasyHandshake ClientEasyHandshake;
    typedef struct _ServerEasyHandshake ServerEasyHandshake;
    typedef struct _CookieJar CookieJar;
    typedef struct _AuthenticatedEncryption AuthenticatedEncryption;

    static const int PRIVATE_KEY_BITS = 256;
    static const int PRIVATE_KEY_BYTES = PRIVATE_KEY_BITS / 8;
    static const int PUBLIC_KEY_BYTES = PRIVATE_KEY_BYTES * 2;
    static const int CHALLENGE_BYTES = PRIVATE_KEY_BYTES * 2;
    static const int ANSWER_BYTES = PRIVATE_KEY_BYTES * 4;
    static const int PROOF_BYTES = PRIVATE_KEY_BYTES;
    static const int IDENTITY_BYTES = PRIVATE_KEY_BYTES * 5;

#define MAXIMUM_MTU_SIZE 1492
#define MINIMUM_MTU_SIZE 400

    typedef struct _BitStream BitStream;
    typedef enum
    {
        RAKNET_STARTED,
        RAKNET_ALREADY_STARTED,
        INVALID_SOCKET_DESCRIPTORS,
        INVALID_MAX_CONNECTIONS,
        SOCKET_FAMILY_NOT_SUPPORTED,
        SOCKET_PORT_ALREADY_IN_USE,
        SOCKET_FAILED_TO_BIND,
        SOCKET_FAILED_TEST_SEND,
        PORT_CANNOT_BE_ZERO,
        FAILED_TO_CREATE_NETWORK_THREAD,
        COULD_NOT_GENERATE_GUID,
        STARTUP_OTHER_FAILURE
    } StartupResult;

    typedef enum
    {
        CONNECTION_ATTEMPT_STARTED,
        INVALID_PARAMETER,
        CANNOT_RESOLVE_DOMAIN_NAME,
        ALREADY_CONNECTED_TO_ENDPOINT,
        CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS,
        SECURITY_INITIALIZATION_FAILED
    } ConnectionAttemptResult;

    /// These enumerations are used to describe when packets are delivered.
    typedef enum
    {
        /// The highest possible priority. These message trigger sends immediately, and are generally not buffered or aggregated into a single datagram.
        IMMEDIATE_PRIORITY,

        /// For every 2 IMMEDIATE_PRIORITY messages, 1 HIGH_PRIORITY will be sent.
        /// Messages at this priority and lower are buffered to be sent in groups at 10 millisecond intervals to reduce UDP overhead and better measure congestion control.
        HIGH_PRIORITY,

        /// For every 2 HIGH_PRIORITY messages, 1 MEDIUM_PRIORITY will be sent.
        /// Messages at this priority and lower are buffered to be sent in groups at 10 millisecond intervals to reduce UDP overhead and better measure congestion control.
        MEDIUM_PRIORITY,

        /// For every 2 MEDIUM_PRIORITY messages, 1 LOW_PRIORITY will be sent.
        /// Messages at this priority and lower are buffered to be sent in groups at 10 millisecond intervals to reduce UDP overhead and better measure congestion control.
        LOW_PRIORITY,

        /// \internal
        NUMBER_OF_PRIORITIES
    } PacketPriority;

    /// These enumerations are used to describe how packets are delivered.
    /// \note  Note to self: I write this with 3 bits in the stream.  If I add more remember to change that
    /// \note In ReliabilityLayer::WriteToBitStreamFromInternalPacket I assume there are 5 major types
    /// \note Do not reorder, I check on >= UNRELIABLE_WITH_ACK_RECEIPT
    typedef enum
    {
        /// Same as regular UDP, except that it will also discard duplicate datagrams.  RakNet adds (6 to 17) + 21 bits of overhead, 16 of which is used to detect duplicate packets and 6 to 17 of which is used for message length.
        UNRELIABLE,

        /// Regular UDP with a sequence counter.  Out of order messages will be discarded.
        /// Sequenced and ordered messages sent on the same channel will arrive in the order sent.
        UNRELIABLE_SEQUENCED,

        /// The message is sent reliably, but not necessarily in any order.  Same overhead as UNRELIABLE.
        RELIABLE,

        /// This message is reliable and will arrive in the order you sent it.  Messages will be delayed while waiting for out of order messages.  Same overhead as UNRELIABLE_SEQUENCED.
        /// Sequenced and ordered messages sent on the same channel will arrive in the order sent.
        RELIABLE_ORDERED,

        /// This message is reliable and will arrive in the sequence you sent it.  Out or order messages will be dropped.  Same overhead as UNRELIABLE_SEQUENCED.
        /// Sequenced and ordered messages sent on the same channel will arrive in the order sent.
        RELIABLE_SEQUENCED,

        /// Same as UNRELIABLE, however the user will get either ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS based on the result of sending this message when calling RakPeerInterface::Receive(). Bytes 1-4 will contain the number returned from the Send() function. On disconnect or shutdown, all messages not previously acked should be considered lost.
        UNRELIABLE_WITH_ACK_RECEIPT,

        /// Same as UNRELIABLE_SEQUENCED, however the user will get either ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS based on the result of sending this message when calling RakPeerInterface::Receive(). Bytes 1-4 will contain the number returned from the Send() function. On disconnect or shutdown, all messages not previously acked should be considered lost.
        /// 05/04/10 You can't have sequenced and ack receipts, because you don't know if the other system discarded the message, meaning you don't know if the message was processed
        // UNRELIABLE_SEQUENCED_WITH_ACK_RECEIPT,

        /// Same as RELIABLE. The user will also get ID_SND_RECEIPT_ACKED after the message is delivered when calling RakPeerInterface::Receive(). ID_SND_RECEIPT_ACKED is returned when the message arrives, not necessarily the order when it was sent. Bytes 1-4 will contain the number returned from the Send() function. On disconnect or shutdown, all messages not previously acked should be considered lost. This does not return ID_SND_RECEIPT_LOSS.
        RELIABLE_WITH_ACK_RECEIPT,

        /// Same as RELIABLE_ORDERED_ACK_RECEIPT. The user will also get ID_SND_RECEIPT_ACKED after the message is delivered when calling RakPeerInterface::Receive(). ID_SND_RECEIPT_ACKED is returned when the message arrives, not necessarily the order when it was sent. Bytes 1-4 will contain the number returned from the Send() function. On disconnect or shutdown, all messages not previously acked should be considered lost. This does not return ID_SND_RECEIPT_LOSS.
        RELIABLE_ORDERED_WITH_ACK_RECEIPT,

        /// Same as RELIABLE_SEQUENCED. The user will also get ID_SND_RECEIPT_ACKED after the message is delivered when calling RakPeerInterface::Receive(). Bytes 1-4 will contain the number returned from the Send() function. On disconnect or shutdown, all messages not previously acked should be considered lost.
        /// 05/04/10 You can't have sequenced and ack receipts, because you don't know if the other system discarded the message, meaning you don't know if the message was processed
        // RELIABLE_SEQUENCED_WITH_ACK_RECEIPT,

        /// \internal
        NUMBER_OF_RELIABILITIES
    } PacketReliability;

    /// Returned from RakPeerInterface::GetConnectionState()
    typedef enum
    {
        /// Connect() was called, but the process hasn't started yet
        IS_PENDING,
        /// Processing the connection attempt
        IS_CONNECTING,
        /// Is connected and able to communicate
        IS_CONNECTED,
        /// Was connected, but will disconnect as soon as the remaining messages are delivered
        IS_DISCONNECTING,
        /// A connection attempt failed and will be aborted
        IS_SILENTLY_DISCONNECTING,
        /// No longer connected
        IS_DISCONNECTED,
        /// Was never connected, or else was disconnected long enough ago that the entry has been discarded
        IS_NOT_CONNECTED
    } ConnectionState;

    /// Given a number of bits, return how many bytes are needed to represent that.

    /// \sa NetworkIDObject.h
    typedef unsigned char UniqueIDType;
    typedef unsigned short SystemIndex;
    typedef unsigned char RPCIndex;
    const int MAX_RPC_MAP_SIZE = ((RPCIndex)-1) - 1;
    const int UNDEFINED_RPC_INDEX = ((RPCIndex)-1);

    /// First byte of a network message
    typedef unsigned char MessageID;

    typedef uint32_t BitSize_t;

    /// Used with the PublicKey structure
    typedef enum
    {
        /// The connection is insecure. You can also just pass 0 for the pointer to PublicKey in RakPeerInterface::Connect()
        PKM_INSECURE_CONNECTION,

        /// Accept whatever public key the server gives us. This is vulnerable to man in the middle, but does not require
        /// distribution of the public key in advance of connecting.
        PKM_ACCEPT_ANY_PUBLIC_KEY,

        /// Use a known remote server public key. PublicKey::remoteServerPublicKey must be non-zero.
        /// This is the recommended mode for secure connections.
        PKM_USE_KNOWN_PUBLIC_KEY,

        /// Use a known remote server public key AND provide a public key for the connecting client.
        /// PublicKey::remoteServerPublicKey, myPublicKey and myPrivateKey must be all be non-zero.
        /// The server must cooperate for this mode to work.
        /// I recommend not using this mode except for server-to-server communication as it significantly increases the CPU requirements during connections for both sides.
        /// Furthermore, when it is used, a connection password should be used as well to avoid DoS attacks.
        PKM_USE_TWO_WAY_AUTHENTICATION
    } PublicKeyMode;

    /// Passed to RakPeerInterface::Connect()
    typedef struct
    {
        /// How to interpret the public key, see above
        PublicKeyMode publicKeyMode;

        /// Pointer to a public key of length cat::EasyHandshake::PUBLIC_KEY_BYTES. See the Encryption sample.
        char *remoteServerPublicKey;

        /// (Optional) Pointer to a public key of length cat::EasyHandshake::PUBLIC_KEY_BYTES
        char *myPublicKey;

        /// (Optional) Pointer to a private key of length cat::EasyHandshake::PRIVATE_KEY_BYTES
        char *myPrivateKey;
    } PublicKey;

    /// Describes the local socket to use for RakPeer::Startup
    typedef struct
    {

        /// The local port to bind to.  Pass 0 to have the OS autoassign a port.
        unsigned short port;

        /// The local network card address to bind to, such as "127.0.0.1".  Pass an empty string to use INADDR_ANY.
        char hostAddress[32];

        /// IP version: For IPV4, use AF_INET (default). For IPV6, use AF_INET6. To autoselect, use AF_UNSPEC.
        /// IPV6 is the newer internet protocol. Instead of addresses such as natpunch.jenkinssoftware.com, you may have an address such as fe80::7c:31f7:fec4:27de%14.
        /// Encoding takes 16 bytes instead of 4, so IPV6 is less efficient for bandwidth.
        /// On the positive side, NAT Punchthrough is not needed and should not be used with IPV6 because there are enough addresses that routers do not need to create address mappings.
        /// RakPeer::Startup() will fail if this IP version is not supported.
        /// \pre RAKNET_SUPPORT_IPV6 must be set to 1 in RakNetDefines.h for AF_INET6
        short socketFamily;

        unsigned short remotePortRakNetWasStartedOn_PS3_PSP2;

        // Required for Google chrome
        int chromeInstance;

        // Set to true to use a blocking socket (default, do not change unless you have a reason to)
        bool blockingSocket;

        /// XBOX only: set IPPROTO_VDP if you want to use VDP. If enabled, this socket does not support broadcast to 255.255.255.255
        unsigned int extraSocketOptions;
    } SocketDescriptor;

    /// \brief Network address for a system
    /// \details Corresponds to a network address<BR>
    /// This is not necessarily a unique identifier. For example, if a system has both LAN and internet connections, the system may be identified by either one, depending on who is communicating<BR>
    /// Therefore, you should not transmit the SystemAddress over the network and expect it to identify a system, or use it to connect to that system, except in the case where that system is not behind a NAT (such as with a dedciated server)
    /// Use RakNetGUID for a unique per-instance of RakPeer to identify systems
    typedef struct
    {
        /// SystemAddress, with RAKNET_SUPPORT_IPV6 defined, holds both an sockaddr_in6 and a sockaddr_in
        union // In6OrIn4
        {
#if RAKNET_SUPPORT_IPV6 == 1
            struct sockaddr_storage sa_stor;
            struct sockaddr_in6 addr6;
#endif

            struct sockaddr_in addr4;
        } address;

        /// This is not used internally, but holds a copy of the port held in the address union, so for debugging it's easier to check what port is being held
        unsigned short debugPort;

        /// \internal Used internally for fast lookup. Optional (use -1 to do regular lookup). Don't transmit this.
        SystemIndex systemIndex;
    } SystemAddress;

    /// Uniquely identifies an instance of RakPeer. Use RakPeer::GetGuidFromSystemAddress() and RakPeer::GetSystemAddressFromGuid() to go between SystemAddress and RakNetGUID
    /// Use RakPeer::GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS) to get your own GUID
    typedef struct
    {
        //	uint32_t g[6];
        uint64_t g;

        // Used internally for fast lookup. Optional (use -1 to do regular lookup). Don't transmit this.
        SystemIndex systemIndex;
    } RakNetGUID;

    /// Index of an invalid SystemAddress
    const SystemAddress UNASSIGNED_SYSTEM_ADDRESS = {0xFFFFFFFF, 0xFFFF};
    //{
    //	{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}
    //	0xFFFFFFFFFFFFFFFF
    //};

    typedef struct
    {
        RakNetGUID rakNetGuid;
        SystemAddress systemAddress;
    } AddressOrGUID;

    typedef uint64_t NetworkID;

    /// This represents a user message from another system.
    typedef struct
    {
        /// The system that send this packet.
        SystemAddress systemAddress;

        /// A unique identifier for the system that sent this packet, regardless of IP address (internal / external / remote system)
        /// Only valid once a connection has been established (ID_CONNECTION_REQUEST_ACCEPTED, or ID_NEW_INCOMING_CONNECTION)
        /// Until that time, will be UNASSIGNED_RAKNET_GUID
        RakNetGUID guid;

        /// The length of the data in bytes
        unsigned int length;

        /// The length of the data in bits
        BitSize_t bitSize;

        /// The data from the sender
        unsigned char *data;

        /// @internal
        /// Indicates whether to delete the data, or to simply delete the packet.
        bool deleteData;

        /// @internal
        /// If true, this message is meant for the user, not for the plugins, so do not process it through plugins
        bool wasGeneratedLocally;
    } Packet;

    ///  Index of an unassigned player
    const SystemIndex UNASSIGNED_PLAYER_INDEX = 65535;

    /// Unassigned object ID
    const NetworkID UNASSIGNED_NETWORK_ID = (uint64_t)-1;

    const int PING_TIMES_ARRAY_SIZE = 5;

    typedef uint64_t Time;
    typedef uint32_t TimeMS;
    typedef uint64_t TimeUS;

    typedef enum
    {
        /// How many bytes per pushed via a call to RakPeerInterface::Send()
        USER_MESSAGE_BYTES_PUSHED,

        /// How many user message bytes were sent via a call to RakPeerInterface::Send(). This is less than or equal to USER_MESSAGE_BYTES_PUSHED.
        /// A message would be pushed, but not yet sent, due to congestion control
        USER_MESSAGE_BYTES_SENT,

        /// How many user message bytes were resent. A message is resent if it is marked as reliable, and either the message didn't arrive or the message ack didn't arrive.
        USER_MESSAGE_BYTES_RESENT,

        /// How many user message bytes were received, and returned to the user successfully.
        USER_MESSAGE_BYTES_RECEIVED_PROCESSED,

        /// How many user message bytes were received, but ignored due to data format errors. This will usually be 0.
        USER_MESSAGE_BYTES_RECEIVED_IGNORED,

        /// How many actual bytes were sent, including per-message and per-datagram overhead, and reliable message acks
        ACTUAL_BYTES_SENT,

        /// How many actual bytes were received, including overead and acks.
        ACTUAL_BYTES_RECEIVED,

        /// \internal
        RNS_PER_SECOND_METRICS_COUNT
    } RNSPerSecondMetrics;

    typedef struct
    {
        /// For each type in RNSPerSecondMetrics, what is the value over the last 1 second?
        uint64_t valueOverLastSecond[RNS_PER_SECOND_METRICS_COUNT];

        /// For each type in RNSPerSecondMetrics, what is the total value over the lifetime of the connection?
        uint64_t runningTotal[RNS_PER_SECOND_METRICS_COUNT];

        /// When did the connection start?
        /// \sa RakNet::GetTimeUS()
        TimeUS connectionStartTime;

        /// Is our current send rate throttled by congestion control?
        /// This value should be true if you send more data per second than your bandwidth capacity
        bool isLimitedByCongestionControl;

        /// If \a isLimitedByCongestionControl is true, what is the limit, in bytes per second?
        uint64_t BPSLimitByCongestionControl;

        /// Is our current send rate throttled by a call to RakPeer::SetPerConnectionOutgoingBandwidthLimit()?
        bool isLimitedByOutgoingBandwidthLimit;

        /// If \a isLimitedByOutgoingBandwidthLimit is true, what is the limit, in bytes per second?
        uint64_t BPSLimitByOutgoingBandwidthLimit;

        /// For each priority level, how many messages are waiting to be sent out?
        unsigned int messageInSendBuffer[NUMBER_OF_PRIORITIES];

        /// For each priority level, how many bytes are waiting to be sent out?
        double bytesInSendBuffer[NUMBER_OF_PRIORITIES];

        /// How many messages are waiting in the resend buffer? This includes messages waiting for an ack, so should normally be a small value
        /// If the value is rising over time, you are exceeding the bandwidth capacity. See BPSLimitByCongestionControl
        unsigned int messagesInResendBuffer;

        /// How many bytes are waiting in the resend buffer. See also messagesInResendBuffer
        uint64_t bytesInResendBuffer;

        /// Over the last second, what was our packetloss? This number will range from 0.0 (for none) to 1.0 (for 100%)
        float packetlossLastSecond;

        /// What is the average total packetloss over the lifetime of the connection?
        float packetlossTotal;
    } RakNetStatistics;

    typedef struct _RakNetSocket2 RakNetSocket2;

    typedef struct
    {
        char data[MAXIMUM_MTU_SIZE];

        int bytesRead;
        SystemAddress systemAddress;
        TimeUS timeRead;
        RakNetSocket2 *socket;
    } RNS2RecvStruct;

    typedef struct _PluginInterface2 PluginInterface2;

#ifdef __cplusplus
}
#endif