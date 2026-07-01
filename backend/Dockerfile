FROM rust:1.75 AS builder

WORKDIR /app

# Install C++ compiler
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    && rm -rf /var/lib/apt/lists/*

COPY Cargo.toml build.rs ./
COPY src ./src
COPY engine ./engine

RUN cargo build --release

FROM debian:bookworm-slim

WORKDIR /app

RUN apt-get update && apt-get install -y \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/target/release/lithos /app/lithos

EXPOSE 3000

CMD ["/app/lithos"]
