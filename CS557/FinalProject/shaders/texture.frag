uniform sampler2D uTex2;
uniform bool uTex2Enabled;
uniform sampler3D uTex3;
uniform bool uTex3Enabled;
uniform samplerCube uTexCube;
uniform bool uTexCubeEnabled;

varying vec3 vNormal;
varying vec3 vEyeDir;
varying vec3 vMC;
varying vec2 vST;

const vec3 WHITE = vec3(1., 1., 1.);

vec3 flipCubeDirection(vec3 dir) {
  vec3 adir = abs(dir);
  if (adir.y > adir.x && adir.y > adir.z) {
    return dir * vec3(1., 1., -1.);
  } else {
    return dir * vec3(1., -1., 1.);
  }
}

void main() {
  if (uTex2Enabled) {
    gl_FragColor = texture2D(uTex2, vST);
  } else if (uTex3Enabled) {
    gl_FragColor = texture3D(uTex3, vMC);
  } else if (uTexCubeEnabled) {
    gl_FragColor = textureCube(uTexCube, flipCubeDirection(vMC));
  } else {
    gl_FragColor = vec4(1., 0., 0., 1.);
  }
}