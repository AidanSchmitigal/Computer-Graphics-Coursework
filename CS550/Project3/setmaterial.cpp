
void SetMaterial(float r, float g, float b, float shininess) {
  glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
  glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, (float *)WHITE_ARRAY));
  glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., (float *)WHITE_ARRAY));
  glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
  glMaterialf(GL_BACK, GL_SHININESS, 2.f);

  glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
  glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
  glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
  glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, (float *)WHITE_ARRAY));
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void SetMaterialI(float r, float g, float b, float shininess) {
  SetMaterial(r / 255., g / 255., b / 255., shininess);
}