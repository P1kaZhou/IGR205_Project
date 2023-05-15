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

typedef struct {
    std::vector<glm::vec2> & points;
    std::vector<glm::vec3> & pointColors255;
    std::vector<glm::uvec3> & triangles;
    std::vector<glm::vec3> & triangleColors255;
    std::vector<Edge> & edges;
    std::vector<glm::vec3> & edgeColors255;
} Mesh2DDrawParam;

inline void saveImagePPM(
    std::string const & filename,
    unsigned width, unsigned height,
    const Mesh2DDrawParam & params
) {
    float maxX = params.points[0].x;
    float minX = params.points[0].x;
    float maxY = params.points[0].y;
    float minY = params.points[0].y;
    for(auto p : params.points) {
        if(p.x>maxX) maxX = p.x;
        if(p.x<minX) minX = p.x;
        if(p.y>maxY) maxY = p.y;
        if(p.y<minY) minY = p.y;
    }
    float xSpan = maxX-minX;
    float ySpan = maxY-minY;

    float * pixels = new float[height*width*3];
    memset(pixels, 0, height*width*3*sizeof(float));
    for(unsigned v=0;v<params.points.size();v++) {
        auto p = params.points[v];
        int i, j;
        getPointInImage(p,xSpan,ySpan,minX,minY,width-1,height-1,i,j);
        pixels[i*width*3+j*3] = params.pointColors255[v].r;
        pixels[i*width*3+j*3+1] = params.pointColors255[v].g;
        pixels[i*width*3+j*3+2] = params.pointColors255[v].b;
    }

    for(unsigned i=0;i<params.triangles.size();i++) {
        auto t = params.triangles[i];
        auto a = params.points[t.x];
        auto b = params.points[t.y];
        auto c = params.points[t.z];
        int ai, aj;
        int bi, bj;
        int ci, cj;
        getPointInImage(a,xSpan,ySpan,minX,minY,width-1,height-1,ai,aj);
        getPointInImage(b,xSpan,ySpan,minX,minY,width-1,height-1,bi,bj);
        getPointInImage(c,xSpan,ySpan,minX,minY,width-1,height-1,ci,cj);
        insertLine(pixels, width, height, ai, aj, bi, bj, params.triangleColors255[i]);
        insertLine(pixels, width, height, ai, aj, ci, cj, params.triangleColors255[i]);
        insertLine(pixels, width, height, ci, cj, bi, bj, params.triangleColors255[i]);
    }

    for(unsigned i=0;i<params.edges.size();i++) {
        auto t = params.edges[i];
        auto a = params.points[t.a];
        auto b = params.points[t.b];
        int ai, aj;
        int bi, bj;
        getPointInImage(a,xSpan,ySpan,minX,minY,width-1,height-1,ai,aj);
        getPointInImage(b,xSpan,ySpan,minX,minY,width-1,height-1,bi,bj);
        insertLine(pixels, width, height, ai, aj, bi, bj, params.edgeColors255[i]);
    }

    std::ofstream output_image(filename.c_str());
    std::cout << "Open" << std::endl;
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
    delete pixels;
    output_image.close();
}

}

#endif
