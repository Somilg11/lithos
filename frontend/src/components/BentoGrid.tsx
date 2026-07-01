import React from 'react';

export function BentoGrid({ children, className = '' }: { children: React.ReactNode, className?: string }) {
  return (
    <div className={`grid grid-cols-1 md:grid-cols-3 gap-4 ${className}`}>
      {children}
    </div>
  );
}

export function BentoItem({ children, className = '' }: { children: React.ReactNode, className?: string }) {
  return (
    <div className={`bg-zinc-900/40 backdrop-blur-xl border border-zinc-800/80 rounded-2xl p-6 shadow-2xl flex flex-col ${className}`}>
      {children}
    </div>
  );
}