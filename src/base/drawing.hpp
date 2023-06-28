#ifndef _SKETCHY_DRAWING_
#define _SKETCHY_DRAWING_

#include <utils.hpp>
#include <texture.hpp>

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

    // std::vector<glm::vec3> res;
    // res.reserve(drawing.size());
    // for(unsigned i=start; i<start+count; i++) {
    //   if(i%10==0) {
    //     res.push_back(drawing[i]);
    //   }
    // }

    // return res;

    return std::vector<glm::vec3>(
      drawing.begin()+start,
      drawing.begin()+(start+count)
    );
  }

  inline unsigned drawingCount() {
    return subDrawingStarts.size();
  }

  inline void clearDrawing() {
    drawing.clear();
    subDrawingStarts.clear();
    getOpenGLError("Drawing before update");
    size_t length = sizeof(float)*drawing.size()*3;
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glBufferData(GL_ARRAY_BUFFER, length, drawing.data(), GL_DYNAMIC_READ);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    getOpenGLError("Drawing update");
  }

  inline void loadShape(const char * filename) {
    std::ifstream in(filename);

    int count;
    in >> count;
    double xPos, yPos;
    continu();
    for(int i=0; i<count; i++) {
      std::cout << xPos << " " << yPos << std::endl;
      in >> xPos >> yPos;
      update(xPos, yPos);
    }
    stop();
  }

  inline void saveShape(const char * filename) {
    std::ofstream out(filename);

    out << int(drawing.size()) << std::endl;
    double xPos, yPos;
    for(int i=0; i<drawing.size(); i++) {
      xPos = drawing[i].x;
      yPos = drawing[i].y;
      out << xPos << " " << yPos << std::endl;
    }
  }

  inline void setupTexture(const char * textureFilename) {
    vs = {
      {-1, 1, 0.f},
      {1, 1, 0.f},
      {1, -1, 0.f},
      {-1, -1, 0.f}
    };
    texcs = {
      {0, 1},
      {1, 1},
      {1, 0},
      {0, 0}
    };
    fs = {
        {0, 1, 2},
        {0, 2, 3}
    };

    glGenVertexArrays(1, &tex_vao);
    glBindVertexArray(tex_vao);

    glGenBuffers(1, &tex_posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, tex_posVbo);
    glBufferData(GL_ARRAY_BUFFER, vs.size()*3*sizeof(float), vs.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &tex_texVbo);
    glBindBuffer(GL_ARRAY_BUFFER, tex_texVbo);
    glBufferData(GL_ARRAY_BUFFER, texcs.size()*2*sizeof(float), texcs.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &tex_Ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tex_Ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size()*3*sizeof(float), fs.data(), GL_DYNAMIC_READ);

    glBindVertexArray(0);
    isTextureInit = true;
    getOpenGLError("Init shape array buffer");

    coverTexture = new ImageTexture();
    coverTexture->initFromImage(textureFilename, 3, false);
  }

  inline void renderTexture() {
    GLuint program = Program::getInstanceCover()->getProgram();
    glUseProgram(program);
    coverTexture->bind(0);
    glUniform1i(glGetUniformLocation(program, "colorTexture"), 0);

    glBindVertexArray(tex_vao);
    glEnable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, fs.size()*3, GL_UNSIGNED_INT, 0);
    getOpenGLError("Drawin shape");
  }

  bool isTextureInit = false;

private:
  std::vector<glm::vec3> drawing;
  std::vector<unsigned> subDrawingStarts;

  bool isDrawing = false;

  GLuint m_vao = 0;
  GLuint m_posVbo = 0;

  ImageTexture * coverTexture = nullptr;
  GLuint tex_vao = 0;
  GLuint tex_posVbo = 0;
  GLuint tex_texVbo = 0;
  GLuint tex_Ibo = 0;
  std::vector<glm::vec3> vs;
  std::vector<glm::uvec3> fs;
  std::vector<glm::vec2> texcs;

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
