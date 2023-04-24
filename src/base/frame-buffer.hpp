#ifndef _GUM3D_FRAME_BUFFER_
#define _GUM3D_FRAME_BUFFER_

#include <utils.hpp>

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

class FrameBuffer {
protected:
    GLuint fbo;
    
    GLuint colorTexture = -1;
    GLuint depthTexture = -1;
    GLuint stencilTexture = -1;

    int textureUnitIndex = -1;
    int width = -1;
    int height = -1;

    bool widthColorAttachment = false;
    bool widthDepthAttachment = false;
    bool widthStencilAttachment = false;

    inline static void bindTexture(int textureUnitIndex, GLuint texture) {
        glActiveTexture(GL_TEXTURE0+textureUnitIndex);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    inline static void unbindTexture(int textureUnitIndex) {
        glActiveTexture(GL_TEXTURE0+textureUnitIndex);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

public:

    void init(int width, int height,
        bool widthColorAttachment, bool widthDepthAttachment, bool widthStencilAttachment);

    inline bool isReady() {
        return widthColorAttachment || widthDepthAttachment || widthStencilAttachment;
    }

    void bindFrameBuffer();

    inline void unbindFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline void bindColorTexture(int textureUnitIndex) {
        bindTexture(textureUnitIndex, colorTexture);
    }
    inline void bindDepthTexture(int textureUnitIndex) {
        bindTexture(textureUnitIndex, depthTexture);
    }
    inline void bindStencilTexture(int textureUnitIndex) {
        bindTexture(textureUnitIndex, stencilTexture);
    }

    inline void clear() {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &colorTexture);
        glDeleteTextures(1, &depthTexture);
        glDeleteTextures(1, &stencilTexture);
    }

    void saveColorTexturePPM(const std::string & filename);
    void saveDepthTexturePPM(const std::string & filename);
    void saveStencilTexturePPM(const std::string & filename);

};

#endif
