uniform sampler2D uTex2;
uniform float uSc;
uniform float uTc;
uniform float uRad;
uniform float uMag;
uniform float uWhirl;
uniform float uMosaic;
uniform bool enableMosaic;

varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vMC;
varying vec2 vST;

const vec3 WHITE = vec3(1., 1., 1.);

void main() {
  float dx = vST.x - uSc;
  float dy = vST.y - uTc;

  float dist = sqrt(dx * dx + dy * dy);

  float angle = atan(dy, dx);

  if (dist > uRad) {
    gl_FragColor = texture2D(uTex2, vST);
    return;
  }

  vec2 st = vec2(vST);
  // Magnify
  st = vec2((st.x - uSc) * uMag + uSc, (st.y - uTc) * uMag + uTc);

  // Whirl
  st += vec2(cos(angle - uWhirl * dist), sin(angle - uWhirl * dist)) * dist;

  // Mosaic
  if (enableMosaic) {
    st = vec2(floor((st.x - uSc) * uMosaic + uSc) / uMosaic + uSc,
              floor((st.y - uTc) * uMosaic + uTc) / uMosaic + uTc);
  }

  gl_FragColor = texture2D(uTex2, st);
}