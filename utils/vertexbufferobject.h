// #pragma once

// #include <stdio.h>
// #include <string.h>

// #include <map>
// #include <vector>

// #ifdef __APPLE__
// #include <OpenGL/gl.h>
// #else
// #include <GL/gl.h>
// #endif

// struct Point {
//   GLfloat x, y, z;     // vertex coordinates
//   GLfloat nx, ny, nz;  // normal coordinates
//   GLfloat r, g, b;     // color components
//   GLfloat s, t;        // texture coordinates
// };

// struct Key {
//   GLfloat x, y, z;
//   Key(GLfloat X, GLfloat Y, GLfloat Z) {
//     x = X;
//     y = Y;
//     z = Z;
//   }
// };

// class VertexBufferObject {
//  public:
//   static const GLuint RESTART_INDEX = ~0u;  // all 1's

//   // Constructor
//   VertexBufferObject() { Init(); }

//   // Main drawing functions
//   void Draw();
//   void DrawInstanced(int numInstances);

//   // GL-style interface functions
//   void glBegin(GLenum topology);
//   void glEnd();
//   void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
//   void glVertex3fv(GLfloat* xyz);
//   void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
//   void glNormal3fv(GLfloat* nxyz);
//   void glColor3f(GLfloat r, GLfloat g, GLfloat b);
//   void glColor3fv(GLfloat* rgb);
//   void glTexCoord2f(GLfloat s, GLfloat t);
//   void glTexCoord2fv(GLfloat* st);

//   // Control functions
//   void Init();
//   void Reset();
//   void RestartPrimitive();
//   void SetVerbose(bool v);
//   void CollapseCommonVertices(bool tf);
//   void Print(char* text, FILE* fpout);

//  private:
//   // Constants
//   static const int THREE_VALUES = 3;
//   static const int TWO_VALUES = 2;

//   // Helper functions
//   GLuint AddVertex(GLfloat x, GLfloat y, GLfloat z);

// // Utility macros/functions
// #define ELEMENT_OFFSET(start, end) ((void*)((char*)(end) - (char*)(start)))
// #define BUFFER_OFFSET(i) ((char*)NULL + (i))

//   typedef std::map<Key, int> PMap;

//   std::vector<Point> PointVec;
//   std::vector<GLuint> ElementVec;
//   PMap PointMap;

//   struct Point* parray;
//   GLuint* earray;
//   GLuint pbuffer;
//   GLuint ebuffer;

//   GLenum topology;
//   bool verbose;
//   bool isFirstDraw;
//   bool hasVertices;
//   bool hasNormals;
//   bool hasColors;
//   bool hasTexCoords;
//   bool collapseCommonVertices;
//   bool restartFound;
//   bool glBeginWasCalled;

//   // Current attribute values
//   GLfloat c_nx, c_ny, c_nz;  // current normal
//   GLfloat c_r, c_g, c_b;     // current color
//   GLfloat c_s, c_t;          // current texture coords
// };

// // Helper functions
// bool operator<(const Key& k0, const Key& k1);
// bool operator==(const Key& k0, const Key& k1);
// bool IsExtensionSupported(const char* extension);