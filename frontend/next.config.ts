import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  async rewrites() {
    return [
      {
        source: "/api/:path*",
        destination: "http://127.0.0.1:8080/:path*", // Explicitly target 127.0.0.1
      },
    ];
  },
};

export default nextConfig;