#ifndef _SKETCHY_TEXTURE_
#define _SKETCHY_TEXTURE_

#include <utils.hpp>

/**
Holds a texture :
- read the image file
- create texture into the GPU
- handle binding to texture unit

T is the type for the coefficient in the image.
*/
template<typename T>
class Texture2D {
protected:
    T * data = nullptr;
    int componentCount = -1;
    int width = -1;
    int height = -1;

    GLuint texture = -1;
    int textureUnitIndex = -1;

    const char * filename = nullptr;
    bool useMipmap = true;


    void loadFromFile();
    void loadZeros();
    void initGPUTexture();

    void call_glTexImage2D();

public:
    Texture2D() {}

    bool interpolateToNearest = false;

    /**
    If filename is not null, read image data file from file.
    If filename is null, image data is filled with zeros of 50X50.
    */
    void initFromImage(
        const char * filename, int componentCount, bool useMipmap = true
    );

    void initFromData(
        T * data,
        int width, int height,
        int componentCount, bool useMipmap=false
    );

    /*
    Bind the texture to the provided texture unit.
    */
    inline void bind(int textureUnitIndex) {
        this->textureUnitIndex = textureUnitIndex;
        glActiveTexture(GL_TEXTURE0+textureUnitIndex);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    /*
    Unbind the texture from the texture unit.
    */
    inline void unbind() {
        if(textureUnitIndex >= 0) {
            glActiveTexture(GL_TEXTURE0+textureUnitIndex);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    /*
    Unbind the texture to the provided texture unit.
    */
    static inline void unbind(int textureUnitIndex) {
        if(textureUnitIndex >= 0) {
            glActiveTexture(GL_TEXTURE0+textureUnitIndex);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    /*
    Return the number of component in the image (1, 2, 3 or 4).
    */
    inline int getComponentCount() { return componentCount; } 
};

template class Texture2D<unsigned char>; // Force to compile functions for unsigned char
template class Texture2D<float>; // Force to compile functions for float

typedef Texture2D<unsigned char> ImageTexture;
typedef Texture2D<float> FloatTexture;

#endif
