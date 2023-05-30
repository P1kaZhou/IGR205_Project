#ifndef _SKETCHY_DRAWING_
#define _SKETCHY_DRAWING_

#include <utils.hpp>

class Drawing {
public:
  Drawing() {
    init();
  }

  void continu() {
    if(!isDrawing) {
      isDrawing = true;
      subDrawingStarts.push_back(drawing.size());
    }
  }
  void stop() {
    if(isDrawing){
      isDrawing = false;
    }
  }

  /*
  Method for rendering the drawing:
  * Render points with lines
    - Take a lot of memory (number of points increase fast)
    - Modify the pos buffer object too often
  * Render points on a texture
    - Fixed amount of memory
    - Modify the texture too often

  What is faster :
    - Modifying texture uniform or
    - Modifying buffer object

  */

  void update(double xPos, double yPos) {
    if(isDrawing) {
      drawing.push_back({xPos, yPos, 1.0});

      // std::cout << "xPos = " << xPos << " yPos = " << yPos << std::endl;

      getOpenGLError("Drawing before update");
      size_t length = sizeof(float)*drawing.size()*3;
      glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
      glBufferData(GL_ARRAY_BUFFER, length, drawing.data(), GL_DYNAMIC_READ);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      getOpenGLError("Drawing update");
    }
  }

  void render2D() {
    getOpenGLError("Drawing before render");
    GLuint program = Program::getInstanceDrawing()->getProgram();
    glUseProgram(program);
    glBindVertexArray(m_vao);

    int start = 0;
    size_t count = 0;
    for(int i=0; i<subDrawingStarts.size(); i++) {
      start = subDrawingStarts[i];
      if(i<subDrawingStarts.size()-1) {
        count = subDrawingStarts[i+1] - start;
      }
      else {
        count = drawing.size() - start;
      }
      glDrawArrays(GL_LINE_LOOP, start, count);
      getOpenGLError("Drawing render");
    }
    
    glBindVertexArray(0);
  }

  void render3D(
    const glm::mat4 & projection,
    const glm::mat4 & view
  ) {
    getOpenGLError("Drawing before render");
    GLuint program = Program::getInstanceDrawingWorld()->getProgram();
    glUseProgram(program);

    glUniformMatrix4fv(glGetUniformLocation(program, "viewMat"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projMat"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(m_vao);

    int start = 0;
    size_t count = 0;
    for(int i=0; i<subDrawingStarts.size(); i++) {
      start = subDrawingStarts[i];
      if(i<subDrawingStarts.size()-1) {
        count = subDrawingStarts[i+1] - start;
      }
      else {
        count = drawing.size() - start;
      }
      glDrawArrays(GL_LINE_LOOP, start, count);
      getOpenGLError("Drawing render");
    }
    
    glBindVertexArray(0);
  }

  inline std::vector<glm::vec3> getDrawing(int i) {
    int start = subDrawingStarts[i];
    size_t count = 0;
    if(i<subDrawingStarts.size()-1) {
      count = subDrawingStarts[i+1] - start;
    }
    else {
      count = drawing.size() - start;
    }

    std::vector<glm::vec3> res;
    res.reserve(drawing.size());
    for(unsigned i=0; i<drawing.size(); i++) {
      if(i%10==0) {
        res.push_back(drawing[i]);
      }
    }

    return res;
  }

  inline unsigned drawingCount() {
    return subDrawingStarts.size();
  }

private:
  std::vector<glm::vec3> drawing;
  std::vector<unsigned> subDrawingStarts;

  bool isDrawing = false;

  GLuint m_vao = 0;
  GLuint m_posVbo = 0;

  void init() {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    getOpenGLError("Drawing before init");
    size_t length = sizeof(float)*drawing.size()*3;
    glGenBuffers(1, &m_posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glBufferData(GL_ARRAY_BUFFER, length, drawing.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    getOpenGLError("Drawing init");
  }

  void renderPositionsSet(
    const std::vector<unsigned> & positionsStarts,
    const std::vector<glm::vec3> & positions,
    GLuint vao
  ) {
    
  }
};

#endif
