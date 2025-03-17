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

// terrain.frag
varying vec3 vEyeDir;
varying vec3 vMC;
varying vec2 vST;
varying vec4 vWC;
varying vec4 vPos;
varying float vEdge;

// Uniforms for customization
uniform float uTime;
uniform float uHeightScale;
uniform float uScale;

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

uniform struct ColorLayer {
  vec3 color;
  float height;
  int materialType;
} colorLayers[8];

// uniform vec3 uColorGradient[8];        // Array of colors for gradient
// uniform float uGradientPercentages[8]; // Percentage thresholds (0.0-1.0)
uniform int uColorCount; // Number of colors actually used
// uniform int uMaterialTypes[8];         // Material type for each color
// segment
uniform float uSmoothness; // Blending between transitions (0.0-1.0)
uniform float uMinColorHeight;
uniform float uMaxColorHeight;

// Light properties
const vec3 uLightDir = vec3(0.0, 1.0, 1.0);
const vec3 uAmbientColor = vec3(0.3, 0.3, 0.3);

// Material properties definitions (12 different materials)
struct MaterialProps {
  float roughness;
  float metallic;
  float specularIntensity;
  float shininess;
  vec3 specularTint;
  float normalStrength;
};

MaterialProps getMaterialProperties(int materialType) {
  MaterialProps props;

  if (materialType == 1) { // Smooth Rock
    props.roughness = 0.7;
    props.metallic = 0.1;
    props.specularIntensity = 0.3;
    props.shininess = 8.0;
    props.specularTint = vec3(0.3, 0.3, 0.3);
    props.normalStrength = 1.0;
  } else if (materialType == 2) { // Rough Rock
    props.roughness = 0.9;
    props.metallic = 0.05;
    props.specularIntensity = 0.1;
    props.shininess = 4.0;
    props.specularTint = vec3(0.2, 0.2, 0.2);
    props.normalStrength = 1.5;
  } else if (materialType == 3) { // Grass
    props.roughness = 0.95;
    props.metallic = 0.0;
    props.specularIntensity = 0.05;
    props.shininess = 2.0;
    props.specularTint = vec3(0.1, 0.2, 0.1);
    props.normalStrength = 0.8;
  } else if (materialType == 4) { // Snow
    props.roughness = 0.3;
    props.metallic = 0.0;
    props.specularIntensity = 0.8;
    props.shininess = 20.0;
    props.specularTint = vec3(0.9, 0.9, 1.0);
    props.normalStrength = 0.6;
  } else if (materialType == 5) { // Ice
    props.roughness = 0.1;
    props.metallic = 0.2;
    props.specularIntensity = 0.9;
    props.shininess = 50.0;
    props.specularTint = vec3(0.8, 0.9, 1.0);
    props.normalStrength = 0.4;
  } else if (materialType == 6) { // Dirt
    props.roughness = 0.85;
    props.metallic = 0.0;
    props.specularIntensity = 0.1;
    props.shininess = 1.0;
    props.specularTint = vec3(0.3, 0.2, 0.1);
    props.normalStrength = 1.0;
  } else if (materialType == 7) { // Sand
    props.roughness = 0.8;
    props.metallic = 0.0;
    props.specularIntensity = 0.2;
    props.shininess = 3.0;
    props.specularTint = vec3(0.5, 0.4, 0.3);
    props.normalStrength = 0.7;
  } else if (materialType == 8) { // Volcanic Rock
    props.roughness = 0.75;
    props.metallic = 0.15;
    props.specularIntensity = 0.4;
    props.shininess = 12.0;
    props.specularTint = vec3(0.1, 0.1, 0.1);
    props.normalStrength = 1.2;
  } else if (materialType == 9) { // Moss
    props.roughness = 0.9;
    props.metallic = 0.0;
    props.specularIntensity = 0.05;
    props.shininess = 1.0;
    props.specularTint = vec3(0.1, 0.15, 0.1);
    props.normalStrength = 0.9;
  } else if (materialType == 10) { // Clay
    props.roughness = 0.8;
    props.metallic = 0.0;
    props.specularIntensity = 0.15;
    props.shininess = 5.0;
    props.specularTint = vec3(0.35, 0.25, 0.2);
    props.normalStrength = 0.8;
  } else if (materialType == 11) { // Slate
    props.roughness = 0.6;
    props.metallic = 0.2;
    props.specularIntensity = 0.4;
    props.shininess = 15.0;
    props.specularTint = vec3(0.25, 0.25, 0.3);
    props.normalStrength = 1.1;
  } else if (materialType == 12) { // Gravel
    props.roughness = 0.85;
    props.metallic = 0.05;
    props.specularIntensity = 0.2;
    props.shininess = 7.0;
    props.specularTint = vec3(0.3, 0.3, 0.3);
    props.normalStrength = 1.3;
  } else { // Default
    props.roughness = 0.7;
    props.metallic = 0.1;
    props.specularIntensity = 0.3;
    props.shininess = 10.0;
    props.specularTint = vec3(0.4, 0.4, 0.4);
    props.normalStrength = 1.0;
  }

  return props;
}

