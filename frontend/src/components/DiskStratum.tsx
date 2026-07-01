// frontend/src/components/DiskStratum.tsx
export function DiskStratum({ layout }: { layout?: { level: number; file_count: number }[] }) {
  return (
    <div className="flex flex-col h-full space-y-4">
      <h3 className="text-sm font-medium text-zinc-400 uppercase tracking-wider">SSTable Stratum (Disk)</h3>
      
      <div className="space-y-3 grow overflow-y-auto pr-2 custom-scrollbar">
        {/* Use optional chaining here */}
        {layout?.map((levelData) => (
          <div key={levelData.level} className="border border-zinc-800 bg-zinc-950/50 rounded-lg p-3">
            <div className="flex justify-between items-center mb-2">
              <p className="text-xs font-mono text-zinc-400">Level {levelData.level}</p>
              <span className="text-[10px] text-zinc-600">{levelData.file_count} files</span>
            </div>
            
            <div className="flex flex-wrap gap-1">
               {Array.from({ length: levelData.file_count }).map((_, j) => (
                 <div key={j} className="h-6 w-10 bg-zinc-800 rounded border border-zinc-700 flex items-center justify-center">
                    <span className="text-[8px] text-zinc-500 font-mono">.db</span>
                 </div>
               ))}
            </div>
          </div>
        )) ?? <div className="text-zinc-700 text-sm italic">Loading disk state...</div>}
      </div>
    </div>
  );
}