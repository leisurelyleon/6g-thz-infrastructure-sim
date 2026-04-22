import React, { useEffect, useRef } from 'react';
import * as THREE from 'three';

interface HolographicMapProps {
  activeStreams: number;
  isOamActive: boolean;
}

const HolographicMap: React.FC<HolographicMapProps> = ({ activeStreams, isOamActive }) => {
  const mountRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (!mountRef.current) return;

    // 1. Initialize 3D Scene, Camera, and WebGL Renderer
    const scene = new THREE.Scene();
    scene.background = new THREE.Color('#020617'); // Tailwind slate-950
    scene.fog = new THREE.FogExp2('#020617', 0.05);

    const camera = new THREE.PerspectiveCamera(75, mountRef.current.clientWidth / 400, 0.1, 1000);
    camera.position.set(0, 5, 10);
    camera.lookAt(0, 0, 0);

    const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
    renderer.setSize(mountRef.current.clientWidth, 400);
    mountRef.current.appendChild(renderer.domElement);

    // 2. Build the Holographic Metasurface (The 6G Antenna)
    const surfaceGeometry = new THREE.PlaneGeometry(8, 8, 32, 32);
    const surfaceMaterial = new THREE.MeshBasicMaterial({ 
      color: 0x0ea5e9, 
      wireframe: true,
      transparent: true,
      opacity: 0.3 
    });
    const metasurface = new THREE.Mesh(surfaceGeometry, surfaceMaterial);
    metasurface.rotation.x = -Math.PI / 2;
    scene.add(metasurface);

    // 3. Generate THz Beams (OAM Vortex Simulation)
    const beams: THREE.Mesh[] = [];
    
    for (let i = 0; i < activeStreams; i++) {
      // If OAM is active, use a twisted torus knot; otherwise, a straight cylinder
      const geometry = isOamActive 
        ? new THREE.TorusKnotGeometry(0.5, 0.1, 100, 16, 2, 3) 
        : new THREE.CylinderGeometry(0.1, 0.1, 8, 32);
        
      const material = new THREE.MeshBasicMaterial({ 
        color: i % 2 === 0 ? 0x8b5cf6 : 0xec4899, // Violet or Pink 
        wireframe: isOamActive,
        transparent: true,
        opacity: 0.8
      });
      
      const beam = new THREE.Mesh(geometry, material);
      beam.position.set((i - 1) * 2, 4, 0);
      scene.add(beam);
      beams.push(beam);
    }

    // 4. Animation Loop
    let animationFrameId: number;
    const animate = () => {
      animationFrameId = requestAnimationFrame(animate);
      
      // Simulate the metasurface phase shifting
      const time = Date.now() * 0.001;
      const positionAttribute = surfaceGeometry.attributes.position;
      for (let i = 0; i < positionAttribute.count; i++) {
        const x = positionAttribute.getX(i);
        const y = positionAttribute.getY(i);
        const z = Math.sin(x * 2 + time) * Math.cos(y * 2 + time) * 0.5;
        positionAttribute.setZ(i, z);
      }
      positionAttribute.needsUpdate = true;

      // Rotate the THz beams to simulate twisted wave propagation
      beams.forEach((beam, index) => {
        beam.rotation.y += 0.02 * (index + 1);
        beam.rotation.z += 0.01;
      });

      renderer.render(scene, camera);
    };
    animate();

    // 5. Cleanup on unmount
    return () => {
      cancelAnimationFrame(animationFrameId);
      renderer.dispose();
      if (mountRef.current) {
        mountRef.current.removeChild(renderer.domElement);
      }
    };
  }, [activeStreams, isOamActive]);

  return (
    <div className="bg-slate-900 rounded-xl p-6 border border-slate-700 shadow-2xl relative overflow-hidden">
      <h3 className="text-cyan-400 font-mono text-sm mb-4 uppercase tracking-widest z-10 relative">
        Continuous Aperture Holographic THz Array
      </h3>
      <div ref={mountRef} className="w-full h-[400px] rounded-lg overflow-hidden bg-slate-950" />
      
      {isOamActive && (
        <div className="absolute top-6 right-6 bg-fuchsia-900/50 border border-fuchsia-500 text-fuchsia-300 px-3 py-1 rounded-full font-mono text-xs animate-pulse">
          OAM VORTEX MULTIPLEXING ACTIVE
        </div>
      )}
    </div>
  );
};

export default HolographicMap;
