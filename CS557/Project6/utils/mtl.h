#pragma once
#include <stdio.h>

class Mtl {
 public:
  char *Name;
  float Ka[3];
  float Kd[3];
  float Ke[3];
  float Ks[3];
  float D;
  float Ni;
  float Ns;
  float Illum;
  char *MapKd;
  Mtl *Next;

  Mtl();
};

class Mtls {
 public:
  FILE *Fp;
  Mtl *First;
  Mtl *Last;

  Mtls();
  // 0 = success, !0 = failure
  int Open(char *fileName);
  void ReadMtlFile();
  void Close();
  Mtl *FindMtl(char *mtlName);
};