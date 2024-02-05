use std::path::PathBuf;

fn generate_bindings() {
    let bindings = bindgen::Builder::default()
        .header("raknet-c/raknet.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(std::env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}

fn build_raknet() {
    println!("cargo:rerun-if-changed=RakNet");

    let root_dir = PathBuf::from(std::env::var("CARGO_MANIFEST_DIR").unwrap());
    let raknet_dir = root_dir.join("raknet-c");
    let mut build = cmake::Config::new(&raknet_dir);

    let cat_audit = if std::env::var("CARGO_FEATURE_CAT_AUDIT").is_ok() {
        "TRUE"
    } else {
        "FALSE"
    };

    let dst = build
        .define("RAKNET_ENABLE_SAMPLES", "FALSE")
        .define("RAKNET_ENABLE_DLL", "FALSE")
        .define("RAKNET_CAT_AUDIT", cat_audit)
        .build();

    println!("cargo:rustc-link-lib=stdc++");
    println!(
        "cargo:rustc-link-search=native={}",
        dst.join("build").display()
    );
    println!(
        "cargo:rustc-link-search=native={}",
        dst.join("build/RakNet/Lib/LibStatic").display()
    );

    println!("cargo:rustc-link-lib=static=raknet-c");
    println!("cargo:rustc-link-lib=static=RakNetLibStatic");
}

fn main() {
    build_raknet();
    generate_bindings();
}
