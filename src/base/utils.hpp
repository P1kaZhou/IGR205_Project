#ifndef _SKETCHY_UTILS_
#define _SKETCHY_UTILS_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/reciprocal.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

#include <chrono>

inline int getOpenGLError(const char * where) {
    int error = glGetError();
    if(error != GL_NO_ERROR) {
      std::cout << "GL Error detected : " << where << " : " << error << std::endl;
      assert(false);
    }
    return error;
}

inline long long getTimeMillis() {
    auto t = std::chrono::high_resolution_clock::now();
    auto td = std::chrono::duration_cast<std::chrono::duration<long long, std::ratio<1l, 1000l>>>(
        t.time_since_epoch());
    return td.count();
}

// Loads the content of an ASCII file in a standard C++ string
inline std::string file2String(const std::string &filename) {
  std::ifstream t(filename.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

/**Create a char pointer containing a string starting with start, the index and then end.
 * indexedStrSize is the size of the integer index as a string.*/
inline char * buildIndexedString(const char * start, int index, const char * end, unsigned indexedStrSize) {
    if(start == nullptr && end == nullptr) {
        return nullptr;
    }
    else if(end == nullptr) {
        char * str = new char[strlen(start)+indexedStrSize + 1];
        sprintf(str, "%s%d", start, index);
        return str;
    }
    else if(start == nullptr) {
        char * str = new char[strlen(end)+indexedStrSize + 1];
        sprintf(str, "%d%s", index, end);
        return str;
    }
    else {
        char * str = new char[strlen(start)+strlen(end)+indexedStrSize + 1];
        sprintf(str, "%s%d%s", start, index, end);
        return str;
    }
}

/**Create a char pointer containing a string starting with start, the index and then end.*/
inline char * buildIndexedString(const char * start, int index, const char * end) {
    int decimalLog = index == 0 ? 1 : ( (int) (glm::log(index)/glm::log(10))+1 );
    return buildIndexedString(start, index, end, decimalLog);
}

inline void showMatrix(const glm::mat4 & m, const char * name = "") {
  std::cout  << name << " : " << std::endl;
  for (unsigned i=0; i<4; i++) {
    for (unsigned j=0; j<4; j++) {
      std::cout << m[i][j] << "  ";
    }
    std::cout << std::endl;
  }
}

inline void showVec(const glm::vec3 & v, const char * name = "") {
  std::cout  << name << " : " << std::endl;
  std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}
inline void showVec(const glm::uvec3 & v, const char * name = "") {
  std::cout  << name << " : " << std::endl;
  std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

template<typename T>
inline void writePPMImage(const std::string & filename, const int width, const int height, const int componentCount, unsigned int maxVal, const T * data) {
  std::ofstream imageStream(filename.c_str());
  int i, j, k;
  imageStream << "P3" << std::endl;
  imageStream << width << " " << height << std::endl;
  imageStream << "255" << std::endl;
  k = 0;
  float ff = 255/maxVal;
  for(i=0; i<width; ++i) {
      for(j=0; j<height; ++j) {
        if(componentCount == 1) {
          imageStream <<
            static_cast<unsigned int>(ff*data[k]) << " " <<
            static_cast<unsigned int>(ff*data[k]) << " " <<
            static_cast<unsigned int>(ff*data[k]) << " ";
          k = k+1;
        }
        else if(componentCount == 3) {
          imageStream <<
            static_cast<unsigned int>(ff*data[k]) << " " <<
            static_cast<unsigned int>(ff*data[k+1]) << " " <<
            static_cast<unsigned int>(ff*data[k+2]) << " ";
          k = k+3;
        }
      }
      imageStream << std::endl;
  }
  imageStream.close();
}

#define COLOR_BLACK glm::vec3(0.0, 0.0, 0.0)
#define COLOR_WHITE glm::vec3(1.0, 1.0, 1.0)
#define COLOR_RED glm::vec3(1.0, 0.1, 0.1)
#define COLOR_GREEN glm::vec3(0.1, 1.0, 0.1)
#define COLOR_BLUE glm::vec3(0.1, 0.1, 1.0)
#define COLOR_YELLOW glm::vec3(0.8, 0.8, 0.1)
#define COLOR_PURPLE glm::vec3(0.5, 0.1, 0.5)
#define COLOR_ORANGE glm::vec3(1.0, 0.5, 0.0)
#define COLOR_GRAY glm::vec3(0.5, 0.5, 0.5)
#define COLOR_PINK glm::vec3(1.0, 0.0, 1.0)

/*Function to write STL file.*/
void writeSTL(
    std::string filename,
    const std::vector<glm::ivec3> & faces,
    const std::vector<glm::vec3> & vertices
) {
    std::ofstream out(filename);

    out << "solid name" << std::endl;
    for(unsigned i=0;i<faces.size(); i++) {
      out << "facet normal " << 0 << " " << 0 << " " << 0 << std::endl;
      out << "\touter loop" << std::endl;
      out << "\t\tvertex " <<
        vertices[faces[i].x].x << " " <<
        vertices[faces[i].x].y << " " <<
        vertices[faces[i].x].z << 
        std::endl;
      out << "\t\tvertex " <<
        vertices[faces[i].y].x << " " <<
        vertices[faces[i].y].y << " " <<
        vertices[faces[i].y].z <<
        std::endl;
      out << "\t\tvertex " <<
        vertices[faces[i].z].x << " " <<
        vertices[faces[i].z].y << " " <<
        vertices[faces[i].z].z <<
        std::endl;
      out << "\tendloop" << std::endl;
      out << "endfacet" << std::endl;
    }
    out << "endsolid name" << std::endl;
}

#endif
