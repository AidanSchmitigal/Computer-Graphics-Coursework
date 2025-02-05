// you can set these 4 uniform variables dynamically or hardwire them:

uniform float uKa, uKd, uKs; // coefficients of each type of lighting
uniform float uShininess;    // specular exponent

// in Project #1, these have to be set dynamically from glman sliders or keytime
// animations or by keyboard hits:
uniform bool toggle;
uniform float uAd, uBd;
uniform float uTol;
uniform sampler2D uNoise2;
uniform sampler3D uNoise3;
uniform float uNoiseAmp;
uniform float uNoiseFreq;
uniform float uTime;

// interpolated from the vertex shader
varying vec2 vST; // texture coords
varying vec3 vN;  // normal vector
varying vec3 vL;  // vector from point to light
varying vec3 vE;  // vector from point to eye
varying vec3 vMC; // model coordinates

// for Mac users:
//	Leave out the #version line, or use 120
//	Change the "in" to "varying"

uniform vec3 OBJECTCOLOR; // = vec3(1.0, 1.0, 1.0);  // color to make the object
const vec3 SPECULARCOLOR = vec3(1., 1., 1.);

vec3 PerturbNormal2(float angx, float angy, vec3 n) {
  float cx = cos(angx);
  float sx = sin(angx);
  float cy = cos(angy);
  float sy = sin(angy);

  // rotate about x:
  float yp = n.y * cx - n.z * sx; // y'
  n.z = n.y * sx + n.z * cx;      // z'
  n.y = yp;
  // n.x      =  n.x;

  // rotate about y:
  float xp = n.x * cy + n.z * sy; // x'
  n.z = -n.x * sy + n.z * cy;     // z'
  n.x = xp;
  // n.y      =  n.y;

  return normalize(n);
}

void main() {
  vec3 myColor = OBJECTCOLOR;
  vec2 st = vST;

  vec3 Light = normalize(vL);
  vec3 Eye = normalize(vE);

  // myColor = texture2D(uNoise2, st).xxx * uNoiseAmp;
  // Normal += (texture3D(uNoise3, vMC * uNoiseFreq + uTime * 0.1).xyz - 0.5) *
  //           uNoiseAmp;

  vec4 nvx = texture3D(uNoise3, uNoiseFreq * vMC + uTime * 0.1);
  float angx = nvx.r + nvx.g + nvx.b + nvx.a - 2.; // -1. to +1.
  angx *= uNoiseAmp;

  vec4 nvy =
      texture3D(uNoise3, uNoiseFreq * vec3(vMC.xy, vMC.z + 0.5) + uTime * 0.1);
  float angy = nvy.r + nvy.g + nvy.b + nvy.a - 2.; // -1. to +1.
  angy *= uNoiseAmp;

  vec3 n = PerturbNormal2(angx, angy, vN);
  n = normalize(gl_NormalMatrix * n);

  vec3 Normal = normalize(n);

  vec3 ambient = uKa * myColor;

  float dd = max(dot(Normal, Light),
                 0.); // only do diffuse if the light can see the point
  vec3 diffuse = uKd * dd * myColor;

  float s = 0.;
  if (dd > 0.) // only do specular if the light can see the point
  {
    vec3 ref = normalize(reflect(-Light, Normal));
    float cosphi = dot(Eye, ref);
    if (cosphi > 0.)
      s = pow(max(cosphi, 0.), uShininess);
  }
  vec3 specular = uKs * s * SPECULARCOLOR.rgb;
  gl_FragColor = vec4(ambient + diffuse + specular, 1.);
}