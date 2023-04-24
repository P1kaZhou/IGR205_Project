#include <frame-buffer.hpp>

void FrameBuffer::init(int width, int height,
    bool widthColorAttachment = false, bool widthDepthAttachment = false, bool widthStencilAttachment = false
) {
    this->width = width;
    this->height = height;
    this->widthColorAttachment = widthColorAttachment;
    this->widthDepthAttachment = widthDepthAttachment;
    this->widthStencilAttachment = widthStencilAttachment;

    // Create and bind frame buffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    if(widthColorAttachment) {
        // Color texture
        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0); // RBG GL_UNSIGNED_BYTE => values [0,255]
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0); // As color attachment 0
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // Render color in color attachment 0
    }
    else {
        // No color rendering if the color texture is not needed
        glDrawBuffer(GL_NONE);
    }

    if(widthDepthAttachment) {
        // Depth texture
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0); // DEPTH GL_FLOAT => values [0,1]
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0); // As depth attachment
    }
    else {
        // Use a render buffer for depth if no texture for depth is needed.
        // This is needed when using DEPTH TESTING.
        GLuint depthRenderBuffer;
        glGenRenderbuffers(1, &depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    }

    if(widthStencilAttachment) {
        // Stencil texture
        // Stencil test happens after fragment shader
        // Stencil values are 8 bits [0x00 - 0xFF] => [00000000 - 11111111]
        // If a stencil value is 1 => fragment rendered
        // Else => fragment discarded
        glStencilMask(0xFF); // Stencil test mask (8 bits) (stencil X mask = new stencil)
        glStencilFunc( // Set the stencil test pass condition
            GL_EQUAL, // Stencil test function
            1, // test stencil values to this value
            0xFF // mask to apply to both values before test
        );
        glStencilOp(
            GL_KEEP, // stencil test fails
            GL_KEEP, // stencil test passes & depth test fails
            GL_KEEP // stencil & depth test pass
        );
    }
    else {
        // If STENCIL TESTING, we need a render buffer for stencil attachment
    }

    // Check fram buffer
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "Cannot init frame buffer : " << status << "  " << GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT << std::endl;
      getOpenGLError("FrameBuffer incomplete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind fram buffer
    getOpenGLError("FrameBuffer - init");
}

void FrameBuffer::bindFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    if(widthDepthAttachment) {
        glEnable(GL_DEPTH_TEST);
    }
    if(widthStencilAttachment) {
        glEnable(GL_STENCIL_TEST);
    }
    getOpenGLError("FrameBuffer bind");
}

void FrameBuffer::saveColorTexturePPM(const std::string & filename) {
    unsigned char * pixels = new unsigned char[width*height*3];
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    writePPMImage(filename, width, height, 3, 255, pixels);
    getOpenGLError("FrameBuffer color texture to ppm");
}

void FrameBuffer::saveDepthTexturePPM(const std::string & filename) {
    float * pixels = new float[width*height];
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    writePPMImage(filename, width, height, 1, 1, pixels);
    getOpenGLError("FrameBuffer depth texture to ppm");
}

void FrameBuffer::saveStencilTexturePPM(const std::string & filename) {
    // unsigned char * pixels = new unsigned char[width*height];
    // glBindTexture(GL_TEXTURE_2D, stencilTexture);
    // glGetTexImage(GL_TEXTURE_2D, 0, , GL_UNSIGNED_BYTE, pixels);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // writePPMImage(filename, width, height, 1, 255, pixels);
    // getOpenGLError("FrameBuffer stencil texture to ppm");
}
