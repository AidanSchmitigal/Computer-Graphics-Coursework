varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vMC;
varying vec2 vST;

void main() {
  vMC = gl_Vertex.xyz;
  vec3 ECposition = (gl_ModelViewMatrix * gl_Vertex).xyz;
  vEyeDir = ECposition.xyz - vec3(0., 0., 0.);
  // vector from the eye position to the point
  vNormal = normalize(gl_Normal);
  vST = gl_MultiTexCoord0.st;

  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}