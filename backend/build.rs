fn main() {
    cxx_build::bridge("src/bridge.rs")
        .file("engine/src/wal.cpp")
        .file("engine/src/memtable.cpp")
        .file("engine/src/sstable.cpp")
        .file("engine/src/bloom.cpp")
        .file("engine/src/wrapper.cpp")
        .include("engine/include") // Point directly to the include folder
        .flag_if_supported("-std=c++17")
        .compile("lithos_engine");

    println!("cargo:rerun-if-changed=src/bridge.rs");
    println!("cargo:rerun-if-changed=engine/src/");
    println!("cargo:rerun-if-changed=engine/include/");
}