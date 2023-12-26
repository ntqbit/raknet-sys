#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    RakPeerInterface *RakPeer_new();

    void RakPeer_drop(RakPeerInterface *rakpeer);

    // --------------------------------------------------------------------------------------------Major Low Level Functions - Functions needed by most users--------------------------------------------------------------------------------------------
    /// \brief Starts the network threads and opens the listen port.
    /// \details You must call this before calling Connect().
    /// \pre On the PS3, call Startup() after Client_Login()
    /// \note Multiple calls while already active are ignored.  To call this function again with different settings, you must first call Shutdown().
    /// \note Call SetMaximumIncomingConnections if you want to accept incoming connections.
    /// \param[in] maxConnections Maximum number of connections between this instance of RakPeer and another instance of RakPeer. Required so that the network can preallocate and for thread safety. A pure client would set this to 1.  A pure server would set it to the number of allowed clients.A hybrid would set it to the sum of both types of connections.
    /// \param[in] localPort The port to listen for connections on. On linux the system may be set up so thast ports under 1024 are restricted for everything but the root user. Use a higher port for maximum compatibility.
    /// \param[in] socketDescriptors An array of SocketDescriptor structures to force RakNet to listen on a particular IP address or port (or both).  Each SocketDescriptor will represent one unique socket.  Do not pass redundant structures.  To listen on a specific port, you can pass SocketDescriptor(myPort,0); such as for a server.  For a client, it is usually OK to just pass SocketDescriptor(); However, on the XBOX be sure to use IPPROTO_VDP
    /// \param[in] socketDescriptorCount The size of the \a socketDescriptors array.  Pass 1 if you are not sure what to pass.
    /// \param[in] threadPriority Passed to the thread creation routine. Use THREAD_PRIORITY_NORMAL for Windows. For Linux based systems, you MUST pass something reasonable based on the thread priorities for your application.
    /// \return RAKNET_STARTED on success, otherwise appropriate failure enumeration.
    StartupResult RakPeer_Startup(RakPeerInterface *self, unsigned int maxConnections, SocketDescriptor *socketDescriptors, unsigned socketDescriptorCount, int threadPriority);

    /// If you accept connections, you must call this or else security will not be enabled for incoming connections.
    /// This feature requires more round trips, bandwidth, and CPU time for the connection handshake
    /// x64 builds require under 25% of the CPU time of other builds
    /// See the Encryption sample for example usage
    /// \pre Must be called while offline
    /// \pre LIBCAT_SECURITY must be defined to 1 in NativeFeatureIncludes.h for this function to have any effect
    /// \param[in] publicKey A pointer to the public key for accepting new connections
    /// \param[in] privateKey A pointer to the private key for accepting new connections
    /// \param[in] bRequireClientKey: Should be set to false for most servers.  Allows the server to accept a public key from connecting clients as a proof of identity but eats twice as much CPU time as a normal connection
    bool RakPeer_InitializeSecurity(RakPeerInterface *self, const char *publicKey, const char *privateKey, bool bRequireClientKey);

    /// Disables security for incoming connections.
    /// \note Must be called while offline
    void RakPeer_DisableSecurity(RakPeerInterface *self);

    /// \brief This is useful if you have a fixed-address internal server behind a LAN.
    ///
    ///  Secure connections are determined by the recipient of an incoming connection. This has no effect if called on the system attempting to connect.
    /// \note If secure connections are on, do not use secure connections for a specific IP address.
    /// \param[in] ip IP address to add. * wildcards are supported.
    void RakPeer_AddToSecurityExceptionList(RakPeerInterface *self, const char *ip);

    /// \brief Remove a specific connection previously added via AddToSecurityExceptionList.
    /// \param[in] ip IP address to remove. Pass 0 to remove all IP addresses. * wildcards are supported.
    void RakPeer_RemoveFromSecurityExceptionList(RakPeerInterface *self, const char *ip);

    /// \brief Checks to see if a given IP is in the security exception list.
    /// \param[in] IP address to check.
    /// \return True if the IP address is found in security exception list, else returns false.
    bool RakPeer_IsInSecurityExceptionList(RakPeerInterface *self, const char *ip);

    /// \brief Sets the maximum number of incoming connections allowed.
    /// \details If the number of incoming connections is less than the number of players currently connected,
    /// no more players will be allowed to connect.  If this is greater than the maximum number of peers allowed,
    /// it will be reduced to the maximum number of peers allowed.
    ///
    /// Defaults to 0, meaning by default, nobody can connect to you
    /// \param[in] numberAllowed Maximum number of incoming connections allowed.
    void RakPeer_SetMaximumIncomingConnections(RakPeerInterface *self, unsigned short numberAllowed);

    /// \brief Returns the value passed to SetMaximumIncomingConnections().
    /// \return Maximum number of incoming connections, which is always <= maxConnections
    unsigned int RakPeer_GetMaximumIncomingConnections(RakPeerInterface *self);

    /// \brief Returns how many open connections exist at this time.
    /// \return Number of open connections.
    unsigned short RakPeer_NumberOfConnections(RakPeerInterface *self);

    /// \brief Sets the password for the incoming connections.
    /// \details  The password must match in the call to Connect (defaults to none).
    /// Pass 0 to passwordData to specify no password.
    /// This is a way to set a low level password for all incoming connections.  To selectively reject connections, implement your own scheme using CloseConnection() to remove unwanted connections.
    /// \param[in] passwordData A data block that incoming connections must match.  This can be just a password, or can be a stream of data. Specify 0 for no password data
    /// \param[in] passwordDataLength The length in bytes of passwordData
    void RakPeer_SetIncomingPassword(RakPeerInterface *self, const char *passwordData, int passwordDataLength);

    /// \brief Gets the password passed to SetIncomingPassword
    /// \param[out] passwordData  Should point to a block large enough to hold the password data you passed to SetIncomingPassword()
    /// \param[in,out] passwordDataLength Maximum size of the passwordData array.  Modified to hold the number of bytes actually written.
    void RakPeer_GetIncomingPassword(RakPeerInterface *self, char *passwordData, int *passwordDataLength);

    /// \brief Connect to the specified host (ip or domain name) and server port.
    /// \details Calling Connect and not calling SetMaximumIncomingConnections acts as a dedicated client.
    /// Calling both acts as a true peer.
    ///
    /// This is a non-blocking connection.
    ///
    /// The connection is successful when GetConnectionState() returns IS_CONNECTED or Receive() gets a message with the type identifier ID_CONNECTION_REQUEST_ACCEPTED.
    /// If the connection is not successful, such as a rejected connection or no response then neither of these things will happen.
    /// \pre Requires that you first call Startup().
    /// \param[in] host Either a dotted IP address or a domain name.
    /// \param[in] remotePort Port to connect to on the remote machine.
    /// \param[in] passwordData A data block that must match the data block on the server passed to SetIncomingPassword().  This can be a string or can be a stream of data.  Use 0 for no password.
    /// \param[in] passwordDataLength The length in bytes of passwordData.
    /// \param[in] publicKey The public key the server is using. If 0, the server is not using security. If non-zero, the publicKeyMode member determines how to connect
    /// \param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in RakPeer::Startup() to determine the one to send on.
    /// \param[in] sendConnectionAttemptCount Number of datagrams to send to the other system to try to connect.
    /// \param[in] timeBetweenSendConnectionAttemptsMS Time to elapse before a datagram is sent to the other system to try to connect. After sendConnectionAttemptCount number of attempts, ID_CONNECTION_ATTEMPT_FAILED is returned. Under low bandwidth conditions with multiple simultaneous outgoing connections, this value should be raised to 1000 or higher, or else the MTU detection can overrun the available bandwidth.
    /// \param[in] timeoutTime Time to elapse before dropping the connection if a reliable message could not be sent. 0 to use the default value from SetTimeoutTime(UNASSIGNED_SYSTEM_ADDRESS);
    /// \return CONNECTION_ATTEMPT_STARTED on successful initiation. Otherwise, an appropriate enumeration indicating failure.
    /// \note CONNECTION_ATTEMPT_STARTED does not mean you are already connected!
    /// \note It is possible to immediately get back ID_CONNECTION_ATTEMPT_FAILED if you exceed the maxConnections parameter passed to Startup(). This could happen if you call CloseConnection() with sendDisconnectionNotificaiton true, then immediately call Connect() before the connection has closed.
    ConnectionAttemptResult RakPeer_Connect(RakPeerInterface *self, const char *host, unsigned short remotePort, const char *passwordData, int passwordDataLength, PublicKey *publicKey, unsigned connectionSocketIndex, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, TimeMS timeoutTime);

    /// \brief Connect to the specified host (ip or domain name) and server port.
    /// \param[in] host Either a dotted IP address or a domain name.
    /// \param[in] remotePort Which port to connect to on the remote machine.
    /// \param[in] passwordData A data block that must match the data block on the server passed to SetIncomingPassword().  This can be a string or can be a stream of data.  Use 0 for no password.
    /// \param[in] passwordDataLength The length in bytes of passwordData.
    /// \param[in] socket A bound socket returned by another instance of RakPeerInterface.
    /// \param[in] sendConnectionAttemptCount Number of datagrams to send to the other system to try to connect.
    /// \param[in] timeBetweenSendConnectionAttemptsMS Time to elapse before a datagram is sent to the other system to try to connect. After sendConnectionAttemptCount number of attempts, ID_CONNECTION_ATTEMPT_FAILED is returned.. Under low bandwidth conditions with multiple simultaneous outgoing connections, this value should be raised to 1000 or higher, or else the MTU detection can overrun the available bandwidth.
    /// \param[in] timeoutTime Time to elapse before dropping the connection if a reliable message could not be sent. 0 to use the default from SetTimeoutTime(UNASSIGNED_SYSTEM_ADDRESS);
    /// \return CONNECTION_ATTEMPT_STARTED on successful initiation. Otherwise, an appropriate enumeration indicating failure.
    /// \note CONNECTION_ATTEMPT_STARTED does not mean you are already connected!
    ConnectionAttemptResult RakPeer_ConnectWithSocket(RakPeerInterface *self, const char *host, unsigned short remotePort, const char *passwordData, int passwordDataLength, RakNetSocket2 *socket, PublicKey *publicKey, unsigned sendConnectionAttemptCount, unsigned timeBetweenSendConnectionAttemptsMS, TimeMS timeoutTime);

    /* /// \brief Connect to the specified network ID (Platform specific console function)
    /// \details Does built-in NAT traversal
    /// \param[in] networkServiceId Network ID structure for the online service
    /// \param[in] passwordData A data block that must match the data block on the server passed to SetIncomingPassword().  This can be a string or can be a stream of data.  Use 0 for no password.
    /// \param[in] passwordDataLength The length in bytes of passwordData.
    //bool Console2LobbyConnect( void *networkServiceId, const char *passwordData, int passwordDataLength );*/

    /// \brief Stops the network threads and closes all connections.
    /// \param[in] blockDuration Wait time(milli seconds) for all remaining messages to go out, including ID_DISCONNECTION_NOTIFICATION.  If 0, it doesn't wait at all.
    /// \param[in] orderingChannel Channel on which ID_DISCONNECTION_NOTIFICATION will be sent, if blockDuration > 0.
    /// \param[in] disconnectionNotificationPriority Priority of sending ID_DISCONNECTION_NOTIFICATION.
    /// If set to 0, the disconnection notification won't be sent.
    void RakPeer_Shutdown(RakPeerInterface *self, unsigned int blockDuration, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority);

    /// \brief Returns true if the network thread is running.
    /// \return True if the network thread is running, False otherwise
    bool RakPeer_IsActive(RakPeerInterface *self);

    /// \brief Fills the array remoteSystems with the SystemAddress of all the systems we are connected to.
    /// \param[out] remoteSystems An array of SystemAddress structures, to be filled with the SystemAddresss of the systems we are connected to. Pass 0 to remoteSystems to get the number of systems we are connected to.
    /// \param[in, out] numberOfSystems As input, the size of remoteSystems array.  As output, the number of elements put into the array.
    bool RakPeer_GetConnectionList(RakPeerInterface *self, SystemAddress *remoteSystems, unsigned short *numberOfSystems);

    /// Returns the next uint32_t that Send() will return
    /// \note If using RakPeer from multiple threads, this may not be accurate for your thread. Use IncrementNextSendReceipt() in that case.
    /// \return The next uint32_t that Send() or SendList will return
    uint32_t RakPeer_GetNextSendReceipt(RakPeerInterface *self);

    /// Returns the next uint32_t that Send() will return, and increments the value by one
    /// \note If using RakPeer from multiple threads, pass this to forceReceipt in the send function
    /// \return The next uint32_t that Send() or SendList will return
    uint32_t RakPeer_IncrementNextSendReceipt(RakPeerInterface *self);

    /// \brief Sends a block of data to the specified system that you are connected to.
    /// \note This function only works while connected.
    /// \note The first byte should be a message identifier starting at ID_USER_PACKET_ENUM.
    /// \param[in] data Block of data to send.
    /// \param[in] length Size in bytes of the data to send.
    /// \param[in] priority Priority level to send on.  See PacketPriority.h
    /// \param[in] reliability How reliably to send this data.  See PacketPriority.h
    /// \param[in] orderingChannel When using ordered or sequenced messages, the channel to order these on. Messages are only ordered relative to other messages on the same stream.
    /// \param[in] systemIdentifier Who to send this packet to, or in the case of broadcasting who not to send it to. Pass either a SystemAddress structure or a RakNetGUID structure. Use UNASSIGNED_SYSTEM_ADDRESS or to specify none
    /// \param[in] broadcast True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to.
    /// \param[in] forceReceipt If 0, will automatically determine the receipt number to return. If non-zero, will return what you give it.
    /// \return 0 on bad input. Otherwise a number that identifies this message. If \a reliability is a type that returns a receipt, on a later call to Receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS with bytes 1-4 inclusive containing this number
    uint32_t RakPeer_Send_data(RakPeerInterface *self, const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID *systemIdentifier, bool broadcast, uint32_t forceReceiptNumber);

    /// \brief "Send" to yourself rather than a remote system.
    /// \details The message will be processed through the plugins and returned to the game as usual.
    /// This function works anytime
    /// \note The first byte should be a message identifier starting at ID_USER_PACKET_ENUM
    /// \param[in] data Block of data to send.
    /// \param[in] length Size in bytes of the data to send.
    void RakPeer_SendLoopback(RakPeerInterface *self, const char *data, const int length);

    /// \brief Sends a block of data to the specified system that you are connected to.
    ///
    /// Same as the above version, but takes a BitStream as input.
    /// \param[in] bitStream Bitstream to send
    /// \param[in] priority Priority level to send on.  See PacketPriority.h
    /// \param[in] reliability How reliably to send this data.  See PacketPriority.h
    /// \param[in] orderingChannel Channel to order the messages on, when using ordered or sequenced messages. Messages are only ordered relative to other messages on the same stream.
    /// \param[in] systemIdentifier System Address or RakNetGUID to send this packet to, or in the case of broadcasting, the address not to send it to.  Use UNASSIGNED_SYSTEM_ADDRESS to specify none.
    /// \param[in] broadcast True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to.
    /// \param[in] forceReceipt If 0, will automatically determine the receipt number to return. If non-zero, will return what you give it.
    /// \return 0 on bad input. Otherwise a number that identifies this message. If \a reliability is a type that returns a receipt, on a later call to Receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS with bytes 1-4 inclusive containing this number
    /// \note COMMON MISTAKE: When writing the first byte, bitStream->Write((unsigned char) ID_MY_TYPE) be sure it is casted to a byte, and you are not writing a 4 byte enumeration.
    uint32_t RakPeer_Send(RakPeerInterface *self, const BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID *systemIdentifier, bool broadcast, uint32_t forceReceiptNumber);

    /// \brief Sends multiple blocks of data, concatenating them automatically.
    ///
    /// This is equivalent to:
    /// BitStream bs;
    /// bs.WriteAlignedBytes(block1, blockLength1);
    /// bs.WriteAlignedBytes(block2, blockLength2);
    /// bs.WriteAlignedBytes(block3, blockLength3);
    /// Send(*bs, ...)
    ///
    /// This function only works when connected.
    /// \param[in] data An array of pointers to blocks of data
    /// \param[in] lengths An array of integers indicating the length of each block of data
    /// \param[in] numParameters Length of the arrays data and lengths
    /// \param[in] priority Priority level to send on.  See PacketPriority.h
    /// \param[in] reliability How reliably to send this data.  See PacketPriority.h
    /// \param[in] orderingChannel Channel to order the messages on, when using ordered or sequenced messages. Messages are only ordered relative to other messages on the same stream.
    /// \param[in] systemIdentifier System Address or RakNetGUID to send this packet to, or in the case of broadcasting, the address not to send it to.  Use UNASSIGNED_SYSTEM_ADDRESS to specify none.
    /// \param[in] broadcast True to send this packet to all connected systems. If true, then systemAddress specifies who not to send the packet to.
    /// \param[in] forceReceipt If 0, will automatically determine the receipt number to return. If non-zero, will return what you give it.
    /// \return 0 on bad input. Otherwise a number that identifies this message. If \a reliability is a type that returns a receipt, on a later call to Receive() you will get ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS with bytes 1-4 inclusive containing this number
    uint32_t RakPeer_SendList(RakPeerInterface *self, const char **data, const int *lengths, const int numParameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID *systemIdentifier, bool broadcast, uint32_t forceReceiptNumber);

    /// \brief Gets a message from the incoming message queue.
    /// \details Use DeallocatePacket() to deallocate the message after you are done with it.
    /// User-thread functions, such as RPC calls and the plugin function PluginInterface::Update occur here.
    /// \return 0 if no packets are waiting to be handled, otherwise a pointer to a packet.
    /// \note COMMON MISTAKE: Be sure to call this in a loop, once per game tick, until it returns 0. If you only process one packet per game tick they will buffer up.
    /// \sa RakNetTypes.h contains struct Packet.
    Packet *RakPeer_Receive(RakPeerInterface *self);

    /// \brief Call this to deallocate a message returned by Receive() when you are done handling it.
    /// \param[in] packet Message to deallocate.
    void RakPeer_DeallocatePacket(RakPeerInterface *self, Packet *packet);

    /// \brief Return the total number of connections we are allowed.
    /// \return Total number of connections allowed.
    unsigned int RakPeer_GetMaximumNumberOfPeers(RakPeerInterface *self);

    // -------------------------------------------------------------------------------------------- Connection Management Functions--------------------------------------------------------------------------------------------
    /// \brief Close the connection to another host (if we initiated the connection it will disconnect, if they did it will kick them out).
    /// \details This method closes the connection irrespective of who initiated the connection.
    /// \param[in] target Which system to close the connection to.
    /// \param[in] sendDisconnectionNotification True to send ID_DISCONNECTION_NOTIFICATION to the recipient.  False to close it silently.
    /// \param[in] channel Which ordering channel to send the disconnection notification on, if any
    /// \param[in] disconnectionNotificationPriority Priority to send ID_DISCONNECTION_NOTIFICATION on.
    void RakPeer_CloseConnection(RakPeerInterface *self, const AddressOrGUID *target, bool sendDisconnectionNotification, unsigned char orderingChannel, PacketPriority disconnectionNotificationPriority);

    /// \brief Cancel a pending connection attempt.
    /// \details If we are already connected, the connection stays open
    /// \param[in] target Target system to cancel.
    void RakPeer_CancelConnectionAttempt(RakPeerInterface *self, const SystemAddress *target);
    /// Returns if a system is connected, disconnected, connecting in progress, or various other states
    /// \param[in] systemIdentifier The system we are referring to
    /// \note This locks a mutex, do not call too frequently during connection attempts or the attempt will take longer and possibly even timeout
    /// \return What state the remote system is in
    ConnectionState RakPeer_GetConnectionState(RakPeerInterface *self, const AddressOrGUID *systemIdentifier);

    /// \brief Given \a systemAddress, returns its index into remoteSystemList.
    /// \details Values range from 0 to the maximum number of players allowed - 1.
    /// This includes systems which were formerly connected, but are now not connected.
    /// \param[in] systemAddress The SystemAddress we are referring to
    /// \return The index of this SystemAddress or -1 on system not found.
    int RakPeer_GetIndexFromSystemAddress(RakPeerInterface *self, const SystemAddress *systemAddress);

    /// \brief Given \a index into remoteSystemList, will return a SystemAddress.
    /// This function is only useful for looping through all systems.
    ///
    /// \param[in] index Index should range between 0 and the maximum number of players allowed - 1.
    /// \return The SystemAddress structure corresponding to \a index in remoteSystemList.
    SystemAddress RakPeer_GetSystemAddressFromIndex(RakPeerInterface *self, unsigned int index);

    /// \brief Same as GetSystemAddressFromIndex but returns RakNetGUID
    /// \param[in] index Index should range between 0 and the maximum number of players allowed - 1.
    /// \return The RakNetGUID
    RakNetGUID RakPeer_GetGUIDFromIndex(RakPeerInterface *self, unsigned int index);

    /// \brief Same as calling GetSystemAddressFromIndex and GetGUIDFromIndex for all systems, but more efficient
    /// Indices match each other, so \a addresses[0] and \a guids[0] refer to the same system
    /// \param[out] addresses All system addresses. Size of the list is the number of connections. Size of the \a addresses list will match the size of the \a guids list.
    /// \param[out] guids All guids. Size of the list is the number of connections. Size of the list will match the size of the \a addresses list.
    // void RakPeer_GetSystemList(RakPeerInterface* self, DataStructures::List<SystemAddress> *addresses, DataStructures::List<RakNetGUID> *guids);

    /// \brief Bans an IP from connecting.
    /// \details Banned IPs persist between connections but are not saved on shutdown nor loaded on startup.
    /// \param[in] IP Dotted IP address. You can use * for a wildcard address, such as 128.0.0. * will ban all IP addresses starting with 128.0.0.
    /// \param[in] milliseconds Gives time in milli seconds for a temporary ban of the IP address.  Use 0 for a permanent ban.
    void RakPeer_AddToBanList(RakPeerInterface *self, const char *IP, TimeMS milliseconds);

    /// \brief Allows a previously banned IP to connect.
    /// param[in] Dotted IP address. You can use * as a wildcard. An IP such as 128.0.0.* will ban all IP addresses starting with 128.0.0.
    void RakPeer_RemoveFromBanList(RakPeerInterface *self, const char *IP);

    /// \brief Allows all previously banned IPs to connect.
    void RakPeer_ClearBanList(RakPeerInterface *self);

    /// \brief Returns true or false indicating if a particular IP is banned.
    /// \param[in] IP Dotted IP address.
    /// \return True if IP matches any IPs in the ban list, accounting for any wildcards. False otherwise.
    bool RakPeer_IsBanned(RakPeerInterface *self, const char *IP);

    /// \brief Enable or disable allowing frequent connections from the same IP adderss
    /// \details This is a security measure which is disabled by default, but can be set to true to prevent attackers from using up all connection slots.
    /// \param[in] b True to limit connections from the same ip to at most 1 per 100 milliseconds.
    void RakPeer_SetLimitIPConnectionFrequency(RakPeerInterface *self, bool b);

    // --------------------------------------------------------------------------------------------Pinging Functions - Functions dealing with the automatic ping mechanism--------------------------------------------------------------------------------------------
    /// Send a ping to the specified connected system.
    /// \pre The sender and recipient must already be started via a successful call to Startup()
    /// \param[in] target Which system to ping
    void RakPeer_Ping(RakPeerInterface *self, const SystemAddress *target);

    /// \brief Send a ping to the specified unconnected system.
    /// \details The remote system, if it is Initialized, will respond with ID_PONG followed by sizeof(TimeMS) containing the system time the ping was sent. Default is 4 bytes - See __GET_TIME_64BIT in RakNetTypes.h
    /// System should reply with ID_PONG if it is active
    /// \param[in] host Either a dotted IP address or a domain name.  Can be 255.255.255.255 for LAN broadcast.
    /// \param[in] remotePort Which port to connect to on the remote machine.
    /// \param[in] onlyReplyOnAcceptingConnections Only request a reply if the remote system is accepting connections
    /// \param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in RakPeer::Startup() to send on.
    /// \return true on success, false on failure (unknown hostname)
    bool RakPeer_Ping_unconnected(RakPeerInterface *self, const char *host, unsigned short remotePort, bool onlyReplyOnAcceptingConnections, unsigned connectionSocketIndex);

    /// \brief Returns the average of all ping times read for the specific system or -1 if none read yet
    /// \param[in] systemAddress Which system we are referring to
    /// \return The ping time for this system, or -1
    int RakPeer_GetAveragePing(RakPeerInterface *self, const AddressOrGUID *systemIdentifier);

    /// \brief Returns the last ping time read for the specific system or -1 if none read yet.
    /// \param[in] systemAddress Which system we are referring to
    /// \return The last ping time for this system, or -1.
    int RakPeer_GetLastPing(RakPeerInterface *self, const AddressOrGUID *systemIdentifier);

    /// \brief Returns the lowest ping time read or -1 if none read yet.
    /// \param[in] systemIdentifier Which system we are referring to
    /// \return The lowest ping time for this system, or -1.
    int RakPeer_GetLowestPing(RakPeerInterface *self, const AddressOrGUID *systemIdentifier);

    /// Ping the remote systems every so often, or not. Can be called anytime.
    /// By default this is true. Recommended to leave on, because congestion control uses it to determine how often to resend lost packets.
    /// It would be true by default to prevent timestamp drift, since in the event of a clock spike, the timestamp deltas would no longer be accurate
    /// \param[in] doPing True to start occasional pings.  False to stop them.
    void RakPeer_SetOccasionalPing(RakPeerInterface *self, bool doPing);

    /// Return the clock difference between your system and the specified system
    /// Subtract GetClockDifferential() from a time returned by the remote system to get that time relative to your own system
    /// Returns 0 if the system is unknown
    /// \param[in] systemIdentifier Which system we are referring to
    Time RakPeer_GetClockDifferential(RakPeerInterface *self, const AddressOrGUID *systemIdentifier);

    // --------------------------------------------------------------------------------------------Static Data Functions - Functions dealing with API defined synchronized memory--------------------------------------------------------------------------------------------
    /// \brief Sets the data to send along with a LAN server discovery or offline ping reply.
    /// \param[in] data Block of data to send, or 0 for none
    /// \param[in] length Length of the data in bytes, or 0 for none
    /// \note \a length should be under 400 bytes, as a security measure against flood attacks
    /// \sa Ping.cpp
    void RakPeer_SetOfflinePingResponse(RakPeerInterface *self, const char *data, const unsigned int length);

    /// \brief Returns pointers to a copy of the \a data passed to SetOfflinePingResponse.
    /// \param[out] data A pointer to a copy of the data passed to SetOfflinePingResponse()
    /// \param[out] length A pointer filled in with the length parameter passed to SetOfflinePingResponse()
    /// \sa SetOfflinePingResponse
    void RakPeer_GetOfflinePingResponse(RakPeerInterface *self, char **data, unsigned int *length);

    //--------------------------------------------------------------------------------------------Network Functions - Functions dealing with the network in general--------------------------------------------------------------------------------------------
    /// \brief Returns the unique address identifier that represents you or another system on the the network
    /// \note Not supported by the XBOX
    /// \param[in] systemAddress Use UNASSIGNED_SYSTEM_ADDRESS to get your behind-LAN address. Use a connected system to get their behind-LAN address. This does not return the port.
    /// \param[in] index When you have multiple internal IDs, which index to return? Currently limited to MAXIMUM_NUMBER_OF_INTERNAL_IDS (so the maximum value of this variable is MAXIMUM_NUMBER_OF_INTERNAL_IDS-1)
    /// \return Identifier of your system internally, which may not be how other systems see if you if you are behind a NAT or proxy.
    SystemAddress RakPeer_GetInternalID(RakPeerInterface *self, const SystemAddress *systemAddress, const int index);

    /// \brief Sets your internal IP address, for platforms that do not support reading it, or to override a value
    /// \param[in] systemAddress. The address to set. Use SystemAddress::FromString() if you want to use a dotted string
    /// \param[in] index When you have multiple internal IDs, which index to set?
    void RakPeer_SetInternalID(RakPeerInterface *self, const SystemAddress *systemAddress, int index);

    /// \brief Returns the unique address identifier that represents the target on the the network and is based on the target's external IP / port.
    /// \param[in] target The SystemAddress of the remote system. Usually the same for all systems, unless you have two or more network cards.
    SystemAddress RakPeer_GetExternalID(RakPeerInterface *self, const SystemAddress *target);

    /// Return my own GUID
    const RakNetGUID RakPeer_GetMyGUID(RakPeerInterface *self);

    /// Return the address bound to a socket at the specified index
    SystemAddress RakPeer_GetMyBoundAddress(RakPeerInterface *self, const int socketIndex);

    /// \brief  Given a connected system address, this method gives the unique GUID representing that instance of RakPeer.
    /// This will be the same on all systems connected to that instance of RakPeer, even if the external system addresses are different.
    /// Complexity is O(log2(n)).
    /// If \a input is UNASSIGNED_SYSTEM_ADDRESS, will return your own GUID
    /// \pre Call Startup() first, or the function will return UNASSIGNED_RAKNET_GUID
    /// \param[in] input The system address of the target system we are connected to.
    const RakNetGUID *RakPeer_GetGuidFromSystemAddress(RakPeerInterface *self, const SystemAddress *input);

    /// \brief Gives the system address of a connected system, given its GUID.
    /// The GUID will be the same on all systems connected to that instance of RakPeer, even if the external system addresses are different.
    /// Currently O(log(n)), but this may be improved in the future
    /// If \a input is UNASSIGNED_RAKNET_GUID, UNASSIGNED_SYSTEM_ADDRESS is returned.
    /// \param[in] input The RakNetGUID of the target system.
    SystemAddress RakPeer_GetSystemAddressFromGuid(RakPeerInterface *self, const RakNetGUID *input);

    /// Given the SystemAddress of a connected system, get the public key they provided as an identity
    /// Returns false if system address was not found or client public key is not known
    /// \param[in] input The RakNetGUID of the system
    /// \param[in] client_public_key The connected client's public key is copied to this address.  Buffer must be cat::EasyHandshake::PUBLIC_KEY_BYTES bytes in length.
    bool RakPeer_GetClientPublicKeyFromSystemAddress(RakPeerInterface *self, const SystemAddress *input, char *client_public_key);

    /// \brief Set the time, in MS, to use before considering ourselves disconnected after not being able to deliver a reliable message.

    /// Set the time, in MS, to use before considering ourselves disconnected after not being able to deliver a reliable message.
    /// Default time is 10,000 or 10 seconds in release and 30,000 or 30 seconds in debug.
    /// Do not set different values for different computers that are connected to each other, or you won't be able to reconnect after ID_CONNECTION_LOST
    /// \param[in] timeMS Time, in MS
    /// \param[in] target SystemAddress structure of the target system. Pass UNASSIGNED_SYSTEM_ADDRESS for all systems.
    void RakPeer_SetTimeoutTime(RakPeerInterface *self, TimeMS timeMS, const SystemAddress *target);

    /// \brief Returns the Timeout time for the given system.
    /// \param[in] target Target system to get the TimeoutTime for. Pass UNASSIGNED_SYSTEM_ADDRESS to get the default value.
    /// \return Timeout time for a given system.
    TimeMS RakPeer_GetTimeoutTime(RakPeerInterface *self, const SystemAddress *target);

    /// \brief Returns the current MTU size
    /// \param[in] target Which system to get MTU for.  UNASSIGNED_SYSTEM_ADDRESS to get the default
    /// \return The current MTU size of the target system.
    int RakPeer_GetMTUSize(RakPeerInterface *self, const SystemAddress *target);

    /// \brief Returns the number of IP addresses this system has internally.
    /// \details Get the actual addresses from GetLocalIP()
    unsigned RakPeer_GetNumberOfAddresses(RakPeerInterface *self);

    /// Returns an IP address at index 0 to GetNumberOfAddresses-1 in ipList array.
    /// \param[in] index index into the list of IP addresses
    /// \return The local IP address at this index
    const char *RakPeer_GetLocalIP(RakPeerInterface *self, unsigned int index);

    /// Is this a local IP?
    /// Checks if this ip is in the ipList array.
    /// \param[in] An IP address to check, excluding the port.
    /// \return True if this is one of the IP addresses returned by GetLocalIP
    bool RakPeer_IsLocalIP(RakPeerInterface *self, const char *ip);

    /// \brief Allow or disallow connection responses from any IP.
    /// \details Normally this should be false, but may be necessary when connecting to servers with multiple IP addresses.
    /// \param[in] allow - True to allow this behavior, false to not allow. Defaults to false. Value persists between connections.
    void RakPeer_AllowConnectionResponseIPMigration(RakPeerInterface *self, bool allow);

    /// \brief Sends a one byte message ID_ADVERTISE_SYSTEM to the remote unconnected system.
    /// This will send our external IP outside the LAN along with some user data to the remote system.
    /// \pre The sender and recipient must already be started via a successful call to Initialize
    /// \param[in] host Either a dotted IP address or a domain name
    /// \param[in] remotePort Which port to connect to on the remote machine.
    /// \param[in] data Optional data to append to the packet.
    /// \param[in] dataLength Length of data in bytes.  Use 0 if no data.
    /// \param[in] connectionSocketIndex Index into the array of socket descriptors passed to socketDescriptors in RakPeer::Startup() to send on.
    /// \return False if IsActive()==false or the host is unresolvable. True otherwise.
    bool RakPeer_AdvertiseSystem(RakPeerInterface *self, const char *host, unsigned short remotePort, const char *data, int dataLength, unsigned connectionSocketIndex);

    /// \brief Controls how often to return ID_DOWNLOAD_PROGRESS for large message downloads.
    /// \details ID_DOWNLOAD_PROGRESS is returned to indicate a new partial message chunk, roughly the MTU size, has arrived.
    /// As it can be slow or cumbersome to get this notification for every chunk, you can set the interval at which it is returned.
    /// Defaults to 0 (never return this notification).
    /// \param[in] interval How many messages to use as an interval before a download progress notification is returned.
    void RakPeer_SetSplitMessageProgressInterval(RakPeerInterface *self, int interval);

    /// \brief Returns what was passed to SetSplitMessageProgressInterval().
    /// \return Number of messages to be recieved before a download progress notification is returned. Default to 0.
    int RakPeer_GetSplitMessageProgressInterval(RakPeerInterface *self);

    /// \brief Set how long to wait before giving up on sending an unreliable message.
    /// Useful if the network is clogged up.
    /// Set to 0 or less to never timeout.  Defaults to 0.
    /// \param[in] timeoutMS How many ms to wait before simply not sending an unreliable message.
    void RakPeer_SetUnreliableTimeout(RakPeerInterface *self, TimeMS timeoutMS);

    /// \brief Send a message to a host, with the IP socket option TTL set to 3.
    /// \details This message will not reach the host, but will open the router.
    /// \param[in] host The address of the remote host in dotted notation.
    /// \param[in] remotePort The port number to send to.
    /// \param[in] ttl Max hops of datagram, set to 3
    /// \param[in] connectionSocketIndex userConnectionSocketIndex.
    /// \remarks Used for NAT-Punchthrough
    void RakPeer_SendTTL(RakPeerInterface *self, const char *host, unsigned short remotePort, int ttl, unsigned connectionSocketIndex);

    // -------------------------------------------------------------------------------------------- Plugin Functions--------------------------------------------------------------------------------------------
    /// \brief Attaches a Plugin interface to an instance of the base class (RakPeer or PacketizedTCP) to run code automatically on message receipt in the Receive call.
    /// If the plugin returns false from PluginInterface::UsesReliabilityLayer(), which is the case for all plugins except PacketLogger, you can call AttachPlugin() and DetachPlugin() for this plugin while RakPeer is active.
    /// \param[in] messageHandler Pointer to the plugin to attach.
    void RakPeer_AttachPlugin(RakPeerInterface *self, PluginInterface2 *plugin);

    /// \brief Detaches a Plugin interface from the instance of the base class (RakPeer or PacketizedTCP) it is attached to.
    ///	\details This method disables the plugin code from running automatically on base class's updates or message receipt.
    /// If the plugin returns false from PluginInterface::UsesReliabilityLayer(), which is the case for all plugins except PacketLogger, you can call AttachPlugin() and DetachPlugin() for this plugin while RakPeer is active.
    /// \param[in] messageHandler Pointer to a plugin to detach.
    void RakPeer_DetachPlugin(RakPeerInterface *self, PluginInterface2 *messageHandler);

    // --------------------------------------------------------------------------------------------Miscellaneous Functions--------------------------------------------------------------------------------------------
    /// \brief Puts a message back in the receive queue in case you don't want to deal with it immediately.
    /// \param[in] packet The pointer to the packet you want to push back.
    /// \param[in] pushAtHead True to push the packet at the start of the queue so that the next receive call returns it.  False to push it at the end of the queue.
    /// \note Setting pushAtHead to false end makes the packets out of order.
    void RakPeer_PushBackPacket(RakPeerInterface *self, Packet *packet, bool pushAtHead);

    /// \internal
    /// \brief For a given system identified by \a guid, change the SystemAddress to send to.
    /// \param[in] guid The connection we are referring to
    /// \param[in] systemAddress The new address to send to
    void RakPeer_ChangeSystemAddress(RakPeerInterface *self, const RakNetGUID *guid, const SystemAddress *systemAddress);

    /// \brief Returns a packet for you to write to if you want to create a Packet for some reason.
    /// You can add it to the receive buffer with PushBackPacket
    /// \param[in] dataSize How many bytes to allocate for the buffer
    /// \return A packet.
    Packet *RakPeer_AllocatePacket(RakPeerInterface *self, unsigned dataSize);

    /// \brief Get the socket used with a particular active connection.
    /// The smart pointer reference counts the RakNetSocket object, so the socket will remain active as long as the smart pointer does, even if RakNet were to shutdown or close the connection.
    /// \note This sends a query to the thread and blocks on the return value for up to one second. In practice it should only take a millisecond or so.
    /// \param[in] target Which system.
    /// \return A smart pointer object containing the socket information about the target. Be sure to check IsNull() which is returned if the update thread is unresponsive, shutting down, or if this system is not connected.
    RakNetSocket2 *RakPeer_GetSocket(RakPeerInterface *self, const SystemAddress target);

    /// \brief Gets all sockets in use.
    /// \note This sends a query to the thread and blocks on the return value for up to one second. In practice it should only take a millisecond or so.
    /// \param[out] sockets List of RakNetSocket structures in use.
    // void RakPeer_GetSockets(RakPeerInterface* self, DataStructures::List<RakNetSocket2 *> *sockets);
    // void RakPeer_ReleaseSockets(RakPeerInterface* self, DataStructures::List<RakNetSocket2 *> *sockets);

    /// \internal
    void RakPeer_WriteOutOfBandHeader(RakPeerInterface *self, BitStream *bitStream);

    /// If you need code to run in the same thread as RakNet's update thread, this function can be used for that
    /// \param[in] _userUpdateThreadPtr C callback function
    /// \param[in] _userUpdateThreadData Passed to C callback function
    void RakPeer_SetUserUpdateThread(RakPeerInterface *self, void (*_userUpdateThreadPtr)(RakPeerInterface *, void *), void *_userUpdateThreadData);

    /// Set a C callback to be called whenever a datagram arrives
    /// Return true from the callback to have RakPeer handle the datagram. Return false and RakPeer will ignore the datagram.
    /// This can be used to filter incoming datagrams by system, or to share a recvfrom socket with RakPeer
    /// RNS2RecvStruct will only remain valid for the duration of the call
    void RakPeer_SetIncomingDatagramEventHandler(RakPeerInterface *self, bool (*_incomingDatagramEventHandler)(RNS2RecvStruct *));

    // --------------------------------------------------------------------------------------------Network Simulator Functions--------------------------------------------------------------------------------------------
    /// Adds simulated ping and packet loss to the outgoing data flow.
    /// To simulate bi-directional ping and packet loss, you should call this on both the sender and the recipient, with half the total ping and packetloss value on each.
    /// You can exclude network simulator code with the _RELEASE #define to decrease code size
    /// \deprecated Use http://www.jenkinssoftware.com/forum/index.php?topic=1671.0 instead.
    /// \note Doesn't work past version 3.6201
    /// \param[in] packetloss Chance to lose a packet. Ranges from 0 to 1.
    /// \param[in] minExtraPing The minimum time to delay sends.
    /// \param[in] extraPingVariance The additional random time to delay sends.
    void RakPeer_ApplyNetworkSimulator(RakPeerInterface *self, float packetloss, unsigned short minExtraPing, unsigned short extraPingVariance);

    /// Limits how much outgoing bandwidth can be sent per-connection.
    /// This limit does not apply to the sum of all connections!
    /// Exceeding the limit queues up outgoing traffic
    /// \param[in] maxBitsPerSecond Maximum bits per second to send.  Use 0 for unlimited (default). Once set, it takes effect immedately and persists until called again.
    void RakPeer_SetPerConnectionOutgoingBandwidthLimit(RakPeerInterface *self, unsigned maxBitsPerSecond);

    /// Returns if you previously called ApplyNetworkSimulator
    /// \return If you previously called ApplyNetworkSimulator
    bool RakPeer_IsNetworkSimulatorActive(RakPeerInterface *self);

    // --------------------------------------------------------------------------------------------Statistical Functions - Functions dealing with API performance--------------------------------------------------------------------------------------------

    /// \brief Returns a structure containing a large set of network statistics for the specified system.
    /// You can map this data to a string using the C style StatisticsToString() function
    /// \param[in] systemAddress Which connected system to get statistics for.
    /// \param[in] rns If you supply this structure,the network statistics will be written to it. Otherwise the method uses a static struct to write the data, which is not threadsafe.
    /// \return 0 if the specified system can't be found. Otherwise a pointer to the struct containing the specified system's network statistics.
    /// \sa RakNetStatistics.h
    RakNetStatistics *RakPeer_GetStatistics_address(RakPeerInterface *self, const SystemAddress *systemAddress, RakNetStatistics *rns);
    /// \brief Returns the network statistics of the system at the given index in the remoteSystemList.
    ///	\return True if the index is less than the maximum number of peers allowed and the system is active. False otherwise.
    bool RakPeer_GetStatistics(RakPeerInterface *self, const unsigned int index, RakNetStatistics *rns);
    /// \brief Returns the list of systems, and statistics for each of those systems
    /// Each system has one entry in each of the lists, in the same order
    /// \param[out] addresses SystemAddress for each connected system
    /// \param[out] guids RakNetGUID for each connected system
    /// \param[out] statistics Calculated RakNetStatistics for each connected system
    // void RakPeer_GetStatisticsList(RakPeerInterface* self, DataStructures::List<SystemAddress> *addresses, DataStructures::List<RakNetGUID> *guids, DataStructures::List<RakNetStatistics> *statistics);

    /// \Returns how many messages are waiting when you call Receive()
    unsigned int RakPeer_GetReceiveBufferSize(RakPeerInterface *self);

    // --------------------------------------------------------------------------------------------EVERYTHING AFTER THIS COMMENT IS FOR INTERNAL USE ONLY--------------------------------------------------------------------------------------------

    /// \internal
    // Call manually if RAKPEER_USER_THREADED==1 at least every 30 milliseconds.
    // updateBitStream should be:
    // 	BitStream updateBitStream( MAXIMUM_MTU_SIZE
    // #if LIBCAT_SECURITY==1
    //	+ cat::AuthenticatedEncryption::OVERHEAD_BYTES
    // #endif
    // );
    bool RakPeer_RunUpdateCycle(RakPeerInterface *self, BitStream *updateBitStream);

    /// \internal
    // Call manually if RAKPEER_USER_THREADED==1 at least every 30 milliseconds.
    // Call in a loop until returns false if the socket is non-blocking
    // remotePortRakNetWasStartedOn_PS3 and extraSocketOptions are from SocketDescriptor when the socket was created
    // bool RunRecvFromOnce( RakNetSocket *s );

    /// \internal
    bool RakPeer_SendOutOfBand(RakPeerInterface *self, const char *host, unsigned short remotePort, const char *data, BitSize_t dataLength, unsigned connectionSocketIndex);

#ifdef __cplusplus
}
#endif