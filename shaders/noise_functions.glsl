// noise_functions.glsl
#ifndef NOISE_FUNCTIONS_GLSL
#define NOISE_FUNCTIONS_GLSL

// --- Random Number Functions --- (Keep the existing rand function)
float rand(vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// --- Interpolation Functions --- (Keep the existing interpolation functions)
float lerp(float a, float b, float t) { return a + t * (b - a); }
float cosineInterpolate(float a, float b, float t) {
  float ft = t * 3.1415927;
  float f = (1.0 - cos(ft)) * 0.5;
  return a * (1.0 - f) + b * f;
}
float smoothstepInterpolate(float a, float b, float t) {
  float f = t * t * (3.0 - 2.0 * t);
  return a + f * (b - a);
}

// --- Permutation and Gradient for Perlin/Simplex --- (Keep existing)
vec4 permute(vec4 t) { return mod(((t * 34.0) + 1.0) * t, 289.0); }
float grad(float hash, vec2 p) {
  vec4 h = vec4(hash, hash + 1.0, hash + 2.0, hash + 3.0);
  h = mod(h, 4.0);
  vec2 g = vec2(0.0);
  if (h.x < 1.0)
    g.xy = vec2(1.0, 1.0);
  else if (h.x < 2.0)
    g.xy = vec2(-1.0, 1.0);
  else if (h.x < 3.0)
    g.xy = vec2(-1.0, -1.0);
  else
    g.xy = vec2(1.0, -1.0);
  return dot(g, p);
}
vec3 simplex_grad[12] = vec3[12]( // Keep existing simplex_grad array
    vec3(1, 1, 0), vec3(-1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, 1, -1), vec3(0, -1, -1));

// --- Base Noise Functions (Perlin & Simplex - keep existing) ---
float perlinNoise(vec2 p) { /* ... (Existing Perlin Noise code) ... */ }
float simplexNoise(vec2 st) { /* ... (Existing Simplex Noise code) ... */ }

// --- Ridged Noise ---
float ridgedNoise(vec2 st, float lacunarity, float gain, int octaves,
                  int noiseType) {
  float sum = 0.0;
  float frequency = 1.0;
  float amplitude = 0.5;
  float maxAmplitude = 0.0;

  for (int i = 0; i < octaves; ++i) {
    float noiseValue;
    if (noiseType == 0) {
      noiseValue = perlinNoise(st * frequency);
    } else {
      noiseValue = simplexNoise(st * frequency);
    }
    noiseValue = abs(noiseValue);  // Create ridges by taking absolute value
    noiseValue = 1.0 - noiseValue; // Invert to get ridges instead of valleys
    noiseValue *= noiseValue;      // Sharpen ridges
    sum += noiseValue * amplitude;

    maxAmplitude += amplitude;
    amplitude *= gain;
    frequency *= lacunarity;
  }
  return sum / maxAmplitude; // Normalize
}

// --- Worley Noise (Cell Noise) ---
float worleyNoise(vec2 st) {
  float minDist = 1.0;
  for (int y = -1; y <= 1; ++y) {
    for (int x = -1; x <= 1; ++x) {
      vec2 neighborCell = floor(st) + vec2(x, y);
      vec2 randomPointInCell =
          rand(neighborCell) *
          vec2(1.0, 1.0); // Random point in the cell (simplified, not uniform)
      vec2 pointPos = neighborCell + randomPointInCell;
      minDist = min(minDist, distance(st, pointPos));
    }
  }
  return minDist; // Distance to closest cell point
}

// --- Fractal Brownian Motion (fBm) ---
float fbm(vec2 st, int octaves, float lacunarity, float gain, float amplitude,
          int noiseType) {
  float sum = 0.0;
  float frequency = 1.0;
  float maxAmplitude = 0.0;

  for (int i = 0; i < octaves; ++i) {
    float noiseValue;
    if (noiseType == 0) {
      noiseValue = perlinNoise(st * frequency);
    } else {
      noiseValue = simplexNoise(st * frequency);
    }
    sum += noiseValue * amplitude;

    maxAmplitude += amplitude;
    amplitude *= gain;
    frequency *= lacunarity;
  }
  return sum / maxAmplitude; // Normalize
}

// --- Domain Warped Noise ---
float domainWarpedNoise(vec2 st, float warpFactor, int warpOctaves,
                        float warpLacunarity, float warpGain, int noiseOctaves,
                        float noiseLacunarity, float noiseGain,
                        float noiseAmplitude, int warpNoiseType,
                        int finalNoiseType) {
  vec2 warpedSt = st;
  float warpFrequency = 1.0;
  float warpAmplitude = warpFactor;
  float maxWarpAmplitude = 0.0;

  for (int i = 0; i < warpOctaves; ++i) {
    float warpNoiseValue;
    if (warpNoiseType == 0) {
      warpNoiseValue = perlinNoise(warpedSt * warpFrequency);
    } else {
      warpNoiseValue = simplexNoise(warpedSt * warpFrequency);
    }
    warpedSt += warpNoiseValue * warpAmplitude;

    maxWarpAmplitude += warpAmplitude;
    warpAmplitude *= warpGain;
    warpFrequency *= warpLacunarity;
  }

  float finalNoise;
  if (finalNoiseType == 0) {
    finalNoise = fbm(warpedSt, noiseOctaves, noiseLacunarity, noiseGain,
                     noiseAmplitude, 0); // Perlin fBm
  } else {
    finalNoise = fbm(warpedSt, noiseOctaves, noiseLacunarity, noiseGain,
                     noiseAmplitude, 1); // Simplex fBm
  }
  return finalNoise;
}

// --- Turbulence Noise ---
float turbulenceNoise(vec2 st, int octaves, float lacunarity, float gain,
                      float amplitude, int noiseType) {
  float sum = 0.0;
  float frequency = 1.0;
  float maxAmplitude = 0.0;

  for (int i = 0; i < octaves; ++i) {
    float noiseValue;
    if (noiseType == 0) {
      noiseValue = perlinNoise(st * frequency);
    } else {
      noiseValue = simplexNoise(st * frequency);
    }
    sum += abs(noiseValue) * amplitude; // Absolute value for turbulence

    maxAmplitude += amplitude;
    amplitude *= gain;
    frequency *= lacunarity;
  }
  return sum / maxAmplitude; // Normalize
}

#endif // NOISE_FUNCTIONS_GLSL