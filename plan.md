# Lithos Implementation Plan

## Phase 1: Core Storage Engine (C++)
Focus entirely on raw memory management and disk I/O.
*   **MemTable:** Implement a Skip List or Red-Black tree wrapper for `O(log N)` insertions.
*   **Write-Ahead Log (WAL):** Build a fast, append-only binary file logger for crash recovery.
*   **SSTable Flusher:** Write logic to dump the in-memory tree to a sorted `.db` file on disk when a threshold is reached.
*   **Bloom Filter:** Implement a bit-array (`std::vector<bool>`) with MurmurHash to check for key existence before hitting the disk.
*   **Compaction:** Create a background thread running a K-way merge algorithm to consolidate older SSTables.

## Phase 2: FFI Bridge & Network Layer (Rust)
Wrap the C++ core in a memory-safe, high-concurrency API.
*   **FFI Setup:** Configure the `cxx` crate in `build.rs` to compile the C++ engine alongside Rust.
*   **Bindings:** Expose core C++ methods (`put`, `get`, `get_metrics`) across the FFI boundary.
*   **Web Server:** Stand up an Axum HTTP server routing JSON requests to the Rust bridge.
*   **Concurrency:** Use `tokio` to handle multiple incoming API connections without blocking the C++ background threads.

## Phase 3: Telemetry & Visualization (Next.js)
Build a real-time monitor using a bento grid layout with Geist typography.
*   **UI Framework:** Next.js with TypeScript and Tailwind CSS.
*   **Design System:** Implement a minimalist aesthetic using a strict zinc color palette, glassmorphism panels, and dashed borders for structural elements.
*   **Live Metrics:** Poll the Rust `/metrics` endpoint to display throughput, latency, and Bloom filter accuracy.
*   **Storage Animation:** Build dynamic UI blocks that visually stack when MemTables flush to disk and merge during compaction.

## Phase 4: Containerization & Deployment
Package the system for local testing and cloud deployment.
*   **Backend Dockerfile:** Create a multi-stage build starting with a C++/Rust compiler toolchain, finalizing in a slim runtime image.
*   **Frontend Dockerfile:** Standard Node.js build for the Next.js dashboard.
*   **Docker Compose:** Map persistent volumes for the WAL/SSTables and network the Next.js frontend to the Rust API.