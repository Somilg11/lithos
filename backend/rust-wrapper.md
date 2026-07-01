# Rust FFI Wrapper & API Server

## What is it?
The Rust wrapper acts as a safe, concurrent, and high-performance network layer for the underlying C++ storage engine. It uses Foreign Function Interface (FFI) via the `cxx` crate to allow Rust code to directly call C++ methods without the heavy overhead of inter-process communication or deep copying.

## Why is it implemented?
While C++ provides absolute control over memory and disk I/O—which is essential for a database engine—it is notoriously dangerous to expose directly to the internet. A single memory leak, buffer overflow, or dangling pointer can crash the entire system or create severe security vulnerabilities.

Rust solves this by acting as a shield:
1. **Network Safety:** Rust handles incoming HTTP requests, async routing, and JSON parsing with guaranteed memory safety.
2. **Concurrency:** Tokio provides a massive, thread-pool-backed async runtime that can handle thousands of concurrent client connections without blocking.
3. **Zero-Cost Abstraction:** Instead of translating data back and forth, Rust passes raw memory slices (`&str`) directly to C++. The C++ engine reads the exact memory address Rust allocated, ensuring microsecond latency.

## How it works (The Data Flow)

### 1. The Build Process
Unlike standard Node or Python apps, Rust takes absolute control of the C++ compiler.
* The `build.rs` script intercepts the build process.
* It looks at `bridge.rs` and the C++ headers to automatically generate the necessary binding code.
* It invokes the system's C++ compiler (like `g++` or `clang++`), compiles the core engine, and statically links it directly into the final Rust binary.

### 2. The Execution Path (Write Request)
1. **Receive:** The Axum web server receives a `POST /set` JSON payload.
2. **Lock:** The Rust handler requests access to the shared `AppState`. It acquires a thread-safe `Mutex` lock on the C++ database pointer.
3. **Cross Boundary:** Rust calls `db.put(&key, &value)`.
4. **C++ Execution:** Execution jumps into the C++ `wrapper.cpp`. C++ reads the Rust string memory, appends the data to the WAL, and inserts it into the Skip List MemTable.
5. **Return & Release:** C++ finishes and returns control to Rust. Rust drops the Mutex lock, freeing the database for the next request, and sends a `200 OK` HTTP response to the client.

## Implementation Details (The FFI Boundary)
* **`cxx` Crate:** The standard library for safe Rust/C++ interop. It strictly enforces which types can cross the boundary to prevent undefined behavior.
* **`wrapper.cpp`:** Because `cxx` cannot directly understand complex C++ objects like our `Skip List`, we create a clean, flattened `Database` class in C++ that exposes only simple `put` and `get` string methods.
* **`unsafe impl Send / Sync`:** By default, Rust assumes C++ pointers are dangerous and prevents them from being shared across web server threads. By explicitly implementing `Send` and `Sync`, we take manual responsibility for the thread safety of the C++ engine, allowing Axum to manage it concurrently.