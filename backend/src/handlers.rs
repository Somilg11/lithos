use axum::{
    extract::{Query, State},
    Json,
};
use serde::{Deserialize, Serialize};
use std::sync::Arc;
use crate::state::AppState;

#[derive(Deserialize)]
pub struct SetRequest {
    pub key: String,
    pub value: String,
}

#[derive(Deserialize)]
pub struct GetRequest {
    pub key: String,
}

#[derive(Serialize)]
pub struct GetResponse {
    pub value: String,
}

pub async fn set_handler(State(state): State<Arc<AppState>>, Json(payload): Json<SetRequest>) -> &'static str {
    let mut db = state.db.lock().unwrap();
    db.pin_mut().put(&payload.key, &payload.value);
    "OK"
}

pub async fn get_handler(State(state): State<Arc<AppState>>, Query(params): Query<GetRequest>) -> Json<GetResponse> {
    let mut db = state.db.lock().unwrap();
    let val = db.pin_mut().get(&params.key);
    Json(GetResponse { value: val })
}

#[derive(Serialize)]
pub struct DiskLevel {
    pub level: usize,
    pub file_count: usize,
}

#[derive(Serialize)]
pub struct MetricsResponse {
    pub memtable_size: usize,
    pub total_reads: usize,
    pub total_writes: usize,
    pub bloom_saved_reads: usize, // <-- ADDED
    pub disk_layout: Vec<DiskLevel>,
}

pub async fn metrics_handler(State(state): State<Arc<AppState>>) -> Json<MetricsResponse> {
    let mut db = state.db.lock().unwrap();
    
    // Fetch live data from C++ engine
    let mem_size = db.pin_mut().get_memtable_size();
    let reads = db.pin_mut().get_total_reads(); // <-- ADDED
    let saved = db.pin_mut().get_saved_reads(); // <-- ADDED
    
    let mut layout = Vec::new();
    for i in 0..6 {
        let count = db.pin_mut().get_level_count(i as i32);
        layout.push(DiskLevel { level: i, file_count: count as usize });
    }

    Json(MetricsResponse {
        memtable_size: mem_size,
        total_reads: reads, // <-- UPDATED
        total_writes: 0, 
        bloom_saved_reads: saved, // <-- ADDED
        disk_layout: layout,
    })
}