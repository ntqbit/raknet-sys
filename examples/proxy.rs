use std::{
    collections::{HashMap, VecDeque},
    time::Duration,
};

use raknet::{
    AddressOrGuid, ClientEasyHandshake, ConnectionAttemptResult, DefaultMessageIDTypes, Packet,
    PublicKey, RakNetGUID, RakPeer, SocketDescriptor, SocketFamily, StartupResult, SystemAddress,
};

pub struct ProxyConnection {
    client: RakPeer,
    messages: VecDeque<Vec<u8>>,
    guid: RakNetGUID,
    system_address: SystemAddress,
}

impl core::fmt::Debug for ProxyConnection {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("ProxyConnection")
            .field("guid", &self.guid)
            .field("system_address", &self.system_address)
            .finish()
    }
}

impl ProxyConnection {
    pub fn new(guid: RakNetGUID, system_address: SystemAddress) -> Result<Self, &'static str> {
        let client = RakPeer::new();

        client.AllowConnectionResponseIPMigration(false);
        client.SetOccasionalPing(true);

        let socket_descriptors = [SocketDescriptor {
            port: 0,
            host_address: String::new(),
            socket_family: SocketFamily::Ipv4,
        }];

        let result = client.Startup(8, &socket_descriptors);

        if !matches!(result, StartupResult::Started) {
            return Err("Could not start");
        }

        let car = client.Connect("127.0.0.1", 4564, None, &PublicKey::AcceptAnyPublicKey);

        if !matches!(car, ConnectionAttemptResult::Started) {
            return Err("Could not start connection attempt");
        }

        Ok(Self {
            client,
            messages: VecDeque::new(),
            guid,
            system_address,
        })
    }

    pub fn system_address(&self) -> SystemAddress {
        self.system_address
    }

    pub fn serve(&mut self) -> Vec<Vec<u8>> {
        self.client
            .ReceiveAll()
            .into_iter()
            .filter_map(|packet| self.handle_packet(packet))
            .collect()
    }

    fn handle_packet(&mut self, packet: Packet) -> Option<Vec<u8>> {
        let packet_id = packet.data[0];

        println!("Remote: {:X?}", packet.data);

        if packet_id >= DefaultMessageIDTypes::ID_ADVERTISE_SYSTEM as u8 {
            Some(packet.data)
        } else {
            None
        }
    }

    pub fn forward_packet(&mut self, packet: Packet) {
        let connected_system_address = self.client.GetSystemAddressFromIndex(0);
        if connected_system_address.is_unassigned() {
            // Not yet connected. Save the packet for later.
            self.messages.push_back(packet.data);
            return;
        }

        // Send queued packets.
        while let Some(queued_message) = self.messages.pop_front() {
            self.send_message(&queued_message);
        }

        self.send_message(&packet.data);
    }

    fn send_message(&self, data: &[u8]) {
        self.client.Send().data(data).broadcast().send().unwrap();
    }
}

pub struct RakNetProxy {
    server: RakPeer,
    connections: HashMap<RakNetGUID, ProxyConnection>,
}

impl RakNetProxy {
    pub fn new() -> Result<Self, &'static str> {
        let server = RakPeer::new();

        server.SetOccasionalPing(true);
        server.SetUnreliableTimeout(Duration::from_secs(1));
        server.SetTimeoutTime(Duration::from_secs(30), SystemAddress::unassigned());

        let mut handshake = ClientEasyHandshake::new();
        let Some((public_key, private_key)) = handshake.GenerateServerKey() else {
            return Err("Failed to generate server key.");
        };

        if !server.InitializeSecurity(&public_key, &private_key, false) {
            return Err("Failed to initialize security.");
        }

        let socket_descriptors = [SocketDescriptor {
            port: 4564,
            host_address: String::new(),
            socket_family: SocketFamily::Ipv4,
        }];

        let result = server.Startup(8, &socket_descriptors);

        if !matches!(result, StartupResult::Started) {
            return Err("Could not start");
        }

        server.SetMaximumIncomingConnections(8);

        Ok(Self {
            server,
            connections: HashMap::new(),
        })
    }

    pub fn serve(&mut self) {
        for packet in self.server.ReceiveAll() {
            self.handle_packet(packet);
        }

        for connection in self.connections.values_mut() {
            let system_address = connection.system_address();
            let messages = connection.serve();

            for message in messages {
                self.server
                    .Send()
                    .data(&message)
                    .system_identifier(AddressOrGuid::Address(system_address))
                    .send()
                    .unwrap();
            }
        }
    }

    fn handle_packet(&mut self, packet: Packet) {
        // Is it possible?
        assert!(!packet.guid.is_unassigned());

        let packet_id = packet.data[0];

        // Handle new connection.
        if packet_id == DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION as u8 {
            println!(
                "New incoming connection from: {}",
                packet.system_address.to_string()
            );

            self.connections.insert(
                packet.guid,
                ProxyConnection::new(packet.guid, packet.system_address).unwrap(),
            );
            return;
        }

        let connection = self
            .connections
            .get_mut(&packet.guid)
            .expect("received a packet from an unconnected system");

        println!("{}: {:X?}", packet.system_address.to_string(), packet.data);

        connection.forward_packet(packet);
    }
}

fn main() {
    let mut proxy = RakNetProxy::new().unwrap();

    loop {
        std::thread::sleep(Duration::from_millis(30));

        proxy.serve();
    }
}
