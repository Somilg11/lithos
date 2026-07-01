use axum::{routing::{get, post}, Router};
use axum::http::Method; // <-- ADDED: Need this for explicitly allowing POST
use tower_http::cors::{Any, CorsLayer};
use std::sync::{Arc, Mutex};
use tokio::net::TcpListener;

mod bridge;
mod handlers;
mod state;

use state::AppState;
use handlers::{set_handler, get_handler, metrics_handler};

#[tokio::main]
async fn main() {
    let state = Arc::new(AppState {
        db: Mutex::new(bridge::ffi::new_database()),
    });

    // 1. Bulletproof CORS: Explicitly allow GET and POST methods
    let cors = CorsLayer::new()
        .allow_origin(Any)
        .allow_methods([Method::GET, Method::POST])
        .allow_headers(Any);

    let app = Router::new()
        .route("/set", post(set_handler))
        .route("/get", get(get_handler))
        .route("/metrics", get(metrics_handler))
        .layer(cors) // 2. Apply the updated CORS layer
        .with_state(state);

    // 3. Bind to 0.0.0.0 so localhost resolves perfectly
    let listener = TcpListener::bind("0.0.0.0:8080").await.unwrap();
    println!("Lithos API running on http://localhost:8080");
    axum::serve(listener, app).await.unwrap();
}