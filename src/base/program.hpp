#ifndef _SKETCHY_MESH_PROGRAM_
#define _SKETCHY_MESH_PROGRAM_

#include <utils.hpp>

/*
Holds singletons for each shader program.
*/
class Program {
protected:
    ~Program() {
        if(glIsProgram(program))
            glDeleteProgram(program);
    }
    static Program * instancePhong;
    static Program * instanceShadowMap;
    static Program * instanceSingleColor;
    static Program * instanceDrawing;
    static Program * instanceDrawingWorld;

    GLuint program;
    unsigned int programUserCount;

    const char * vertexShaderFilename;
    const char * fragmentShaderFilename;
    
    void loadShader(GLuint program, GLenum type, const std::string &shaderFilename);
    void initProgram();

public:
    /*Returns the instance for the phong shader program.*/
    static Program * getInstancePhong();
    static Program * getInstanceShadowMap();
    static Program * getInstanceSingleColor();
    static Program * getInstanceDrawing();
    static Program * getInstanceDrawingWorld();
    
    /*Subscribe to the program. This compile the program if it was previously delete from the GPU.*/
    void subscribe();
    /*Subscribe to the program. This delete the program from the GPU if there are no left subscribers.*/
    void unsubscribe();

    /*Returns the program id.*/
    inline GLuint getProgram() { return program; }
};

#endif
