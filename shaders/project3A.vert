uniform float uRipplePeriod;
uniform float uRippleAmp;

// will be interpolated into the fragment shader:
varying vec2 vST; // texture coords
varying vec3 vN;  // normal vector
varying vec3 vL;  // vector from point to light
varying vec3 vE;  // vector from point to eye
varying vec3 vMC; // model coordinates

// for Mac users:
//	Leave out the #version line, or use 120
//	Change the "out" to "varying"

uniform vec3 uLightPos;
uniform float uLightDistance;

void main() {
  vST = gl_MultiTexCoord0.st;
  vMC = gl_Vertex.xyz;

  // For a mesh in the xy plane, the ripples are in the z direction
  // vec4 new_position = gl_Vertex + vec4(0.0, 0.0, sin(vST.x * uRipplePeriod) *
  // uRippleAmp * (1. - vST.y), 0.0);
  // float dzdx = uA * (Y0-vert.y) * (2.*pi/uP) * cos( 2.*pi*vert.x/uP )
  // float dzdy = -uA * sin( 2.*π*vert.x/uP )

  float Y0 = 1.;                     // y value of the center of the ripple
  float pi = 3.14159265358979323846; // pi
  vec4 new_position =
      gl_Vertex + vec4(0.0, 0.0,
                       sin(2. * pi * (vST.x * 2. - 1.) / uRipplePeriod) *
                           uRippleAmp * (1. - vST.y),
                       0.0);
  float dzdx = uRippleAmp * (Y0 - new_position.y) * (2. * pi / uRipplePeriod) *
               cos(2. * pi * new_position.x / uRipplePeriod);
  float dzdy = -uRippleAmp * sin(2. * pi * new_position.x / uRipplePeriod);

  vec3 Tx = vec3(1., 0., dzdx);
  vec3 Ty = vec3(0., 1., dzdy);
  vec3 new_normal = normalize(cross(Tx, Ty));

  vec4 ECposition = gl_ModelViewMatrix * gl_Vertex; // eye coordinate position
  vN = normalize(gl_NormalMatrix * new_normal);     // normal vector
  vL =
      (gl_ModelViewMatrix * vec4((uLightPos - 0.5) * uLightDistance, 1.0)).xyz -
      ECposition.xyz; // vector from the point to the light position
  vE = vec3(0., 0., 0.) -
       ECposition.xyz; // vector from the point to the eye position
  gl_Position = gl_ModelViewProjectionMatrix * new_position;
}