import React, { useEffect, useRef } from 'react';

interface SensingRadarProps {
  trackedEntitiesCount: number;
}

const SensingRadar: React.FC<SensingRadarProps> = ({ trackedEntitiesCount }) => {
  const canvasRef = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const width = canvas.width;
    const height = canvas.height;
    const centerX = width / 2;
    const centerY = height / 2;
    const maxRadius = Math.min(centerX, centerY) - 10;

    let angle = 0;
    let animationFrameId: number;

    // Generate mock static environment points (walls/furniture)
    const staticPoints = Array.from({ length: 150 }).map(() => ({
      r: Math.random() * maxRadius,
      theta: Math.random() * 2 * Math.PI,
      opacity: Math.random() * 0.5 + 0.1
    }));

    const render = () => {
      // Fade effect to create radar persistence
      ctx.fillStyle = 'rgba(2, 6, 23, 0.15)'; // slate-950 with transparency
      ctx.fillRect(0, 0, width, height);

      // Draw Grid Rings
      ctx.strokeStyle = 'rgba(16, 185, 129, 0.2)'; // emerald-500
      ctx.lineWidth = 1;
      for (let i = 1; i <= 4; i++) {
        ctx.beginPath();
        ctx.arc(centerX, centerY, maxRadius * (i / 4), 0, 2 * Math.PI);
        ctx.stroke();
      }

      // Draw Static Environment (JCAS Mapper)
      staticPoints.forEach(p => {
        const x = centerX + p.r * Math.cos(p.theta);
        const y = centerY + p.r * Math.sin(p.theta);
        ctx.fillStyle = `rgba(148, 163, 184, ${p.opacity})`; // slate-400
        ctx.beginPath();
        ctx.arc(x, y, 2, 0, 2 * Math.PI);
        ctx.fill();
      });

      // Draw Kinetic Entities (Doppler Tracking)
      for (let i = 0; i < trackedEntitiesCount; i++) {
        // Simulated movement for tracked targets
        const targetAngle = angle - (0.5 + i * 0.2);
        const targetRadius = maxRadius * 0.6 + Math.sin(Date.now() * 0.001 + i) * 20;
        const x = centerX + targetRadius * Math.cos(targetAngle);
        const y = centerY + targetRadius * Math.sin(targetAngle);

        ctx.fillStyle = '#ef4444'; // red-500
        ctx.shadowBlur = 10;
        ctx.shadowColor = '#ef4444';
        ctx.beginPath();
        ctx.arc(x, y, 4, 0, 2 * Math.PI);
        ctx.fill();
        ctx.shadowBlur = 0; // Reset
      }

      // Draw Radar Sweep Beam
      const sweepLength = maxRadius;
      ctx.beginPath();
      ctx.moveTo(centerX, centerY);
      ctx.lineTo(
        centerX + sweepLength * Math.cos(angle),
        centerY + sweepLength * Math.sin(angle)
      );
      ctx.strokeStyle = '#10b981'; // emerald-500
      ctx.lineWidth = 2;
      ctx.stroke();

      // Draw Radar Sweep Gradient (Trailing edge)
      ctx.beginPath();
      ctx.moveTo(centerX, centerY);
      ctx.arc(centerX, centerY, maxRadius, angle - 0.4, angle, false);
      ctx.lineTo(centerX, centerY);
      const gradient = ctx.createRadialGradient(centerX, centerY, 0, centerX, centerY, maxRadius);
      gradient.addColorStop(0, 'rgba(16, 185, 129, 0)');
      gradient.addColorStop(1, 'rgba(16, 185, 129, 0.4)');
      ctx.fillStyle = gradient;
      ctx.fill();

      angle += 0.03; // Sweep speed
      animationFrameId = requestAnimationFrame(render);
    };

    render();

    return () => {
      cancelAnimationFrame(animationFrameId);
    };
  }, [trackedEntitiesCount]);

  return (
    <div className="bg-slate-900 rounded-xl p-6 border border-slate-700 shadow-xl flex flex-col items-center">
      <div className="w-full flex justify-between items-center mb-4">
        <h3 className="text-emerald-400 font-mono text-sm uppercase tracking-widest">
          JCAS Spatial Radar
        </h3>
        <span className="text-slate-400 text-xs font-mono">
          {trackedEntitiesCount} Kinetic Entities Detected
        </span>
      </div>
      <div className="relative w-[300px] h-[300px] rounded-full overflow-hidden border-2 border-slate-800 bg-slate-950">
        <canvas ref={canvasRef} width={300} height={300} className="absolute top-0 left-0" />
      </div>
    </div>
  );
};

export default SensingRadar;
