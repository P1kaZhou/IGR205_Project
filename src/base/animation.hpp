#ifndef _SKETCHY_ANIMATION_
#define _SKETCHY_ANIMATION_

#include <utils.hpp>
#include <mesh-group.hpp>

typedef struct {
    glm::vec3 position = {0, 0.0f, 0.0f};
    glm::vec3 orientation = {0, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    long time = 0; // In millis
} AnimationFrame;

class FrameAnimator {
    friend class Animation;
public:
    FrameAnimator(
        const AnimationFrame & startFrame, const AnimationFrame & endFrame
    ): startFrame(startFrame), endFrame(endFrame) {
        totalDuration = endFrame.time-startFrame.time;
        float inverseTime = (1.0f/(float) totalDuration);
        
        glm::vec3 totalMoveVector = endFrame.position-startFrame.position;
        posSpeed = totalMoveVector*inverseTime;
        
        glm::vec3 totalScaleDiff = endFrame.scale-startFrame.scale;
        scaleSpeed = totalScaleDiff*inverseTime;

        glm::vec3 totalOrientationDiff = endFrame.orientation-startFrame.orientation;
        orientationSpeed = totalOrientationDiff*inverseTime;
    }

    inline void start(long time) {
        startTime = time;
        localTime = 0;
    }

    inline void updateTime(long currentTime) {
        localTime = currentTime-startTime;
    }

    inline bool isFinished() {
        return localTime > totalDuration;
    }

    inline glm::vec3 nextPosition() const {
        return ((float)localTime)*posSpeed+startFrame.position;
    }
    inline glm::vec3 nextOrientation() const {
        return ((float)localTime)*orientationSpeed+startFrame.orientation;
    }
    inline glm::vec3 nextScale() const {
        return ((float)localTime)*scaleSpeed+startFrame.scale;
    }

private:
    AnimationFrame startFrame;
    AnimationFrame endFrame;

    long startTime;
    long totalDuration;
    long localTime;
    glm::vec3 posSpeed;
    glm::vec3 scaleSpeed;
    glm::vec3 orientationSpeed;
};

class Animation {
public:

    inline void start(long time) {
        current = 0;
        if(!animators.empty())
            animators.begin()->start(time);
    }

    inline void animate(long time) {
        if(animators[current].isFinished()) {
            if(current >= animators.size()-1) {
                if(loop) {
                    current = 0;
                }
                else {
                    return;
                }
            }
            else {
                current += 1;
            }
            animators[current].start(time);
        }
        animators[current].updateTime(time);
        mesh->setPosition(animators[current].nextPosition());
        mesh->setScale(animators[current].nextScale());
        mesh->setOrientation(animators[current].nextOrientation());
    }

    inline void setMesh(MeshGroup * m) { mesh = m; }
    inline void setIsLoop(bool b) { loop = b; }

    inline void setFrames(const std::vector<AnimationFrame> & frames) {
        animators.reserve(frames.size()-1);
        for(unsigned i=0; i<frames.size()-1; i++) {
            auto & startF = frames[i];
            auto & endF = frames[i+1];
            animators.emplace_back(startF, endF);
        }
    }

protected:
    MeshGroup * mesh;

    std::vector<FrameAnimator> animators;
    unsigned current;

    bool loop = false;
};

#endif
