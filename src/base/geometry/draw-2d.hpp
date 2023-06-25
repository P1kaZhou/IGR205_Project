#ifndef _GUM3D_DRAW_2D_
#define _GUM3D_DRAW_2D_

#include "geometry.hpp"
#include <cstring>
#include <fstream>

namespace Geometry {

inline void getPointInImage(
    glm::vec2 p,
    float xSpan, float ySpan,
    float minX, float minY,
    unsigned width, unsigned height,
    int & i, int & j
) {
    float x = ((p.x-minX)/xSpan)*width;
    float y = ((p.y-minY)/ySpan)*height;
    i = glm::floor(y);
    j = glm::floor(x);
}

inline void insertLine(
    uint8_t * pixels,
    unsigned width, unsigned height,
    int ai, int aj, int bi, int bj,
    const glm::vec3 & color
) {
    if((ai-bi)*(ai-bi)+(aj-bj)*(aj-bj)==0) return;

    int di = bi-ai;
    int dj = bj-aj;
    int posi, posj;
    
    if(glm::abs(di)>glm::abs(dj)) {
        int step = ai<bi?1:-1;
        for(posi=ai+step; (step>0&&posi<bi)||(step<0&&posi>bi); posi+=step) {
            if(posi!=ai) {
                posj = aj + (dj/(float)di)*(posi-ai);
                pixels[posi*width*3+posj*3] = color.r;
                pixels[posi*width*3+posj*3+1] = color.g;
                pixels[posi*width*3+posj*3+2] = color.b;
            }
        }
    }
    else {
        int step = aj<bj?1:-1;
        for(posj=aj; (step>0&&posj<bj)||(step<0&&posj>bj); posj+=step) {
            if(posj!=aj) {
                posi = ai + (di/(float)dj)*(posj-aj);
                pixels[posi*width*3+posj*3] = color.r;
                pixels[posi*width*3+posj*3+1] = color.g;
                pixels[posi*width*3+posj*3+2] = color.b;
            }
        }
    }
}

class DrawBuilder {
public:
    DrawBuilder(
        unsigned width,
        unsigned height
    );

    ~DrawBuilder() {
        delete pixels;
    }

    void drawSegments(
        const std::vector<std::pair<glm::vec2, glm::vec2>> & segments,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & segmentColors255 = std::vector<glm::vec3>()
    );

    void drawPoints(
        const std::vector<glm::vec2> & points,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & pointColors255 = std::vector<glm::vec3>()
    );

    void drawTriangles(
        const std::vector<glm::vec2> & points,
        const std::vector<glm::uvec3> & triangles,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & triangleColors255 = std::vector<glm::vec3>()
    );

    void drawEdges(
        const std::vector<glm::vec2> & points,
        const std::vector<Edge> & edges,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & edgeColors255 = std::vector<glm::vec3>()
    );

    void drawShape(
        bool closed,
        const std::vector<glm::vec2> & points,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & pointColors255 = std::vector<glm::vec3>()
    );

    inline void setExtraPoints(const std::vector<glm::vec2> & points) {
        extraPoints = points;
    }
    inline void addExtraPoints(const std::vector<glm::vec2> & points) {
        for(auto p : points) extraPoints.push_back(p);
    }
    inline void clearExtraPoints() {
        extraPoints.clear();
    }

    void savePPM(std::string const & filename);
    void savePNG(std::string const & filename);

    inline void save(
        std::string const & filename,
        bool ppm=false, bool png=true
    ) {
        if(ppm) savePPM(filename);
        if(png) savePNG(filename);
    }

private:
    uint8_t * pixels = nullptr;
    unsigned width;
    unsigned height;

    float maxX;
    float minX;
    float maxY;
    float minY;
    float xSpan;
    float ySpan;

    std::vector<glm::vec2> extraPoints;

    inline void computeParams(const std::vector<glm::vec2> & points) {
        maxX = points[0].x;
        minX = points[0].x;
        maxY = points[0].y;
        minY = points[0].y;
        for(auto p : points) {
            if(p.x>maxX) maxX = p.x;
            if(p.x<minX) minX = p.x;
            if(p.y>maxY) maxY = p.y;
            if(p.y<minY) minY = p.y;
        }
        for(auto p : extraPoints) {
            if(p.x>maxX) maxX = p.x;
            if(p.x<minX) minX = p.x;
            if(p.y>maxY) maxY = p.y;
            if(p.y<minY) minY = p.y;
        }
        xSpan = maxX-minX;
        ySpan = maxY-minY;
    }
};

}

#endif
