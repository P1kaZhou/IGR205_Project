#include "draw-2d.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image_write.h>
#endif

namespace Geometry {

DrawBuilder::DrawBuilder(
    unsigned width,
    unsigned height
): width(width), height(height) {
    pixels = new uint8_t[height*width*3];
    memset(pixels, 0, height*width*3*sizeof(uint8_t));

    for(int i=0; i<height; ++i) {
        for(int j=0; j<width; ++j) {
            pixels[i*width*3+j*3] = 255;
            pixels[i*width*3+j*3+1] = 255;
            pixels[i*width*3+j*3+2] = 255;
        }
    }
}

void DrawBuilder::drawSegments(
    const std::vector<std::pair<glm::vec2, glm::vec2>> & segments,
    const glm::vec3 & color,
    const std::vector<glm::vec3> & segmentColors255
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

void DrawBuilder::drawPoints(
    const std::vector<glm::vec2> & points,
    const glm::vec3 & color,
    const std::vector<glm::vec3> & pointColors255
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

void DrawBuilder::drawTriangles(
    const std::vector<glm::vec2> & points,
    const std::vector<glm::uvec3> & triangles,
    const glm::vec3 & color,
    const std::vector<glm::vec3> & triangleColors255
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

void DrawBuilder::drawEdges(
    const std::vector<glm::vec2> & points,
    const std::vector<Edge> & edges,
    const glm::vec3 & color,
    const std::vector<glm::vec3> & edgeColors255
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

void DrawBuilder::drawShape(
    bool closed,
    const std::vector<glm::vec2> & points,
    const glm::vec3 & color,
    const std::vector<glm::vec3> & pointColors255
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

void DrawBuilder::savePPM(std::string const & filename) {
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

void DrawBuilder::savePNG(std::string const & filename) {
    stbi_write_png(
        filename.c_str(),
        width, height,
        3,
        pixels, width*3
    );
}

}