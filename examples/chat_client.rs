use std::{
    sync::atomic::{AtomicBool, Ordering},
    time::Duration,
};

use raknet::{
    AddressOrGuid, ConnectionAttemptResult, PublicKey, RakPeer, SocketDescriptor, SocketFamily,
    StartupResult, SystemAddress,
};

fn main() {
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
        println!("Could not start: {:?}", result);
        return;
    }

    let car = client.Connect("127.0.0.1", 4564, None, &PublicKey::AcceptAnyPublicKey);

    if !matches!(car, ConnectionAttemptResult::Started) {
        println!("Could not start connection attempt: {:?}", car);
        return;
    }

    println!("My IP addresses:");
    for (i, addr) in client.GetLocalIpAddresses().into_iter().enumerate() {
        println!("\t{}. {}", i, addr);
    }

    println!(
        "My GUID is: {}",
        client
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
                    let system_address = client.GetSystemAddressFromIndex(0);
                    let stats = client.GetStatistics(system_address);
                    println!("Stats: {:#?}", stats);

                    let average_ping =
                        client.GetAveragePing(AddressOrGuid::Address(system_address));
                    println!("Average ping: {:#?}", average_ping);
                }
                "disconnect" => {
                    client.CloseConnection(
                        AddressOrGuid::Address(client.GetSystemAddressFromIndex(0)),
                        false,
                    );
                    println!("Disconnected");
                }
                "ping" => {
                    let s = client.GetSystemAddressFromIndex(0);
                    if !s.is_unassigned() {
                        client.Ping(s);
                        println!("Ping sent.");
                    }
                }
                "get_last_ping" => {
                    let s = client.GetSystemAddressFromIndex(0);
                    if !s.is_unassigned() {
                        let last_ping = client.GetLastPing(AddressOrGuid::Address(s));
                        println!("Last ping: {:?}", last_ping);
                    }
                }
                message => {
                    client
                        .Send()
                        .data(message.as_bytes())
                        .broadcast()
                        .send()
                        .unwrap();
                }
            }
        });

        while !stop.load(Ordering::SeqCst) {
            std::thread::sleep(Duration::from_millis(30));

            for packet in client.ReceiveAll() {
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

        client.Shutdown(Duration::from_millis(300));
    });
}
