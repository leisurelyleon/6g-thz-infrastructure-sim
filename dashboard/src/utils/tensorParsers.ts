// Define the expected strict JSON schema from the C++ core engine
export interface RawCoreTelemetry {
  tick: number;
  tracked_entities: number;
  iot_charge_pct: number;
}

// Define the mapped frontend state required by the React components
export interface DashboardState {
  nanosecondTick: number;
  kineticEntitiesCount: number;
  meshPowerPercentage: number;
  isOamActive: boolean;
  activeThzStreams: number;
}

/**
 * Parses and sanitizes the high-frequency telemetry payload from the 6G Core.
 */
export const parseCoreTelemetry = (rawPayload: string): DashboardState | null => {
  try {
    const parsed: Partial<RawCoreTelemetry> = JSON.parse(rawPayload);

    // Strict Type Guarding to prevent React from crashing on malformed C++ output
    if (
      typeof parsed.tick !== 'number' ||
      typeof parsed.tracked_entities !== 'number' ||
      typeof parsed.iot_charge_pct !== 'number'
    ) {
      console.warn('[TENSOR-PARSER] Invalid signature detected. Dropping corrupted frame.');
      return null;
    }

    // Dynamic Inference: Determine OAM state based on entity density
    // In a full build, the C++ core would explicitly send this flag.
    const calculatedOamState = parsed.tracked_entities > 2;
    const dynamicStreams = calculatedOamState ? parsed.tracked_entities : 1;

    // Transform snake_case to camelCase for standard TS compliance
    return {
      nanosecondTick: parsed.tick,
      kineticEntitiesCount: parsed.tracked_entities,
      meshPowerPercentage: parsed.iot_charge_pct,
      isOamActive: calculatedOamState,
      activeThzStreams: Math.min(dynamicStreams, 8) // Cap visualization at 8 beams
    };

  } catch (error) {
    console.error('[TENSOR-PARSER] Deserialization failure. Raw string:', rawPayload);
    return null;
  }
};

/**
 * Utility function to normalize the unpredictable nature of RF noise.
 * Useful for smoothing out rapid jitter in the UI displays.
 */
export const calculateExponentialMovingAverage = (
  currentValue: number, 
  previousEma: number, 
  alpha: number = 0.2
): number => {
  return (currentValue * alpha) + (previousEma * (1 - alpha));
};
