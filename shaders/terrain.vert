#version 120

/************************************************************************
 * NOISE FUNCTIONS
 ************************************************************************/

// -- -Random Number Functions-- -
float rand(vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}
#define RANDOM_SCALE vec4(443.897, 441.423, .0973, .1099)
vec2 random2(vec2 p) {
  vec3 p3 = fract(p.xyx * RANDOM_SCALE.xyz);
  p3 += dot(p3, p3.yzx + 19.19);
  return fract((p3.xx + p3.yz) * p3.zy);
}

vec2 mod289(const in vec2 x) { return x - floor(x * (1. / 289.)) * 289.; }
vec3 mod289(const in vec3 x) { return x - floor(x * (1. / 289.)) * 289.; }
vec4 mod289(const in vec4 x) { return x - floor(x * (1. / 289.)) * 289.; }

// --- Interpolation Functions --- (Keep the existing interpolation
// functions)
// --- Value Noise ---
float valueNoise(vec2 st) {
  vec2 i = floor(st);
  vec2 f = fract(st);

  float bottom = mix(rand(i + vec2(0.0, 0.0)), rand(i + vec2(1.0, 0.0)), f.x);
  float top = mix(rand(i + vec2(0.0, 1.0)), rand(i + vec2(1.0, 1.0)), f.x);
  return mix(bottom, top, f.y);
}

// --- Linear Interpolation ---
float lerp(float a, float b, float t) { return a + t * (b - a); }

// --- Cosine Interpolation ---
float cosineInterpolate(float a, float b, float t) {
  float ft = t * 3.1415927;
  float f = (1.0 - cos(ft)) * 0.5;
  return a * (1.0 - f) + b * f;
}

// --- Smoothstep Interpolation ---
float smoothstepInterpolate(float a, float b, float t) {
  float f = t * t * (3.0 - 2.0 * t);
  return a + f * (b - a);
}

// --- Improved Perlin Noise (Value Noise Gradient) ---
// float permute(const in float v) { return mod289(((v * 34.0) + 1.0) * v); }
// vec2 permute(const in vec2 v) { return mod289(((v * 34.0) + 1.0) * v); }
vec3 permute(const in vec3 v) { return mod289(((v * 34.0) + 1.0) * v); }
vec4 permute(const in vec4 v) { return mod289(((v * 34.0) + 1.0) * v); }

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

// --- Base Noise Functions (Perlin & Simplex - keep existing) ---
vec4 taylorInvSqrt(in vec4 r) {
  return 1.79284291400159 - 0.85373472095314 * r;
}
vec2 quintic(const in vec2 v) {
  return v * v * v * (v * (v * 6.0 - 15.0) + 10.0);
}

