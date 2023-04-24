#ifndef _GUM3D_TEXTURE_
#define _GUM3D_TEXTURE_

#include <utils.hpp>

/*Thhe type of an image*/
typedef enum {
    AUTO, GRAY_LEVEL, GRAY_ALPHA, RBG, RGBA
} ImageLoadType;

/**
Holds a texture :
- read the image file
- create texture into the GPU
- handle binding to texture unit
*/
class Texture2D {
protected:
    unsigned char * data;
    int componentCount = -1;
    int width = -1;
    int height = -1;

    GLuint texture = -1;
    int textureUnitIndex = -1;

    const char * filename;
    ImageLoadType loadType;
    bool useMipmap = true;

    void loadFromFile();
    void loadZeros();
    void initGPUTexture();

public:
    Texture2D(const char * filename, ImageLoadType loadType, bool useMipmap = true)
    : filename(filename), loadType(loadType), useMipmap(useMipmap) {}

    /**
    If filename is not null, read image data file from file.
    If filename is null, image data is filled with zeros of 50X50.
    */
    void init();
    
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

#endif
