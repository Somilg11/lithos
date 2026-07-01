# Lithos

<p align="center">
  <img src="./images/banner.png" alt="Lithos Banner" width="100%">
</p>


<h1 align="center">Lithos</h1>

<p align="center">
<b>High-Performance Log-Structured Merge (LSM) Tree Key-Value Storage Engine</b>
</p>

<p align="center">

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Rust](https://img.shields.io/badge/Rust-Stable-orange)
![Axum](https://img.shields.io/badge/Axum-Backend-success)
![Next.js](https://img.shields.io/badge/Next.js-Frontend-black)
![Tailwind](https://img.shields.io/badge/Tailwind-CSS-38BDF8)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS-lightgrey)

</p>

---

## Overview

Lithos is a custom-built embeddable key-value database built around a Log-Structured Merge Tree (LSM Tree).

The storage engine is implemented in **C++17**, exposed safely through a **Rust cxx FFI bridge**, and monitored through a **Next.js telemetry dashboard**.

### Highlights

- Skip List MemTable
- Write Ahead Log (WAL)
- Sorted String Tables (SSTables)
- Multi-Level Storage
- Background Compaction
- Bloom Filters
- Compression
- Rust ↔ C++ Zero-cost FFI
- Axum REST API
- Live Dashboard

---

# Table of Contents

- Architecture
- Features
- Tech Stack
- Write Path
- Read Path
- LSM Layout
- Compaction
- Bloom Filters
- Storage Hierarchy
- Request Lifecycle
- Project Structure
- Dashboard
- API
- Getting Started
- Roadmap
- Project Status

---

## Preview

<p align="center">
    <img src="./images/preview.png" width="95%">
</p>


# System Architecture

```mermaid
graph LR
Client[Client]
API[Rust Axum API]
FFI[cxx FFI]
Engine[C++ Engine]
WAL[(WAL)]
Mem[(Skip List)]
L0[(L0)]
L1[(L1)]
L2[(L2)]
Bloom[Bloom Filter]
Dashboard[Next.js Dashboard]

Client-->API
API-->FFI
FFI-->Engine
Engine-->WAL
Engine-->Mem
Mem--Flush-->L0
L0--Compaction-->L1
L1--Compaction-->L2
API-->Bloom
Dashboard-->API
```

<p align="center">
    <img src="./images//LithosDB-Storage-Engine-Visualizer.png" width="90%">
</p>

# Features

| Feature | Status |
|---------|--------|
| Skip List MemTable | ✅ |
| WAL | ✅ |
| SSTables | ✅ |
| Multi-Level SSTables | ✅ |
| Bloom Filter | ✅ |
| Background Compaction | ✅ |
| Compression | ✅ |
| Rust FFI | ✅ |
| REST API | ✅ |
| Live Dashboard | ✅ |

# Tech Stack

| Layer | Technology |
|------|------------|
| Storage Engine | C++17 |
| FFI | Rust cxx |
| Backend | Rust + Axum |
| Frontend | Next.js |
| UI | Tailwind CSS |

## Zero-Cost FFI

Lithos communicates between Rust and C++ through the `cxx` crate with negligible overhead.

<p align="center">
    <img src="./images/wrapper-performance.png" width="80%">
</p>

# Write Path

```mermaid
flowchart LR
A[PUT]
B[Append WAL]
C[Insert MemTable]
D{Full?}
E[Return]
F[Flush]
G[Compaction]

A-->B-->C-->D
D--No-->E
D--Yes-->F-->G-->E
```

# Read Path

```mermaid
flowchart TD
A[GET]
A-->B[MemTable]
B--Found-->C[Return]
B--Miss-->D[Bloom Filter]
D--Definitely No-->E[NULL]
D--Maybe-->F[SSTables]
F-->C
```

# LSM Tree

```text
Memory
┌─────────────┐
│ MemTable    │
└─────┬───────┘
      │ Flush
      ▼
┌─────────────┐
│ L0 SSTables │
└─────┬───────┘
      │ Compaction
      ▼
┌─────────────┐
│ L1 SSTables │
└─────┬───────┘
      ▼
┌─────────────┐
│ L2 SSTables │
└─────────────┘
```

<p align="center">
    <img src="./images/LSM-Tree-Engine-Simulation.png" width="90%">
</p>

# Flush Lifecycle

```mermaid
stateDiagram-v2
[*]-->Writing
Writing-->Writing
Writing-->Flush: Threshold
Flush-->SSTable
SSTable-->Writing
```

# Skip List (MemTable)

The MemTable uses a probabilistic Skip List for O(log n) insertion and lookup.

<p align="center">
    <img src="./images/skip-list.png" width="80%">
</p>

# Compaction

```mermaid
flowchart LR
A[L0-1]
B[L0-2]
C[L0-3]
D[Merge]
E[L1]

A-->D
B-->D
C-->D
D-->E
```

# Bloom Filter

```mermaid
flowchart LR
Query-->Bloom
Bloom--No-->Stop[Skip Disk]
Bloom--Maybe-->Disk[SSTables]
```

# Request Lifecycle

```mermaid
sequenceDiagram
participant Client
participant Rust
participant CPP
participant WAL
participant Mem
participant Disk

Client->>Rust: PUT
Rust->>CPP: FFI
CPP->>WAL: Append
CPP->>Mem: Insert
CPP-->>Rust: OK
Rust-->>Client: 200
Mem->>Disk: Flush
Disk->>Disk: Compaction
```

# Project Structure

```text
Lithos/
├── engine/
├── backend/
├── frontend/
├── ffi/
├── docs/
└── data/
```

# Deployment Topology

<p align="center">
    <img src="./images/Lithos-Deployment-Topology.png" width="95%">
</p>

# Dashboard

The dashboard visualizes:

- MemTable occupancy
- WAL activity
- SSTables
- Compaction
- Bloom filter efficiency
- Storage hierarchy
- Query metrics
- Disk usage

> Add screenshots or GIFs:
>
> - docs/assets/dashboard.png
> - docs/assets/demo.gif

# REST API

## PUT

```bash
curl -X POST http://localhost:8080/set \
-H "Content-Type: application/json" \
-d '{"key":"hello","value":"world"}'
```

## GET

```bash
curl http://localhost:8080/get?key=hello
```

# Getting Started

## Backend

```bash
cd backend
cargo run
```

## Frontend

```bash
cd frontend
npm install
npm run dev
```

Open:

```
http://localhost:3000
```

# Roadmap

- Distributed replication
- MVCC
- Transactions
- Range scans
- Parallel compaction
- Block cache
- SIMD optimizations

# Project Status

- [x] Core LSM Engine
- [x] WAL
- [x] Skip List
- [x] Bloom Filters
- [x] Multi-Level SSTables
- [x] Background Compaction
- [x] Compression
- [x] Rust FFI
- [x] Axum API
- [x] Next.js Dashboard

---

Built as a systems engineering project focused on storage engines, database internals, and high-performance systems programming.
