import React, { useEffect, useState } from 'react';
import HolographicMap from './components/HolographicMap';
import SensingRadar from './components/SensingRadar';
import EnergyGrid from './components/EnergyGrid';
import { coreSocket } from './services/telemetrySocket';
import { parseCoreTelemetry, DashboardState } from './utils/tensorParsers';
import './index.css';

const App: React.FC = () => {
  // Initialize the dashboard state with our typed interface
  const [dashboardState, setDashboardState] = useState<DashboardState>({
    nanosecondTick: 0,
    kineticEntitiesCount: 0,
    meshPowerPercentage: 0,
    isOamActive: false,
    activeThzStreams: 1,
  });

  useEffect(() => {
    // 1. Boot the WebSocket connection to the C++ Core Engine
    coreSocket.connect();

    // 2. Subscribe to the global telemetry channel for the high-frequency tick data
    const unsubscribe = coreSocket.subscribe('GLOBAL_TICK', (rawData: string) => {
      const parsedData = parseCoreTelemetry(rawData);
      if (parsedData) {
        setDashboardState(parsedData);
      }
    });

    // Cleanup the listener when the component unmounts to prevent memory leaks
    return () => {
      unsubscribe();
    };
  }, []);

  return (
    <div className="min-h-screen bg-slate-950 text-slate-200 font-sans p-8 selection:bg-fuchsia-500/30 flex flex-col">
      <header className="mb-8 border-b border-slate-800 pb-4">
        <h1 className="text-4xl font-light tracking-tight text-white">
          <span className="font-bold text-fuchsia-500">6G AI-NATIVE</span> SENSING FABRIC
        </h1>
        <p className="text-slate-400 font-mono text-sm mt-2">
          Terahertz Holographic MIMO & JCAS Spatial Telemetry
        </p>
      </header>

      <main className="grid grid-cols-1 lg:grid-cols-3 gap-8 flex-grow">
        {/* Left Column: 3D Holographic Rendering */}
        <section className="lg:col-span-2 glass-panel rounded-2xl flex flex-col p-2">
          <div className="flex-1 min-h-[400px]">
            <HolographicMap 
              activeStreams={dashboardState.activeThzStreams} 
              isOamActive={dashboardState.isOamActive} 
            />
          </div>
        </section>

        {/* Right Column: Radar & Energy Grids */}
        <section className="flex flex-col gap-8">
          <div className="glass-panel rounded-2xl">
            <SensingRadar trackedEntitiesCount={dashboardState.kineticEntitiesCount} />
          </div>
          <div className="glass-panel rounded-2xl">
            <EnergyGrid chargePercentage={dashboardState.meshPowerPercentage} />
          </div>
        </section>
      </main>
      
      <footer className="mt-8 pt-4 border-t border-slate-800 flex justify-between text-xs text-slate-500 font-mono">
        <span>ENGINE TICK: {dashboardState.nanosecondTick}</span>
        <span>STATUS: {dashboardState.nanosecondTick > 0 ? 'SYNCHRONIZED' : 'AWAITING THz TELEMETRY...'}</span>
      </footer>
    </div>
  );
};

export default App;