// Function to blend between two material properties
MaterialProps blendMaterialProperties(MaterialProps a, MaterialProps b,
                                      float t) {
  MaterialProps result;
  result.roughness = mix(a.roughness, b.roughness, t);
  result.metallic = mix(a.metallic, b.metallic, t);
  result.specularIntensity = mix(a.specularIntensity, b.specularIntensity, t);
  result.shininess = mix(a.shininess, b.shininess, t);
  result.specularTint = mix(a.specularTint, b.specularTint, t);
  result.normalStrength = mix(a.normalStrength, b.normalStrength, t);
  return result;
}

// Function to determine material based on height and slope
MaterialProps getMaterialForSurface(float height, float slope) {
  // Find which gradient segment we're in
  int lowerIndex = 0;
  int upperIndex = 1;
  float blendFactor = 0.0;

  for (int i = 0; i < 7; i++) {
    if (i < uColorCount - 1) {
      if (height >= colorLayers[i].height &&
          height <= colorLayers[i + 1].height) {
        lowerIndex = i;
        upperIndex = i + 1;

        // Calculate smooth blend factor
        float range =
            colorLayers[upperIndex].height - colorLayers[lowerIndex].height;
        if (range > 0.0001) {
          float normalizedHeight =
              (height - colorLayers[lowerIndex].height) / range;

          // Apply smoothness factor
          blendFactor = smoothstep(0.0, uSmoothness * 2.0, normalizedHeight);
          if (normalizedHeight > 1.0 - uSmoothness * 2.0) {
            blendFactor = 0.0;
          } else if (normalizedHeight < uSmoothness * 2.0) {
            blendFactor = 1.0;
          }
        }
        break;
      }
    }
  }

  // Get base materials
  MaterialProps lowerMaterial =
      getMaterialProperties(colorLayers[lowerIndex].materialType);
  MaterialProps upperMaterial =
      getMaterialProperties(colorLayers[upperIndex].materialType);

  // Adjust for slope (steeper slopes favor more rocky materials)
  float slopeInfluence = smoothstep(0.5, 0.9, slope);
  MaterialProps rockMaterial = getMaterialProperties(2); // Rough rock

  // Blend based on slope
  lowerMaterial =
      blendMaterialProperties(lowerMaterial, rockMaterial, slopeInfluence);
  upperMaterial =
      blendMaterialProperties(upperMaterial, rockMaterial, slopeInfluence);

  // Final blend between height segments
  return blendMaterialProperties(lowerMaterial, upperMaterial, blendFactor);
}

int max(int a, int b) { return a > b ? a : b; }

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

struct HeightSlopeNormal {
  float height;
  float slope;
  vec3 normal;
};

HeightSlopeNormal getHeightAndSlope() {
  vec2 position = vPos.xz;
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
  vec3 normal = cross(vec3(position.x, height, position.y) -
                          vec3(position_dy.x, height_dy, position_dy.y),
                      vec3(position.x, height, position.y) -
                          vec3(position_dx.x, height_dx, position_dx.y));
  normal = normalize(normal);

  return HeightSlopeNormal(height, slope, normal);
}

