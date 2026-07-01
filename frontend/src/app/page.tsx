'use client';

import { BentoGrid, BentoItem } from '@/components/BentoGrid';
import { MemTable } from '@/components/MemTable';
import { DiskStratum } from '@/components/DiskStratum';
import { get, set, metrics } from '@/lib/api';
import { useEffect, useState } from 'react';
import { Telemetry } from '@/components/Telemetry';

export default function Home() {
  // --- Engine Metrics State ---
  const [memtableSize, setMemtableSize] = useState(0);
  const [totalReads, setTotalReads] = useState(0);         // <-- ADDED
  const [bloomSavedReads, setBloomSavedReads] = useState(0); // <-- ADDED

  // --- Interactive State ---
  const [status, setStatus] = useState<'Idle' | 'Writing' | 'Reading'>('Idle');
  const [writeKey, setWriteKey] = useState('');
  const [writeValue, setWriteValue] = useState('');
  const [readKey, setReadKey] = useState('');
  const [readResult, setReadResult] = useState<string | null>(null);
  const [memtableItems, setMemtableItems] = useState<{k: string, v: string}[]>([]);
  const [diskLayout, setDiskLayout] = useState<{level: number, file_count: number}[]>([]);

  useEffect(() => {
    const fetchMetrics = async () => {
      try {
        const data = await metrics();
        setMemtableSize(data.memtable_size);
        setDiskLayout(data.disk_layout);
        setTotalReads(data.total_reads);             // <-- ADDED
        setBloomSavedReads(data.bloom_saved_reads);  // <-- ADDED
      } catch (error) {
        console.error('Failed to fetch metrics:', error);
      }
    };

    fetchMetrics();
    const interval = setInterval(fetchMetrics, 1000);
    return () => clearInterval(interval);
  }, []);

  const handleWrite = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!writeKey || !writeValue) return;
    
    setStatus('Writing');
    try {
      await set(writeKey, writeValue);
      setMemtableItems(prev => [{k: writeKey, v: writeValue}, ...prev].slice(0, 8));
      setWriteKey('');
      setWriteValue('');
    } catch (err) {
      console.error(err);
    } finally {
      setStatus('Idle');
    }
  };

  const handleRead = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!readKey) return;

    setStatus('Reading');
    try {
      const res = await get(readKey);
      setReadResult(res.value || "Not Found");
    } catch (err) {
      setReadResult("Error fetching data");
    } finally {
      setStatus('Idle');
    }
  };

  return (
    <div className="min-h-screen bg-zinc-950 text-zinc-100 flex flex-col">
      <header className="border-b border-zinc-800/80 p-6 bg-zinc-950/50 backdrop-blur-md sticky top-0 z-10">
        <h1 className="text-2xl font-bold tracking-tight text-zinc-100">Lithos Dashboard</h1>
        <p className="text-zinc-500 text-sm font-mono mt-1">LSM-Tree Storage Engine</p>
      </header>
      
      <main className="p-6 max-w-7xl mx-auto w-full grow flex flex-col justify-center">
        <BentoGrid>
          
          <BentoItem className="md:col-span-1 flex flex-col gap-8">
            <form onSubmit={handleWrite} className="flex flex-col gap-3">
              <h3 className="text-sm font-medium text-zinc-400 uppercase tracking-wider mb-2">Write (Put)</h3>
              <input 
                className="bg-zinc-950 border border-zinc-800 focus:border-zinc-600 rounded-lg px-4 py-2 text-sm outline-none transition-colors w-full placeholder:text-zinc-700" 
                placeholder="Key (e.g., user_1)" 
                value={writeKey} 
                onChange={(e) => setWriteKey(e.target.value)}
              />
              <input 
                className="bg-zinc-950 border border-zinc-800 focus:border-zinc-600 rounded-lg px-4 py-2 text-sm outline-none transition-colors w-full placeholder:text-zinc-700" 
                placeholder="Value (e.g., architect)" 
                value={writeValue} 
                onChange={(e) => setWriteValue(e.target.value)}
              />
              <button 
                type="submit" 
                disabled={status === 'Writing'}
                className="bg-zinc-100 text-zinc-900 font-medium rounded-lg px-4 py-2 text-sm hover:bg-white transition-colors disabled:opacity-50 mt-1"
              >
                Execute Write
              </button>
            </form>

            <hr className="border-zinc-800/60" />

            <form onSubmit={handleRead} className="flex flex-col gap-3">
              <h3 className="text-sm font-medium text-zinc-400 uppercase tracking-wider mb-2">Read (Get)</h3>
              <div className="flex gap-2">
                <input 
                  className="bg-zinc-950 border border-zinc-800 focus:border-zinc-600 rounded-lg px-4 py-2 text-sm outline-none transition-colors w-full placeholder:text-zinc-700" 
                  placeholder="Search Key..." 
                  value={readKey} 
                  onChange={(e) => setReadKey(e.target.value)}
                />
                <button 
                  type="submit" 
                  disabled={status === 'Reading'}
                  className="bg-zinc-800 text-zinc-100 border border-zinc-700 font-medium rounded-lg px-4 py-2 text-sm hover:bg-zinc-700 transition-colors disabled:opacity-50"
                >
                  Query
                </button>
              </div>
              
              <div className="mt-2 h-16 w-full rounded-lg bg-zinc-950 border border-zinc-800 flex items-center p-4 overflow-hidden">
                <span className={`font-mono text-sm truncate ${readResult === 'Not Found' ? 'text-red-400' : 'text-green-400'}`}>
                  {readResult ? `> ${readResult}` : '> _'}
                </span>
              </div>
            </form>
          </BentoItem>

          <BentoItem className="md:col-span-1">
            <MemTable items={memtableItems} />
          </BentoItem>
          
          <BentoItem className="md:col-span-1">
            <DiskStratum layout={diskLayout} />
          </BentoItem>
          
          <BentoItem className="md:col-span-3">
            {/* <-- ADDED PROPS HERE */}
            <Telemetry totalReads={totalReads} savedReads={bloomSavedReads} />
          </BentoItem>

        </BentoGrid>
      </main>
    </div>
  );
}