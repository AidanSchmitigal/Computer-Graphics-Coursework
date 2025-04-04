varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vMC;
varying vec2 vST;

uniform float uTime;

mat3 rotateY(float angle) {
  float c = cos(angle);
  float s = sin(angle);
  return mat3(c, 0.0, -s, 0.0, 1.0, 0.0, s, 0.0, c);
}

void main() {
  // float speed = 0.1;
  // // Apply rotation to the vertex position
  // vec4 rotatedVertex = vec4(rotateY(uTime * speed) * gl_Vertex.xyz, 1.0);

  // vMC = rotatedVertex.xyz;
  // vec3 ECposition = (gl_ModelViewMatrix * rotatedVertex).xyz;
  // vEyeDir = ECposition.xyz - vec3(0., 0., 0.);

  // // Rotate the normal as well
  // vNormal = normalize(rotateY(uTime * speed) * gl_Normal);
  // vST = gl_MultiTexCoord0.st;

  // gl_Position = gl_ModelViewProjectionMatrix * rotatedVertex;

  vMC = gl_Vertex.xyz;
  vec3 ECposition = (gl_ModelViewMatrix * gl_Vertex).xyz;
  vEyeDir = ECposition.xyz - vec3(0., 0., 0.);
  // vector from the eye position to the point
  vNormal = normalize(gl_Normal);
  vST = gl_MultiTexCoord0.st;

  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}