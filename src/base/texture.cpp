#include <texture.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

template<typename T>
void Texture2D<T>::initFromImage(
    const char * filename, int componentCount, bool useMipmap
) {
    this->filename = filename;
    this->componentCount = componentCount;
    this->useMipmap = useMipmap;
    if(filename) {
        loadFromFile();
    }
    else {
        loadZeros();
    }
    initGPUTexture();
    
    if(filename) stbi_image_free(data);
}

template<typename T>
void Texture2D<T>::initFromData(
    T * data,
    int width, int height,
    int componentCount, bool useMipmap
) {
    this->data = data;
    this->width = width;
    this->height = height;
    this->componentCount = componentCount;
    this->useMipmap = useMipmap;
    initGPUTexture();
}

template<typename T>
void Texture2D<T>::loadFromFile() {
    stbi_set_flip_vertically_on_load(true);
    int componentCount_;
    data = (T*) stbi_load(filename, &width, &height, &componentCount_, componentCount);
    componentCount = componentCount_;
    std::cout 
        << "Texture loaded with"
        << " width " << width
        << " height " << height
        << " channels " << componentCount
        << std::endl;
    if(data == nullptr) {
        std::cerr << "Fail loading image : " << stbi_failure_reason() << std::endl;
    }
}

template<typename T>
void Texture2D<T>::loadZeros() {
    width = 50;
    height = 50;
    data = new T[width*height]{};
}

template<>
void Texture2D<unsigned char>::call_glTexImage2D() {
    int format = 0;
    int internalFormat = 0;
    if(componentCount == 1) {
        format = GL_RED;
        internalFormat = GL_RED;
    }
    else if(componentCount == 2) {
        format = GL_RG;
        internalFormat = GL_RG;
    }
    else if(componentCount == 3) {
        format = GL_RGB;
        internalFormat = GL_RGB;
    }
    else if(componentCount == 4) {
        format = GL_RGBA;
        internalFormat = GL_RGBA;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D, 0, internalFormat, width, height, 0, 
        format, GL_UNSIGNED_BYTE, data);
    getOpenGLError("texture to gpu (unsigned char)");
}
template<>
void Texture2D<float>::call_glTexImage2D() {
    int format = 0;
    int internalFormat = 0;
    if(componentCount == 1) {
        format = GL_RED;
        internalFormat = GL_RED;
    }
    else if(componentCount == 2) {
        format = GL_RG;
        internalFormat = GL_RG;
    }
    else if(componentCount == 3) {
        format = GL_RGB;
        internalFormat = GL_RGB;
    }
    else if(componentCount == 4) {
        format = GL_RGBA;
        internalFormat = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_2D, 0, internalFormat, width, height, 0, 
        format, GL_FLOAT, data);
    getOpenGLError("texture to gpu (float)");
}

template<typename T>
void Texture2D<T>::initGPUTexture() {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Texture coordinates clamping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Texture value interpolation
    if(interpolateToNearest) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    if(useMipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

    getOpenGLError("texture params");

    call_glTexImage2D();
    
    if(useMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
        getOpenGLError("texture mipmap");
    }
}
