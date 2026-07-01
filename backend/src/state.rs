use std::sync::Mutex;
use crate::bridge;

// Explicitly tell Rust that our C++ engine is safe to move and share across web threads.
// This allows Axum to wrap it in an Arc<Mutex<...>> for concurrent requests.
unsafe impl Send for bridge::ffi::Database {}
unsafe impl Sync for bridge::ffi::Database {}

pub struct AppState {
    pub db: Mutex<cxx::UniquePtr<bridge::ffi::Database>>,
}