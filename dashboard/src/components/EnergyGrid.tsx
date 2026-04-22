import React from 'react';

interface EnergyGridProps {
  chargePercentage: number;
}

const EnergyGrid: React.FC<EnergyGridProps> = ({ chargePercentage }) => {
  // Clamp percentage between 0 and 100 for safety
  const safeCharge = Math.min(Math.max(chargePercentage, 0), 100);
  
  // Determine UI color state based on charge
  const isBrownout = safeCharge < 15;
  const isReady = safeCharge >= 80;
  
  const statusColor = isBrownout ? 'text-red-500' : isReady ? 'text-emerald-500' : 'text-amber-500';
  const barColor = isBrownout ? 'bg-red-500' : isReady ? 'bg-emerald-500' : 'bg-amber-500';

  return (
    <div className="bg-slate-900 rounded-xl p-6 border border-slate-700 shadow-xl flex flex-col gap-4">
      <h3 className="text-amber-400 font-mono text-sm uppercase tracking-widest border-b border-slate-800 pb-2">
        Zero-Energy IoT Mesh
      </h3>

      <div className="flex justify-between items-end">
        <div className="flex flex-col">
          <span className="text-xs text-slate-500 font-mono mb-1">Micro-Capacitor Status</span>
          <span className={`text-3xl font-bold font-mono ${statusColor}`}>
            {safeCharge.toFixed(1)}%
          </span>
        </div>
        <div className="text-right">
          <span className="text-xs text-slate-400 font-mono">
            {isBrownout ? 'CRITICAL BROWNOUT' : isReady ? 'ACTIVE THz BACKSCATTER' : 'HARVESTING RF...'}
          </span>
        </div>
      </div>

      {/* Futuristic Capacitor Bar */}
      <div className="w-full h-4 bg-slate-800 rounded-sm overflow-hidden relative border border-slate-700">
        <div 
          className={`h-full ${barColor} transition-all duration-300 ease-out`}
          style={{ width: `${safeCharge}%` }}
        />
        {/* Harvesting animation overlay */}
        {!isBrownout && (
          <div className="absolute top-0 left-0 w-full h-full bg-gradient-to-r from-transparent via-white/20 to-transparent animate-pulse" />
        )}
      </div>

      <div className="grid grid-cols-2 gap-4 mt-2">
        <div className="bg-slate-950 p-3 rounded border border-slate-800">
          <p className="text-[10px] text-slate-500 font-mono uppercase">Ambient Scavenge</p>
          <p className="text-sm text-slate-300 font-mono">300 GHz Field</p>
        </div>
        <div className="bg-slate-950 p-3 rounded border border-slate-800">
          <p className="text-[10px] text-slate-500 font-mono uppercase">Modulation</p>
          <p className="text-sm text-slate-300 font-mono">OOK Reflection</p>
        </div>
      </div>
    </div>
  );
};

export default EnergyGrid;
