import * as THREE from 'three';

export class HolographicRenderEngine {
  private scene: THREE.Scene;
  private camera: THREE.PerspectiveCamera;
  private renderer: THREE.WebGLRenderer;
  private animationId: number | null = null;
  private activeMeshes: Map<string, THREE.Mesh> = new Map();

  constructor(container: HTMLElement) {
    console.log('[WEBGL-ENGINE] Booting 6G Spatial Render Pipeline...');
    
    this.scene = new THREE.Scene();
    this.scene.background = new THREE.Color('#020617'); // Tailwind slate-950
    this.scene.fog = new THREE.FogExp2('#020617', 0.05);

    this.camera = new THREE.PerspectiveCamera(75, container.clientWidth / container.clientHeight, 0.1, 1000);
    this.camera.position.set(0, 5, 10);
    this.camera.lookAt(0, 0, 0);

    this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true, powerPreference: "high-performance" });
    this.renderer.setSize(container.clientWidth, container.clientHeight);
    this.renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2)); // Prevent mobile GPU overload
    
    container.appendChild(this.renderer.domElement);
  }

  public getScene(): THREE.Scene {
    return this.scene;
  }

  public registerMesh(id: string, mesh: THREE.Mesh): void {
    this.activeMeshes.set(id, mesh);
    this.scene.add(mesh);
  }

  public startRenderLoop(customAnimationLogic: () => void): void {
    const loop = () => {
      this.animationId = requestAnimationFrame(loop);
      customAnimationLogic();
      this.renderer.render(this.scene, this.camera);
    };
    loop();
  }

  public handleResize(width: number, height: number): void {
    this.camera.aspect = width / height;
    this.camera.updateProjectionMatrix();
    this.renderer.setSize(width, height);
  }

  // Critical for React unmounting to prevent WebGL context loss and memory leaks
  public dispose(): void {
    console.log('[WEBGL-ENGINE] Flushing VRAM and destroying spatial contexts...');
    if (this.animationId !== null) {
      cancelAnimationFrame(this.animationId);
    }
    
    // Deep clone destruction for Three.js
    this.activeMeshes.forEach((mesh) => {
      mesh.geometry.dispose();
      if (Array.isArray(mesh.material)) {
        mesh.material.forEach((m) => m.dispose());
      } else {
        mesh.material.dispose();
      }
      this.scene.remove(mesh);
    });

    this.activeMeshes.clear();
    this.renderer.dispose();
    this.renderer.domElement.remove();
  }
}