void main() {
  HeightSlopeNormal stuff = getHeightAndSlope();
  float vHeight = stuff.height;
  float vSlope = stuff.slope;
  vec3 vNormal = stuff.normal;

  vHeight =
      clamp((vHeight - uMinColorHeight) / (uMaxColorHeight - uMinColorHeight),
            0.0, 1.0);

  // Default gradient colors if not provided
  vec3 defaultGradient[8];
  defaultGradient[0] = vec3(0.0, 0.5, 0.0);    // Grass (lower)
  defaultGradient[1] = vec3(0.2, 0.6, 0.2);    // Light grass
  defaultGradient[2] = vec3(0.6, 0.5, 0.3);    // Dirt
  defaultGradient[3] = vec3(0.5, 0.5, 0.5);    // Rock
  defaultGradient[4] = vec3(0.65, 0.65, 0.65); // Light rock
  defaultGradient[5] = vec3(0.8, 0.8, 0.8);    // Stone
  defaultGradient[6] = vec3(0.9, 0.9, 0.9);    // Light snow
  defaultGradient[7] = vec3(1.0, 1.0, 1.0);    // Snow (higher)

  // Default gradient percentages if not provided
  float defaultPercentages[8];
  defaultPercentages[0] = 0.0;
  defaultPercentages[1] = 0.15;
  defaultPercentages[2] = 0.25;
  defaultPercentages[3] = 0.35;
  defaultPercentages[4] = 0.5;
  defaultPercentages[5] = 0.65;
  defaultPercentages[6] = 0.8;
  defaultPercentages[7] = 1.0;

  // Default material types if not provided
  int defaultMaterialTypes[8];
  defaultMaterialTypes[0] = 3;  // Grass
  defaultMaterialTypes[1] = 9;  // Moss
  defaultMaterialTypes[2] = 6;  // Dirt
  defaultMaterialTypes[3] = 1;  // Smooth rock
  defaultMaterialTypes[4] = 2;  // Rough rock
  defaultMaterialTypes[5] = 11; // Slate
  defaultMaterialTypes[6] = 4;  // Snow
  defaultMaterialTypes[7] = 5;  // Ice

  // Use provided values or defaults
  int colorCount = max(uColorCount, 2);

  // Calculate the color based on height and slope
  vec3 baseColor = vec3(0.5);
  float lowerIndex = 0.0;
  float upperIndex = 1.0;
  float blendFactor = 0.0;

  for (int i = 0; i < 7; i++) {
    if (i < colorCount - 1) {
      float lowerHeight =
          (uColorCount > 0) ? colorLayers[i].height : defaultPercentages[i];
      float upperHeight = (uColorCount > 0) ? colorLayers[i + 1].height
                                            : defaultPercentages[i + 1];

      if (vHeight >= lowerHeight && vHeight <= upperHeight) {
        lowerIndex = float(i);
        upperIndex = float(i + 1);

        // Calculate smooth blend factor
        float range = upperHeight - lowerHeight;
        if (range > 0.0001) {
          float normalizedHeight = (vHeight - lowerHeight) / range;
          // Apply smoothing
          blendFactor = smoothstep(0.0, uSmoothness, normalizedHeight);
          if (normalizedHeight > 1.0 - uSmoothness) {
            blendFactor = smoothstep(1.0, 1.0 - uSmoothness, normalizedHeight);
          }
        }
        break;
      }
    }
  }

  // Get colors
  vec3 lowerColor = (uColorCount > 0) ? colorLayers[int(lowerIndex)].color
                                      : defaultGradient[int(lowerIndex)];
  vec3 upperColor = (uColorCount > 0) ? colorLayers[int(upperIndex)].color
                                      : defaultGradient[int(upperIndex)];
  // vec3 lowerColor = uColorGradient[2];
  // vec3 upperColor = uColorGradient[3];

  // Blend colors
  baseColor = mix(lowerColor, upperColor, blendFactor);

  // Get material properties for current position
  MaterialProps material = getMaterialForSurface(vHeight, vSlope);

  // Add some noise to break up the transitions (would be better with a proper
  // noise texture)
  float fakeNoise = fract(sin(dot(vST, vec2(12.9898, 78.233))) * 43758.5453);
  baseColor = mix(baseColor, baseColor * (0.95 + 0.1 * fakeNoise),
                  0.2 * material.roughness);

  // Lighting calculations
  vec3 Normal = normalize(vNormal);
  vec3 Light = normalize(uLightDir);
  vec3 Eye = normalize(vEyeDir);
  vec3 HalfVector = normalize(Light + Eye);

  // Ambient lighting
  float ambientIntensity = 0.3 + 0.2 * (1.0 - material.roughness);
  vec3 ambient = ambientIntensity * baseColor;

  // Diffuse lighting
  float diffuseFactor = max(dot(Normal, Light), 0.0);
  // Add some diffuse wrap for softer lighting on rocks and terrain
  diffuseFactor =
      0.6 * diffuseFactor + 0.4 * smoothstep(-0.3, 1.0, diffuseFactor);
  vec3 diffuse = diffuseFactor * baseColor;

  // Specular lighting (using GGX-inspired approximation)
  float specularFactor = 0.0;
  if (diffuseFactor > 0.0) {
    float NdotH = max(dot(Normal, HalfVector), 0.0);
    float roughnessSq = material.roughness * material.roughness;

    // Simplified specular model suitable for terrain materials
    specularFactor = pow(NdotH, material.shininess) * (1.0 - roughnessSq);
    specularFactor *= material.specularIntensity;
  }
  vec3 specular = specularFactor * material.specularTint;

  // Fresnel effect for edges (especially nice for snow and ice)
  float fresnel = pow(1.0 - max(dot(Normal, Eye), 0.0), 4.0);
  vec3 fresnelColor =
      vec3(0.2, 0.2, 0.3) * fresnel * (1.0 - material.roughness);

  // Final color composition
  vec3 finalColor = ambient + diffuse + specular + fresnelColor;

  // Apply some ambient occlusion approximation based on height differences
  float aoFactor = 1.0 - (0.15 * smoothstep(0.4, 0.6, vSlope));
  finalColor *= aoFactor;

  // Apply a slight atmospheric fog effect with distance
  // float fogFactor = smoothstep(0.0, 0.8, vHeight);
  // vec3 fogColor = vec3(0.8, 0.85, 0.9);
  // finalColor = mix(finalColor, fogColor, fogFactor * 0.2);

  gl_FragColor = mix(vec4(finalColor, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vEdge);
  // gl_FragColor = vec4(vNormal, 1.0);
  // gl_FragColor = vec4(vHeight, 0.0, 0.0, 1.0);
}