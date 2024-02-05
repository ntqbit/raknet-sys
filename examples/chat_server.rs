use std::{
    sync::atomic::{AtomicBool, Ordering},
    time::Duration,
};

use raknet::{
    AddressOrGuid, ClientEasyHandshake, RakPeer, SocketDescriptor, SocketFamily, StartupResult,
    SystemAddress,
};

fn main() {
    let server = RakPeer::new();

    server.SetOccasionalPing(true);
    server.SetUnreliableTimeout(Duration::from_secs(1));
    server.SetTimeoutTime(Duration::from_secs(30), SystemAddress::unassigned());

    let mut handshake = ClientEasyHandshake::new();
    let Some((public_key, private_key)) = handshake.GenerateServerKey() else {
        println!("Failed to generate server key.");
        return;
    };

    if !server.InitializeSecurity(&public_key, &private_key, false) {
        println!("Failed to initialize security.");
        return;
    }

    let socket_descriptors = [SocketDescriptor {
        port: 4564,
        host_address: String::new(),
        socket_family: SocketFamily::Ipv4,
    }];

    let result = server.Startup(8, &socket_descriptors);

    if !matches!(result, StartupResult::Started) {
        println!("Could not start: {:?}", result);
        return;
    }

    server.SetMaximumIncomingConnections(8);

    println!("My IP addresses:");
    for (i, addr) in server.GetInternalAddresses().into_iter().enumerate() {
        println!("\t{}. {}", i, addr.to_string());
    }

    println!(
        "My GUID is: {}",
        server
            .GetGuidFromSystemAddress(SystemAddress::unassigned())
            .to_string()
    );

    let stop: AtomicBool = AtomicBool::new(false);

    std::thread::scope(|s| {
        s.spawn(|| loop {
            let mut message = String::new();
            std::io::stdin().read_line(&mut message).unwrap();

            match message.trim_end() {
                "quit" => {
                    stop.store(true, Ordering::SeqCst);
                    break;
                }
                "stat" => {
                    let system_address = server.GetSystemAddressFromIndex(0);
                    let stats = server.GetStatistics(system_address);
                    println!("Stats: {:#?}", stats);

                    let average_ping =
                        server.GetAveragePing(AddressOrGuid::Address(system_address));
                    println!("Average ping: {:#?}", average_ping);
                }
                "disconnect" => {
                    server.CloseConnection(
                        AddressOrGuid::Address(server.GetSystemAddressFromIndex(0)),
                        false,
                    );
                    println!("Disconnected");
                }
                "ping" => {
                    let s = server.GetSystemAddressFromIndex(0);
                    if !s.is_unassigned() {
                        server.Ping(s);
                        println!("Ping sent.");
                    }
                }
                "get_last_ping" => {
                    let s = server.GetSystemAddressFromIndex(0);
                    if !s.is_unassigned() {
                        let last_ping = server.GetLastPing(AddressOrGuid::Address(s));
                        println!("Last ping: {:?}", last_ping);
                    }
                }
                message => {
                    server
                        .Send()
                        .data(format!("Server: {}", message).as_bytes())
                        .broadcast()
                        .send()
                        .unwrap();
                }
            }
        });

        while !stop.load(Ordering::SeqCst) {
            std::thread::sleep(Duration::from_millis(30));

            for packet in server.ReceiveAll() {
                let data_len = packet.data.len();
                let packet_id = packet.data[0];

                let message = match String::from_utf8(packet.data) {
                    Ok(message) => message,
                    Err(err) => format!("{:X?}", err.into_bytes()),
                };

                println!(
                    "Receive from {}{}: [{:X}] ({}/{}) {}",
                    packet.system_address.to_string(),
                    if packet.local { "(local)" } else { "" },
                    packet_id,
                    data_len,
                    packet.bitsize,
                    message,
                );
            }
        }

        server.Shutdown(Duration::from_millis(300));
    });
}
