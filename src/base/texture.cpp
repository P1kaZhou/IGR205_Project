#include <texture.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

void Texture2D::init() {
    if(filename) {
        loadFromFile();
    }
    else {
        loadZeros();
    }
    initGPUTexture();
}

void Texture2D::loadFromFile() {
    int channelCount = 0;
    switch (loadType)
    {
    case AUTO:
        channelCount = 0;
        break;
    case GRAY_LEVEL:
        channelCount = 1;
        break;
    case GRAY_ALPHA:
        channelCount = 2;
        break;
    case RBG:
        channelCount = 3;
        break;
    case RGBA:
        channelCount = 4;
        break;
    }
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(filename, &width, &height, &componentCount, channelCount);
    if(data == nullptr) {
        std::cerr << "Fail loading image : " << stbi_failure_reason() << std::endl;
    }
}

void Texture2D::loadZeros() {
    width = 50;
    height = 50;
    data = (unsigned char*) new char[width*height]{};
}

void Texture2D::initGPUTexture() {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    if(useMipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    getOpenGLError("texture params");

    int format = GL_RGB;
    if(componentCount == 1) {
        format = GL_RED;
    }
    else if(componentCount == 2) {
        format = GL_RG;
    }
    else if(componentCount == 3) {
        format = GL_RGB;
    }
    else if(componentCount == 4) {
        format = GL_RGBA;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );
    getOpenGLError("texture to gpu");
    
    if(useMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
        getOpenGLError("texture mipmap");
    }

    stbi_image_free(data);
}
