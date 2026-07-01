// frontend/src/components/MemTable.tsx
export function MemTable({ items }: { items: {k: string, v: string}[] }) {
  return (
    <div className="flex flex-col h-full space-y-4">
      <h3 className="text-sm font-medium text-zinc-400 uppercase tracking-wider">L0 Buffer (RAM)</h3>
      
      <div className="grow space-y-2 overflow-y-auto pr-2">
        {items.length === 0 ? (
          <div className="h-32 flex items-center justify-center border border-dashed border-zinc-800 rounded-lg text-zinc-700 text-sm italic">
            MemTable is empty
          </div>
        ) : (
          items.map((item, idx) => (
            <div key={idx} className="flex justify-between items-center bg-zinc-900 border border-zinc-800 rounded px-3 py-2 text-sm font-mono animate-in fade-in slide-in-from-left-2">
              <span className="text-zinc-300">{item.k}</span>
              <span className="text-zinc-500 text-xs">→</span>
              <span className="text-zinc-100">{item.v}</span>
            </div>
          ))
        )}
      </div>
    </div>
  );
}