// --- Base Noise Functions ---
float perlinNoise(in vec2 P) {
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;

  vec4 i = permute(permute(ix) + iy);

  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0;
  vec4 gy = abs(gx) - 0.5;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  vec2 g00 = vec2(gx.x, gy.x);
  vec2 g10 = vec2(gx.y, gy.y);
  vec2 g01 = vec2(gx.z, gy.z);
  vec2 g11 = vec2(gx.w, gy.w);

  vec4 norm = taylorInvSqrt(
      vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;

  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));

  vec2 fade_xy = quintic(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

// --- Simplex Noise --- (Simplified 2D Simplex Noise - adapted from various
// sources) Simplex noise contribution at 4 directions

float simplexNoise(in vec2 v) {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                      -0.577350269189626, // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
  // First corner
  vec2 i = floor(v + dot(v, C.yy));
  vec2 x0 = v - i + dot(i, C.xx);

  // Other corners
  vec2 i1;
  // i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  // i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

  // Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p =
      permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));

  vec3 m = max(
      0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
  m = m * m;
  m = m * m;

  // Gradients: 41 points uniformly over a line, mapped onto a diamond.
  // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

  // Normalise gradients implicitly by scaling m
  // Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

  // Compute final noise value at P
  vec3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

// --- Ridged Noise ---
float ridgedNoise(vec2 st, int octaves) {
  float sum = 0.0;
  float frequency = 1.0;
  float amplitude = 0.5;

  for (int i = 0; i < octaves; ++i) {
    float noiseValue;
    noiseValue = perlinNoise(st * frequency);

    noiseValue = abs(noiseValue);  // Create ridges by taking absolute value
    noiseValue = 1.0 - noiseValue; // Invert to get ridges instead of valleys
    noiseValue *= noiseValue;      // Sharpen ridges
    sum += noiseValue;
  }
  return sum; // Normalize
}

// --- Worley Noise (Cell Noise) ---
float distEuclidean(vec2 a, vec2 b) { return distance(a, b); }

vec2 worleyNoise(vec2 p, float jitter) {
  vec2 n = floor(p);
  vec2 f = fract(p);

  float distF1 = 1.0;
  float distF2 = 1.0;
  vec2 off1 = vec2(0.0);
  vec2 pos1 = vec2(0.0);
  vec2 off2 = vec2(0.0);
  vec2 pos2 = vec2(0.0);
  for (int j = -1; j <= 1; j++) {
    for (int i = -1; i <= 1; i++) {
      vec2 g = vec2(i, j);
      vec2 o = random2(n + g) * jitter;
      vec2 p = g + o;
      float d = distEuclidean(p, f);
      if (d < distF1) {
        distF2 = distF1;
        distF1 = d;
        off2 = off1;
        off1 = g;
        pos2 = pos1;
        pos1 = p;
      } else if (d < distF2) {
        distF2 = d;
        off2 = g;
        pos2 = p;
      }
    }
  }

  return vec2(distF1, distF2);
}

// --- Fractal Brownian Motion (fBm) ---
float fbm(vec2 st, int octaves) {
  float sum = 0.0;
  float frequency = 1.0;
  float maxAmplitude = 0.0;

  for (int i = 0; i < octaves; ++i) {
    float noiseValue;
    noiseValue = perlinNoise(st * frequency);

    sum += noiseValue;
  }
  return sum; // Normalize
}

// --- Turbulence Noise ---
float turbulenceNoise(vec2 st, int octaves) {
  float sum = 0.0;
  float frequency = 1.0;
  float maxAmplitude = 0.0;

  for (int i = 0; i < octaves; ++i) {
    float noiseValue;
    noiseValue = perlinNoise(st * frequency);
    sum += abs(noiseValue); // Absolute value for turbulence
  }
  return sum;
}

/************************************************************************
 * SHADER
 ************************************************************************/

uniform float uTime;
uniform float uHeightScale;
uniform float uScale;

// uniform sampler2D uTex1;
// uniform sampler2D uTex2;
// uniform sampler2D uTex3;
// uniform sampler2D uTex4;
// uniform sampler2D uTex5;

uniform struct {
  bool enabled;
  int textureOrModifierId;
  bool textureNotModifier;
  float scale;
  float shift;
  float positionScale;
  float positionShift;
  float var1; // Scale for textures | varies for modifiers
  float var2;
  float var3;
  int maskId;
  float maskVar1; // Min values for mask
  float maskVar2; // Max values for mask
} layers[5];

varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vMC;
varying vec2 vST;
varying vec4 vWC;
varying vec4 vPos;
varying float vEdge;

float getLayerHeight(int i, vec2 position, float height) {
  float scale = layers[i].scale;
  float shift = layers[i].shift;
  int texture = layers[i].textureOrModifierId;
  bool textureNotModifier = layers[i].textureNotModifier;
  float var1 = layers[i].var1;
  float var2 = layers[i].var2;
  float var3 = layers[i].var3;

  float layerValue = 0.0;

  if (textureNotModifier) {
    /************************************************************************
     * TEXTURES
     ************************************************************************/
    int texture = layers[i].textureOrModifierId;
    vec2 scalePos =
        position * layers[i].positionScale +
        vec2(layers[i].positionShift, layers[i].positionShift) * 100.;

    if (texture == 0) { // Value Noise
      layerValue = valueNoise(scalePos);
    } else if (texture == 1) { // Perlin Noise
      layerValue = perlinNoise(scalePos);
    } else if (texture == 2) { // Simplex Noise
      layerValue = simplexNoise(scalePos);
    } else if (texture == 3) { // Ridged Noise
      // var1 = octaves
      layerValue = ridgedNoise(scalePos, int(var1));
    } else if (texture == 4) { // Worley Noise
      // var1 = jitter
      layerValue = 1.0 - worleyNoise(scalePos, var1).x;
    } else if (texture == 5) { // Fbm
      layerValue = fbm(scalePos, int(var1));
    } else if (texture == 6) { // Turbulence Noise
      layerValue = turbulenceNoise(scalePos, int(var1));
    }
  } else {
    /************************************************************************
     * MODIFIERS
     ************************************************************************/
    int modifier = layers[i].textureOrModifierId;

    if (modifier == 0) { // Thermal
      // layerValue = texture2D(uTex1, position * textureScale).r;
    } else if (modifier == 1) { // Hydraulic
      // layerValue = texture2D(uTex2, position * textureScale).r;
    } else if (modifier == 2) { // Wind
      // layerValue = texture2D(uTex3, position * textureScale).r;
    } else if (modifier == 3) { // Canyon
      // layerValue = texture2D(uTex4, position * textureScale).r;
    } else if (modifier == 4) { // Terracing

      float stepSize = 0.4;
      float blendFactor = var1;
      float fractional = fract(height / stepSize);
      // when blendFactor is 0, smooth is 0.5.
      // when blendFactor is 1, smooth is 0
      float smooth = blendFactor * -0.5 + 0.5;
      fractional = smoothstep(0. + smooth, 1. - smooth, fractional);
      float terraceHeight = (floor(height / stepSize) + fractional) * stepSize;
      layerValue = mix(height, terraceHeight, blendFactor) - height;
    } else if (modifier == 5) { // Fault Line
      // layerValue = texture2D(uTex6, position * textureScale).r;
    } else if (modifier == 6) { // Mountain Ranges
      // layerValue = texture2D(uTex7, position * textureScale).r;
    } else if (modifier == 7) { // Cratered
      // layerValue = texture2D(uTex8, position * textureScale).r;
    } else if (modifier == 8) { // Cave
      // layerValue = texture2D(uTex9, position * textureScale).r;
    } else if (modifier == 9) { // Volcanoes
      // layerValue = texture2D(uTex10, position * textureScale).r;
    } else if (modifier == 10) { // Swamps
      // layerValue = texture2D(uTex11, position * textureScale).r;
    } else if (modifier == 11) { // Island Chains
      // layerValue = texture2D(uTex12, position * textureScale).r;
    } else if (modifier == 12) { // Smooth
      // layerValue = texture2D(uTex13, position * textureScale).r;
    } else if (modifier == 13) { // Clamp
      // layerValue = texture2D(uTex14, position * textureScale).r;
    } else if (modifier == 14) { // Sharp Ridge
      // layerValue = texture2D(uTex15, position * textureScale).r;
    } else if (modifier == 15) { // Flow Map
      // layerValue = texture2D(uTex16, position * textureScale).r;
    }
  }

  layerValue = layerValue; // Center
  layerValue *= scale;

  return layerValue + shift;
}

vec2 getHeightAndSlope() {
  vec2 position = gl_Vertex.xz;
  vec2 position_dx = vec2(position.x + 0.001, position.y);
  vec2 position_dy = vec2(position.x, position.y + 0.001);
  // vec3 heightSlopeNormal = getHeightSlopeNormal(position);

  float height = 0.0;
  float height_dx = 0.0;
  float height_dy = 0.0;
  float slope = 0.0;

  for (int i = 0; i < 8; i++) {
    if (layers[i].enabled == false)
      continue;
    /************************************************************************
     * MASKS
     ************************************************************************/
    int mask = layers[i].maskId;
    float maskVar1 = layers[i].maskVar1;
    float maskVar2 = layers[i].maskVar2;

    if (mask == 1) { // Height
      // maskVar1 = Min height
      // maskVar2 = Max height
      // Only modify if height is between the two
      if (height < maskVar1 && height > maskVar2)
        continue;
    } else if (mask == 2) { // Slope
      // maskVar1 = Min slope
      // maskVar2 = Max slope
      // Only modify if slope is between the two
      if (slope < maskVar1 || slope > maskVar2) {
        continue;
      }
    }

    height += getLayerHeight(i, position, height);
    height_dx += getLayerHeight(i, position_dx, height_dx);
    height_dy += getLayerHeight(i, position_dy, height_dy);

    float slope_x = (height_dx - height) / 0.001;
    float slope_y = (height - height_dy) / 0.001;
    slope = length(vec2(slope_x, slope_y));
  }

  height *= uHeightScale;
  height_dx *= uHeightScale;
  height_dy *= uHeightScale;

  float slope_x = (height_dx - height) / 0.001;
  float slope_y = (height - height_dy) / 0.001;
  slope = length(vec2(slope_x, slope_y));

  // Normal
  vNormal = cross(vec3(position.x, height, position.y) -
                      vec3(position_dy.x, height_dy, position_dy.y),
                  vec3(position.x, height, position.y) -
                      vec3(position_dx.x, height_dx, position_dx.y));
  vNormal = normalize(vNormal);

  return vec2(height, slope);
}

void main() {
  vec3 ECposition = (gl_ModelViewMatrix * gl_Vertex).xyz;
  vEyeDir = ECposition.xyz - vec3(0., 0., 0.);
  // vector from the eye position to the point
  vNormal = normalize(gl_Normal);
  vST = gl_MultiTexCoord0.st;
  vWC = gl_ModelViewMatrix * gl_Vertex;

  vec2 position = gl_Vertex.xz;

  vec2 heightSlope = getHeightAndSlope();
  float height = heightSlope.x;

  vec4 newPosition = vec4(position.x, height, position.y, 1.0);
  if (position.x <= -0.5 || position.x >= 0.5 || position.y <= -0.5 ||
      position.y >= 0.5) {
    newPosition.y = -5.0;
    vEdge = 1.0;
  } else {
    vEdge = 0.0;
  }
  vPos = newPosition;
  newPosition.x *= 10.0;
  newPosition.z *= 10.0;
  vMC = newPosition.xyz;
  gl_Position = gl_ModelViewProjectionMatrix * newPosition;
}