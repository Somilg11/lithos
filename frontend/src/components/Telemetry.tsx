export function Telemetry({ totalReads = 0, savedReads = 0 }: { totalReads?: number, savedReads?: number }) {
  return (
    <div className="flex flex-col md:flex-row items-start md:items-center justify-between h-full gap-6">
      <div className="flex items-center space-x-4">
        <div className="h-3 w-3 rounded-full bg-green-500 animate-pulse shadow-[0_0_10px_rgba(34,197,94,0.5)]" />
        <div>
          <h2 className="text-lg font-semibold tracking-tight text-zinc-200">Lithos Core Engine</h2>
          <p className="text-xs font-mono text-zinc-500">System online and stable</p>
        </div>
      </div>
      
      {/* <-- ADDED BLOOM FILTER METRICS UI */}
      <div className="flex items-center gap-6 border-l border-zinc-800 pl-6">
        <div className="flex flex-col">
          <span className="text-zinc-600 text-[10px] mb-1 uppercase tracking-wider">Total Queries</span>
          <span className="text-zinc-100 font-mono text-xl">{totalReads}</span>
        </div>
        <div className="flex flex-col">
          <span className="text-zinc-600 text-[10px] mb-1 uppercase tracking-wider">Bloom I/O Saved</span>
          <span className="text-green-400 font-mono text-xl font-bold">{savedReads}</span>
        </div>
      </div>

      <div className="flex flex-wrap gap-8 text-sm font-mono text-zinc-400">
        <div className="flex flex-col">
          <span className="text-zinc-600 mb-1">Architecture</span>
          <span className="text-zinc-100 bg-zinc-800/50 px-2 py-1 rounded">C++17</span>
        </div>
        <div className="flex flex-col">
          <span className="text-zinc-600 mb-1">Network Bridge</span>
          <span className="text-zinc-100 bg-zinc-800/50 px-2 py-1 rounded">Rust FFI</span>
        </div>
        <div className="flex flex-col">
          <span className="text-zinc-600 mb-1">UI Client</span>
          <span className="text-zinc-100 bg-zinc-800/50 px-2 py-1 rounded">Next.js</span>
        </div>
      </div>
    </div>
  );
}