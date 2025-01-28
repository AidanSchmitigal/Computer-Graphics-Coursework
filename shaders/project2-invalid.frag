// you can set these 4 uniform variables dynamically or hardwire them:

uniform float uKa, uKd, uKs; // coefficients of each type of lighting
uniform float uShininess;    // specular exponent

// in Project #1, these have to be set dynamically from glman sliders or keytime
// animations or by keyboard hits:
uniform bool uUseNoise;
uniform float uAd, uBd;
uniform float uTol;
uniform sampler3D uNoise3;
uniform sampler2D uNoise2;
uniform float uNoiseAmp;
uniform float uNoiseFreq;

// interpolated from the vertex shader
varying vec2 vST; // texture coords
varying vec3 vN;  // normal vector
varying vec3 vL;  // vector from point to light
varying vec3 vE;  // vector from point to eye
varying vec3 vMC; // model coordinates

// for Mac users:
//	Leave out the #version line, or use 120
//	Change the "in" to "varying"

const vec3 OBJECTCOLOR = vec3(1.0, 1.0, 1.0);  // color to make the object
const vec3 ELLIPSECOLOR = vec3(1.0, 0.0, 1.0); // color to make the ellipse
const vec3 SPECULARCOLOR = vec3(1., 1., 1.);

void main() {
  vec3 myColor = OBJECTCOLOR;
  vec2 st = vST;

  vec4 nv = vec4(0.0, 0.0, 0.0, 0.0);

  // =================
  // Error "Program Invalid" happens when using 2 textures
  // =================

  if (uUseNoise == true) {
    nv = texture3D(uNoise3, uNoiseFreq * vMC) * uNoiseAmp;
  } else {
    nv = texture3D(uNoise3, uNoiseFreq * vMC) * uNoiseAmp;
    // nv = texture2D(uNoise2, st * uNoiseFreq) * uNoiseAmp;
  }

  // =================
  // It also has the error outside of the if statement
  // =================
  /*
    nv = texture3D(uNoise3, uNoiseFreq * vMC) * uNoiseAmp;
    nv = texture2D(uNoise2, st * uNoiseFreq) * uNoiseAmp;
  */

  float n = nv.r + nv.g + nv.b + nv.a; // range is 1. -> 3.
  n = n - 2.;                          // range is now -1. -> 1.
  n *= uNoiseAmp;

  // blend OBJECTCOLOR and ELLIPSECOLOR by using the ellipse equation to decide
  // how close 	this fragment is to the ellipse border:

  // (s-sc)^2 / Ar^2 + (t-tc)^2 / Br^2 = 1

  float sc = fract((st.s) / uAd);
  float tc = fract((st.t) / uBd);

  float Ar = 0.5;
  float Br = 0.5;
  // float circleDist =
  //     (sc - 0.5) * (sc - 0.5) / (Ar * Ar) + (tc - 0.5) * (tc - 0.5) / (Br *
  //     Br);

  // float t = smoothstep(1. - uTol, 1. + uTol, circleDist);
  // myColor = mix(ELLIPSECOLOR, OBJECTCOLOR, t);

  float ds = st.s - sc; // wrt ellipse center
  float dt = st.t - tc; // wrt ellipse center
  float oldDist = sqrt(ds * ds + dt * dt);
  float newDist = oldDist + n;
  float scale = newDist / oldDist; // this could be < 1., = 1., or > 1.
  ds *= scale;                     // scale by noise factor
  ds /= Ar;                        // ellipse equation
  dt *= scale;                     // scale by noise factor
  dt /= Br;                        // ellipse equation
  float d = ds * ds + dt * dt;
  float t = smoothstep(1. - uTol, 1. + uTol, d);
  myColor = mix(OBJECTCOLOR, ELLIPSECOLOR, t);

  // now use myColor in the per-fragment lighting equations:

  vec3 Normal = normalize(vN);
  vec3 Light = normalize(vL);
  vec3 Eye = normalize(vE);

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