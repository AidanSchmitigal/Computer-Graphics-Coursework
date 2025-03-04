uniform vec2 uResolution;
uniform float uTime;
uniform float uTimeLoop;
uniform float uFirePos;
uniform sampler2D noiseTex;
uniform bool uOnFire;
uniform bool uStars;
uniform bool uMakeRainbow;
uniform vec3 uBorderColor;
uniform vec3 uCenterColor;

varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vMC;
varying vec2 vST;
varying vec4 vWC;

const vec3 WHITE = vec3(1., 1., 1.);

vec2 hash2(vec2 p) {
  // texture based white noise
  // return textureLod(iChannel0, (p + 0.5) / 256.0, 0.0).xy;

  // procedural white noise
  return fract(
      sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) *
      43758.5453);
}

vec3 voronoi(in vec2 x) {
  vec2 ip = floor(x);
  vec2 fp = fract(x);

  //----------------------------------
  // first pass: regular voronoi
  //----------------------------------
  vec2 mg, mr;

  float md = 8.0;
  for (int j = -1; j <= 1; j++)
    for (int i = -1; i <= 1; i++) {
      vec2 g = vec2(float(i), float(j));
      vec2 o = hash2(ip + g);
      o = 0.5 + 0.5 * sin(uTime + 6.2831 * o);
      vec2 r = g + o - fp;
      float d = dot(r, r);

      if (d < md) {
        md = d;
        mr = r;
        mg = g;
      }
    }

  //----------------------------------
  // second pass: distance to borders
  //----------------------------------
  md = 8.0;
  for (int j = -2; j <= 2; j++)
    for (int i = -2; i <= 2; i++) {
      vec2 g = mg + vec2(float(i), float(j));
      vec2 o = hash2(ip + g);
      o = 0.5 + 0.5 * sin(uTime + 6.2831 * o);
      vec2 r = g + o - fp;

      if (dot(mr - r, mr - r) > 0.00001)
        md = min(md, dot(0.5 * (mr + r), normalize(r - mr)));
    }

  return vec3(md, mr);
}

vec3 HueShift(in vec3 Color, in float Shift) {
  vec3 P = vec3(0.55735) * dot(vec3(0.55735), Color);

  vec3 U = Color - P;

  vec3 V = cross(vec3(0.55735), U);

  Color = U * cos(Shift * 6.2832) + V * sin(Shift * 6.2832) + P;

  return vec3(Color);
}

void main() {
  vec2 screenPos = vec2(atan((vMC.y - 0.4), vMC.z), vMC.x);
  float alongSnake = vMC.x / 18.0 + 0.5;

  vec3 c = voronoi(screenPos * 2.0);

  vec3 col = (0.4 + c.x) * uCenterColor;

  if (uMakeRainbow)
    col = HueShift(col, vMC.x * 0.1 + uTime * 0.1 + c.x * 0.9);

  col = mix(uBorderColor, col, smoothstep(0.0, 0.05, c.x));
  // gl_FragColor = vec4(col, 1.0);
  // gl_FragColor = vec4(vNormal, 1.0);

  screenPos *= 0.25;

  // // Create flowing iridescent base
  vec2 uv = screenPos * 8.0 + vec2(uTime * 0.1, sin(screenPos.x * 10.0) * 0.2);

  // Add stars
  vec3 noise = texture2D(noiseTex, uv * 0.2 + uTime * 0.1).rgb;

  float stars = step(0.53, noise.g) * smoothstep(0.0, 1.0, alongSnake);
  if (!uStars)
    stars = 0.0;

  noise = texture2D(noiseTex, uv * 0.08 + uTime * 0.2).rgb;

  float fireFactor = step(uFirePos, noise.b + alongSnake / 2.0);
  vec3 fire = vec3(1.0, 0.2, 0.5) * fireFactor;
  if (!uOnFire)
    fire = vec3(0.0, 0.0, 0.0);

  // Final color with rim lighting
  // float rim = pow(1.0 - max(0.0, dot(vNormal, vec3(0.0,
  // 0.0, 1.0))), 3.0);
  gl_FragColor = vec4(col + stars + fire, 1.0) * 2.0;

  // gl_FragColor = vec4(stars, stars, stars, 1.0);
}