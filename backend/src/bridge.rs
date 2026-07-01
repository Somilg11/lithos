#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        // Since build.rs includes the folder, we just need the filename now
        include!("wrapper.h"); 

        type Database;

        fn new_database() -> UniquePtr<Database>;
        fn put(self: Pin<&mut Database>, key: &str, value: &str);
        fn get(self: Pin<&mut Database>, key: &str) -> String;
        fn get_memtable_size(self: Pin<&mut Database>) -> usize;
        fn get_level_count(self: Pin<&mut Database>, level: i32) -> i32;
        
        // <-- ADDED FOR BLOOM FILTER TELEMETRY
        fn get_total_reads(self: Pin<&mut Database>) -> usize;
        fn get_saved_reads(self: Pin<&mut Database>) -> usize;
    }
}