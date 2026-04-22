type TelemetryCallback = (data: any) => void;

class SixGTelemetrySocket {
  private socket: WebSocket | null = null;
  private url: string;
  private subscribers: Map<string, Set<TelemetryCallback>> = new Map();
  private reconnectAttempts = 0;
  private readonly MAX_RECONNECTS = 10;
  private isConnecting = false;

  constructor(endpoint: string) {
    this.url = endpoint;
  }

  public connect(): void {
    if (this.socket?.readyState === WebSocket.OPEN || this.isConnecting) return;

    this.isConnecting = true;
    console.log(`[6G-SOCKET] Initializing Quantum/THz Telemetry Link to ${this.url}...`);
    this.socket = new WebSocket(this.url);

    this.socket.onopen = () => {
      console.log('[6G-SOCKET] Link Established. C++ Core Engine Synchronized.');
      this.reconnectAttempts = 0;
      this.isConnecting = false;
    };

    this.socket.onmessage = (event) => {
      this.routeTelemetry(event.data);
    };

    this.socket.onclose = () => {
      console.warn('[6G-SOCKET] Core Engine Link Severed. Initiating fallback protocols.');
      this.socket = null;
      this.isConnecting = false;
      this.attemptReconnect();
    };

    this.socket.onerror = (error) => {
      console.error('[6G-SOCKET] Sub-THz Link Interference Detected:', error);
      this.socket?.close();
    };
  }

  // Pub/Sub system tailored for specific 6G data streams
  public subscribe(channel: string, callback: TelemetryCallback): () => void {
    if (!this.subscribers.has(channel)) {
      this.subscribers.set(channel, new Set());
    }
    this.subscribers.get(channel)!.add(callback);
    
    // Return unsubscribe closure
    return () => {
      this.subscribers.get(channel)?.delete(callback);
    };
  }

  private routeTelemetry(rawData: string): void {
    // In a full implementation, this routes specific JSON keys to specific channels.
    // For this dashboard, we broadcast everything to the 'GLOBAL_TICK' channel.
    const globalSubscribers = this.subscribers.get('GLOBAL_TICK');
    if (globalSubscribers) {
      globalSubscribers.forEach((cb) => cb(rawData));
    }
  }

  private attemptReconnect(): void {
    if (this.reconnectAttempts >= this.MAX_RECONNECTS) {
      console.error('[6G-SOCKET] FATAL: Max reconnection attempts exceeded. Core is unresponsive.');
      return;
    }

    this.reconnectAttempts++;
    const backoffTime = Math.min(500 * Math.pow(1.5, this.reconnectAttempts), 5000);
    
    console.log(`[6G-SOCKET] Retrying connection (${this.reconnectAttempts}/${this.MAX_RECONNECTS}) in ${backoffTime}ms...`);
    setTimeout(() => this.connect(), backoffTime);
  }

  public transmitIntent(commandPayload: object): void {
    if (this.socket?.readyState === WebSocket.OPEN) {
      this.socket.send(JSON.stringify(commandPayload));
    } else {
      console.warn('[6G-SOCKET] Cannot transmit intent: Link is down.');
    }
  }
}

// Singleton export for the React lifecycle
export const coreSocket = new SixGTelemetrySocket('ws://localhost:8080');
