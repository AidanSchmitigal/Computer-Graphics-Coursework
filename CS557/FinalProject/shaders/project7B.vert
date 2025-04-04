// #version 120 core

// out variables to be interpolated in the rasterizer and sent to each fragment

uniform float twistAroundX;
uniform float twistAroundY;
uniform float twistAroundZ;
uniform float twistScale;
uniform bool changeWithTime;
uniform float uTimeLoop;

// shader:
varying vec3 vN;  // normal vector
varying vec3 vL;  // vector from point to light
varying vec3 vE;  // vector from point to eye
varying vec2 vST; // (s,t) texture coordinates

// where the light is:

const vec3 LightPosition = vec3(0., 5., 5.);

void main() {
  vST = gl_MultiTexCoord0.st;
  vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;
  vN = normalize(gl_NormalMatrix * gl_Normal); // normal vector
  vL = LightPosition - ECposition.xyz;         // vector from the point
                                               // to the light position
  vE = vec3(0., 0., 0.) - ECposition.xyz;      // vector from the point
                                               // to the eye position

  vec4 new_vertex = gl_Vertex;

  float radius =
      sqrt(new_vertex.y * new_vertex.y + new_vertex.z * new_vertex.z +
           new_vertex.x * new_vertex.x) *
      twistScale;

  float time = 0.0;
  if (changeWithTime) {
    if (uTimeLoop < 0.5) {
      time = smoothstep(0.0, 1.0, uTimeLoop * 2.0);
    } else {
      time = smoothstep(1.0, 0.0, (uTimeLoop - 0.5) * 2.0);
    }
    time *= 10.0;
  }

  float sinX = sin((twistAroundX + time) * radius);
  float cosX = cos((twistAroundX + time) * radius);
  float sinY = sin((twistAroundY + time) * radius);
  float cosY = cos((twistAroundY + time) * radius);
  float sinZ = sin((twistAroundZ + time) * radius);
  float cosZ = cos((twistAroundZ + time) * radius);

  // Twist around X:
  float yPos = new_vertex.y;
  new_vertex.y = new_vertex.y * cosX - new_vertex.z * sinX;
  new_vertex.z = yPos * sinX + new_vertex.z * cosX;

  // Twist around Y:
  float xPos = new_vertex.x;
  new_vertex.x = new_vertex.x * cosY - new_vertex.z * sinY;
  new_vertex.z = new_vertex.z * cosY + xPos * sinY;

  // Twist around Z:
  xPos = new_vertex.x;
  new_vertex.x = new_vertex.x * cosZ - new_vertex.y * sinZ;
  new_vertex.y = xPos * sinZ + new_vertex.y * cosZ;

  gl_Position = gl_ModelViewProjectionMatrix * new_vertex;
}