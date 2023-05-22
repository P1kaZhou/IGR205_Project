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
    float * pixels,
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
    ): width(width), height(height) {
        pixels = new float[height*width*3];
        memset(pixels, 0, height*width*3*sizeof(float));
    }

    ~DrawBuilder() {
        delete pixels;
    }

    inline void drawSegments(
        const std::vector<std::pair<glm::vec2, glm::vec2>> & segments,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & segmentColors255 = std::vector<glm::vec3>()
    ) {
        if(segments.size() == 0) return;
        {
            std::vector<glm::vec2> points;
            for(auto seg : segments) {
                points.push_back(seg.first);
                points.push_back(seg.second);
            }
            computeParams(points);
        }

        for(unsigned i=0;i<segments.size();i++) {
            auto seg = segments[i];
            auto a = seg.first;
            auto b = seg.second;
            int ai, aj;
            int bi, bj;
            getPointInImage(a,xSpan,ySpan,minX,minY,width-1,height-1,ai,aj);
            getPointInImage(b,xSpan,ySpan,minX,minY,width-1,height-1,bi,bj);
            if(segmentColors255.size()>=segments.size()) {
                insertLine(pixels, width, height, ai, aj, bi, bj, segmentColors255[i]);
            }
            else {
                insertLine(pixels, width, height, ai, aj, bi, bj, color);
            }
        }
    }

    inline void drawPoints(
        const std::vector<glm::vec2> & points,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & pointColors255 = std::vector<glm::vec3>()
    ) {
        if(points.size() == 0) return;
        computeParams(points);
        for(unsigned v=0;v<points.size();v++) {
            auto p = points[v];
            int i, j;
            getPointInImage(p,xSpan,ySpan,minX,minY,width-1,height-1,i,j);
            if(pointColors255.size()>=points.size()) {
                pixels[i*width*3+j*3] = pointColors255[v].r;
                pixels[i*width*3+j*3+1] = pointColors255[v].g;
                pixels[i*width*3+j*3+2] = pointColors255[v].b;
            }
            else {
                pixels[i*width*3+j*3] = color.r;
                pixels[i*width*3+j*3+1] = color.g;
                pixels[i*width*3+j*3+2] = color.b;
            }
        }
    }

    inline void drawTriangles(
        const std::vector<glm::vec2> & points,
        const std::vector<glm::uvec3> & triangles,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & triangleColors255 = std::vector<glm::vec3>()
    ) {
        if(points.size() == 0) return;
        if(triangles.size() == 0) return;
        computeParams(points);
        for(unsigned i=0;i<triangles.size();i++) {
            auto t = triangles[i];
            auto a = points[t.x];
            auto b = points[t.y];
            auto c = points[t.z];
            int ai, aj;
            int bi, bj;
            int ci, cj;
            getPointInImage(a,xSpan,ySpan,minX,minY,width-1,height-1,ai,aj);
            getPointInImage(b,xSpan,ySpan,minX,minY,width-1,height-1,bi,bj);
            getPointInImage(c,xSpan,ySpan,minX,minY,width-1,height-1,ci,cj);
            if(triangleColors255.size()>=triangles.size()) {
                insertLine(pixels, width, height, ai, aj, bi, bj, triangleColors255[i]);
                insertLine(pixels, width, height, ai, aj, ci, cj, triangleColors255[i]);
                insertLine(pixels, width, height, ci, cj, bi, bj, triangleColors255[i]);
            }
            else {
                insertLine(pixels, width, height, ai, aj, bi, bj, color);
                insertLine(pixels, width, height, ai, aj, ci, cj, color);
                insertLine(pixels, width, height, ci, cj, bi, bj, color);
            }
        }
    }

    inline void drawEdges(
        const std::vector<glm::vec2> & points,
        const std::vector<Edge> & edges,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & edgeColors255 = std::vector<glm::vec3>()
    ) {
        if(points.size() == 0) return;
        if(edges.size() == 0) return;
        computeParams(points);
        for(unsigned i=0;i<edges.size();i++) {
            auto t = edges[i];
            auto a = points[t.a];
            auto b = points[t.b];
            int ai, aj;
            int bi, bj;
            getPointInImage(a,xSpan,ySpan,minX,minY,width-1,height-1,ai,aj);
            getPointInImage(b,xSpan,ySpan,minX,minY,width-1,height-1,bi,bj);
            if(edgeColors255.size()>=edges.size()) {
                insertLine(pixels, width, height, ai, aj, bi, bj, edgeColors255[i]);
            }
            else {
                insertLine(pixels, width, height, ai, aj, bi, bj, color);
            }
        }
    }

    inline void drawShape(
        bool closed,
        const std::vector<glm::vec2> & points,
        const glm::vec3 & color,
        const std::vector<glm::vec3> & pointColors255 = std::vector<glm::vec3>()
    ) {
        if(points.size() == 0) return;
        computeParams(points);
        int x = 1;
        if(closed) x = 0;
        for(unsigned i=0;i<points.size()-x;i++) {
            auto a = points[i];
            auto b = points[(i+1)%points.size()];
            int ai, aj;
            int bi, bj;
            getPointInImage(a,xSpan,ySpan,minX,minY,width-1,height-1,ai,aj);
            getPointInImage(b,xSpan,ySpan,minX,minY,width-1,height-1,bi,bj);
            if(pointColors255.size()>=points.size()) {
                insertLine(pixels, width, height, ai, aj, bi, bj, pointColors255[i]);
            }
            else {
                insertLine(pixels, width, height, ai, aj, bi, bj, color);
            }
        }
    }

    inline void setExtraPoints(const std::vector<glm::vec2> & points) {
        extraPoints = points;
    }
    inline void addExtraPoints(const std::vector<glm::vec2> & points) {
        for(auto p : points) extraPoints.push_back(p);
    }
    inline void clearExtraPoints() {
        extraPoints.clear();
    }

    inline void save(std::string const & filename) {
        std::ofstream output_image(filename.c_str());
        output_image << "P3" << std::endl;
        output_image << width << " " << height << std::endl;
        output_image << "255" << std::endl;

        int i, j;
        for(i=0; i<height; ++i) {
            for(j=0; j<width; ++j) {
                output_image <<
                static_cast<unsigned int>(pixels[i*width*3+j*3]) << " " <<
                static_cast<unsigned int>(pixels[i*width*3+j*3+1]) << " " <<
                static_cast<unsigned int>(pixels[i*width*3+j*3+2]) << " ";
            }
            output_image << std::endl;
        }
        output_image.close();
    }

private:
    float * pixels = nullptr;
    unsigned width;
    unsigned height;

    float maxX;
    float minX;
    float maxY;
    float minY;
    float xSpan;
    float ySpan;

    std::vector<glm::vec2> extraPoints;

    void computeParams(const std::vector<glm::vec2> & points) {
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
