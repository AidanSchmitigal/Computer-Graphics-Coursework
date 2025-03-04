uniform float uTime;
uniform float uRadius;
uniform float uBumbScale;
uniform float uHeightScale;
uniform int uWiggleStep;

varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vMC;
varying vec2 vST;
varying vec4 vWC;

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

void main() {
  vMC = gl_Vertex.xyz;
  vec3 ECposition = (gl_ModelViewMatrix * gl_Vertex).xyz;
  vEyeDir = ECposition.xyz - vec3(0., 0., 0.);
  // vector from the eye position to the point
  vNormal = normalize(gl_Normal);
  vST = gl_MultiTexCoord0.st;
  vWC = gl_ModelViewMatrix * gl_Vertex;

  vec2 screenPos = vec2(atan((vMC.y - 0.4), vMC.z), vMC.x);
  vec3 c = voronoi(screenPos * 2.0);
  float alongSnake = vMC.x / 18.0 + 0.5;

  vec4 newPosition = gl_Vertex;
  newPosition += vec4(vNormal, 0.0) * c.x * uBumbScale * alongSnake;

  float len = newPosition.x / uRadius + uTime;
  vec2 arcPos = vec2(cos(len) * uRadius, sin(len) * uRadius);

  // [ x = 0, y = z ] =
  // [ x cos t - y sin t, x sin t + y cos t ] =
  // [ z sin t, z cos t ]

  float z = newPosition.z;
  len += 3.14 * 0.5;
  newPosition.z = z * cos(len);
  newPosition.x = z * -sin(len);

  newPosition.x += arcPos.x;
  newPosition.z += arcPos.y;
  newPosition.y += sin(uTime + len * 2.0 * float(uWiggleStep)) * uHeightScale;

  gl_Position = gl_ModelViewProjectionMatrix * newPosition;
}