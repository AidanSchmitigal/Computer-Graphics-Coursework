#include "mtl.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *DELIMS = " \t";

int Readline(FILE *fp, char *line) {
  char *cp = line;
  int c;
  int numChars;

  while ((c = fgetc(fp)) != '\n' && c != EOF) {
    if (c == '\r') {
      continue;
    }
    // fprintf( stderr, "c = '%c' (0x%02x)\n", c, c );
    *cp++ = c;
  }
  *cp = '\0';

  // reject an empty line:

  numChars = 0;
  char cc;
  for (char *cp = line; (cc = *cp) != '\0'; cp++) {
    if (cc != '\t' && cc != ' ')
      numChars++;
  }
  if (numChars == 0) {
    line[0] = '\0';
  }

  return c;  // the character that caused the while-loop to terminate
}

Mtl::Mtl() {
  Next = (Mtl *)nullptr;
};

Mtls::Mtls() {
  Fp = (FILE *)nullptr;
  First = Last = (Mtl *)nullptr;
};

// 0 = success, !0 = failure
int Mtls::Open(char *fileName) {
  Fp = fopen(fileName, "r");
  if (Fp == nullptr) {
    fprintf(stderr, "Cannot open file '%s'\n", fileName);
    return 1;
  }

  return 0;
};

void Mtls::ReadMtlFile() {
  char line[256];
  Mtl *thisMtl = (Mtl *)nullptr;
  int c;
  while ((c = Readline(Fp, line)) != EOF) {
    if (c == EOF)
      break;

    if (line[0] == '\0')
      continue;

    // fprintf( stderr, "line = '%s'\n", line );

    char *tok = strtok(line, DELIMS);  // first token is the variable name
    char cc;
    for (char *cp = tok; (cc = *cp) != '\0'; cp++) {
      if (isupper(cc))
        *cp = tolower(cc);
    }
    // fprintf( stderr, "Variable name = '%s'\n", tok );

    // process the rest of the line:

    while (tok != (char *)nullptr) {
      if (tok[0] == '#')
        break;  // comment causes us to stop reading this line
      // fprintf( stderr, "tok = *%s*\n", tok );

      if (strcmp(tok, "newmtl") == 0) {
        thisMtl = new Mtl;
        thisMtl->Next = (Mtl *)nullptr;
        if (First == (Mtl *)nullptr) {
          First = thisMtl;
        }
        if (Last != (Mtl *)nullptr) {
          Last->Next = thisMtl;
        }
        Last = thisMtl;
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Name = strdup(tok);
        fprintf(stderr, "Material name = '%s'\n", thisMtl->Name);
        break;
      }

      if (strcmp(tok, "ka") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        if (tok != (char *)nullptr) thisMtl->Ka[0] = atof(tok);
        tok = strtok((char *)nullptr, DELIMS);
        if (tok != (char *)nullptr) thisMtl->Ka[1] = atof(tok);
        tok = strtok((char *)nullptr, DELIMS);
        if (tok != (char *)nullptr) thisMtl->Ka[2] = atof(tok);
        break;
      }

      if (strcmp(tok, "kd") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        if (tok != (char *)nullptr) thisMtl->Kd[0] = atof(tok);
        tok = strtok((char *)nullptr, DELIMS);
        if (tok != (char *)nullptr) thisMtl->Kd[1] = atof(tok);
        tok = strtok((char *)nullptr, DELIMS);
        if (tok != (char *)nullptr) thisMtl->Kd[2] = atof(tok);
        break;
      }

      if (strcmp(tok, "ke") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Ke[0] = atof(tok);
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Ke[1] = atof(tok);
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Ke[2] = atof(tok);
        break;
      }

      if (strcmp(tok, "ks") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Ks[0] = atof(tok);
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Ks[1] = atof(tok);
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Ks[2] = atof(tok);
        break;
      }

      if (strcmp(tok, "d") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->D = atof(tok);
        break;
      }

      if (strcmp(tok, "ni") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Ni = atof(tok);
        break;
      }

      if (strcmp(tok, "ns") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Ns = atof(tok);
        break;
      }

      if (strcmp(tok, "illum") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->Illum = atof(tok);
        break;
      }

      if (strcmp(tok, "map_kd") == 0) {
        tok = strtok((char *)nullptr, DELIMS);
        thisMtl->MapKd = strdup(tok);
        break;
      }

      fprintf(stderr, "Don't recognize Mtl file token '%s'\n", tok);
    }
  }
};

void Mtls::Close() {
  fclose(Fp);
  Fp = (FILE *)nullptr;
};

Mtl *Mtls::FindMtl(char *mtlName) {
  for (Mtl *mtl = First; mtl != (Mtl *)nullptr; mtl = mtl->Next) {
    if (strcmp(mtl->Name, mtlName) == 0) {
      return mtl;
    }
  }
  return (Mtl *)nullptr;
};
