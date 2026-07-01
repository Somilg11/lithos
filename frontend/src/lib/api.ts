const API_BASE = 'http://localhost:8080';

export interface GetResponse { value: string | null; }
export interface SetResponse { success: boolean; }

export interface MetricsResponse {
  memtable_size: number;
  total_reads: number;
  total_writes: number;
  bloom_saved_reads: number; // <-- ADDED THIS LINE
  disk_layout: { level: number; file_count: number }[];
}

export async function get(key: string): Promise<GetResponse> {
  const response = await fetch(`${API_BASE}/get?key=${key}`);
  if (!response.ok) throw new Error('Failed to get');
  return response.json();
}

export async function set(key: string, value: string): Promise<SetResponse> {
  const response = await fetch(`${API_BASE}/set`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ key, value }),
  });
  if (!response.ok) throw new Error('Failed to set');
  return { success: (await response.text()) === "OK" };
}

export async function metrics(): Promise<MetricsResponse> {
  const response = await fetch(`${API_BASE}/metrics`);
  if (!response.ok) throw new Error('Failed to fetch metrics');
  return response.json();
}