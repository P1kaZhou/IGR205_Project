#include <mesh-geometry.hpp>

MeshGeometry * MeshGeometry::meshGetTriangleData(float width, float height, const glm::vec3 * color){
    MeshGeometry * geo = new MeshGeometry();
    geo->vertexPositions = {
        {-width/2, -height/3, 0.f},
        {width/2, -height/3, 0.f},
        {0.f, height*2/3, 0.f}
    };
    
    if(color) {
        geo->vertexColors = {
            {color->r, color->g*0.01f, color->b*0.01f},
            {color->r*0.01f, color->g, color->b*0.01f},
            {color->r*0.01f, color->g*0.01f, color->b}
        };
    }

    geo->vertexNormals = {
        {0.f, 0.f, 1.f},
        {0.f, 0.f, 1.f},
        {0.f, 0.f, 1.f}
    };
    geo->faces = {
        {0, 1, 2}
    };

    geo->boundingBox = Geometry::BoundingBox::computeBoundingBox(geo->vertexPositions);
    
    return geo;
}

MeshGeometry * MeshGeometry::meshGetPlaneData(float width, float height, const glm::vec3 * color) {
    MeshGeometry * geo = new MeshGeometry();
    geo->vertexPositions = {
        {-width/2, 0.f, height/2},
        {width/2, 0.f, height/2},
        {width/2, 0.f, -height/2},
        {-width/2, 0.f, -height/2},
    };
    
    if(color) {
        geo->vertexColors = {
            {color->r, color->g*0.01f, color->b*0.01f},
            {color->r*0.01f, color->g, color->b*0.01f},
            {color->r*0.01f, color->g*0.01f, color->b},
            {color->r, color->g*0.01f, color->b*0.01f},
        };
    }

    geo->vertexNormals = {
        {0.f, 1.f, 0.f},
        {0.f, 1.f, 0.f},
        {0.f, 1.f, 0.f},
        {0.f, 1.f, 0.f}
    };
    geo->faces = {
        {0, 1, 2},
        {0, 2, 3}
    };

    geo->boundingBox = Geometry::BoundingBox::computeBoundingBox(geo->vertexPositions);

    return geo;
}

MeshGeometry * MeshGeometry::meshGetCubeData(float lx, float ly, float lz, const glm::vec3 * color){
    MeshGeometry * geo = new MeshGeometry();

    // Vertex positions
    geo->vertexPositions = {
        {-lx/2, ly/2, lz/2},
        {-lx/2, -ly/2, lz/2},
        {-lx/2, ly/2, -lz/2},
        {-lx/2, -ly/2, -lz/2},
        {lx/2, ly/2, -lz/2},
        {lx/2, -ly/2, -lz/2},
        {lx/2, ly/2, lz/2},
        {lx/2, -ly/2, lz/2}
    };
    auto count = geo->vertexPositions.size();

    // Vertex colors
    if(color) {
        geo->vertexColors.reserve(count);
        for (unsigned i = 0; i < count; i++) {
            geo->vertexColors.push_back(
                {color->r*i/count, color->g*i/count, color->b*i/count}
            );
        }
    }

    // Triangle indices
    geo->faces.reserve(6*2);
    for (unsigned triangle = 0; triangle < 4*2; triangle++) {
        if(triangle%2 == 0) {
            geo->faces.push_back({
                triangle, (triangle+2)%8, (triangle+1)%8
            });
        }
        else {
            geo->faces.push_back({
                triangle, (triangle+1)%8, (triangle+2)%8
            });
        }
    }
    // Top face
    // geo->faces.push_back({0, 4, 2});
    // geo->faces.push_back({4, 0, 6});
    // Bottom face
    geo->faces.push_back({3, 5, 1});
    geo->faces.push_back({1, 5, 7});

    // Vertex normals
    glm::vec3 normal;
    for(unsigned i=0; i<geo->vertexPositions.size(); i++) {
        normal = geo->vertexPositions[i];
        glm::normalize(normal);
        geo->vertexNormals.push_back(normal);
    }

    geo->boundingBox = Geometry::BoundingBox::computeBoundingBox(geo->vertexPositions);

    return geo;
}
MeshGeometry * MeshGeometry::meshGetSphereData(float radius, unsigned precH, unsigned precV, const glm::vec3 * color){
    MeshGeometry * geo = new MeshGeometry();

    // Vertex positions and vertex texture coordinates
    //geo->vertexTexCoord.reserve(precV*precH*2);
    geo->vertexPositions.reserve(precV*precH*3);

    float phi = 0; // Horizontal angle (from 0 to 2*PI)
    float theta = 0; // Vertical angle (from 0 to PI)
    const float PI = glm::pi<float>();
    float x, y, z;
    for(unsigned j = 0; j <= precV; j++) {
        theta = j*PI/precV;
        for(unsigned i = 0; i < precH; i++) {
            // if(i < precH) {
                // Vertex position
                phi = i*2*PI/precH;
                x = radius*glm::sin(theta)*glm::cos(phi);
                z = radius*glm::sin(theta)*glm::sin(phi);
                y = radius*glm::cos(theta);
                geo->vertexPositions.push_back({x, y, z});
                // texture coordinate
                //geo->vertexTexCoord.push_back({phi/(2*PI), 1-theta/PI});
            // }
            // else {
            //     // Vertex position
            //     auto s = geo->vertexPositions->size();
            //     float x = geo->vertexPositions->data()[s-3];
            //     float y = geo->vertexPositions->data()[s-2];
            //     float z = geo->vertexPositions->data()[s-1];
            //     geo->vertexPositions->push_back(x);
            //     geo->vertexPositions->push_back(y);
            //     geo->vertexPositions->push_back(z);
            //     // texture coordinate
            //     geo->vertexTexCoord->push_back(phi/(2*PI));
            //     geo->vertexTexCoord->push_back(1-theta/PI);
            // }
        }
    }
    auto count = geo->vertexPositions.size();

    // Triangle indices
    const unsigned fCount = precH*precV*2;
    geo->faces.reserve(fCount);
    for(unsigned j=0; j<precV; j++) {
        for(unsigned i=0; i<precH; i++) {
            auto a = (j*precH + i);
            auto b = ((j+1)*precH + i);
            auto c = (j*precH + (i + 1)%precH);
            auto d = ((j+1)*precH + (i + 1)%precH);

            geo->faces.push_back({a, c, b});
            geo->faces.push_back({b, c, d});
        }
    }

    // Vertex colors
    if(color) {
        geo->vertexColors.reserve(count);
        for (unsigned i = 0; i < count; i++) {
            geo->vertexColors.push_back({color->r*i/count, color->g*i/count, color->b*i/count});
        }
    }

    // Vertex normals
    glm::vec3 normal;
    for(unsigned i=0; i<geo->vertexPositions.size(); i++) {
        normal = geo->vertexPositions[i];
        glm::normalize(normal);
        geo->vertexNormals.push_back(normal);
    }

    geo->recomputeVertexTexCoord_sphere_mapping();

    geo->boundingBox = Geometry::BoundingBox::computeBoundingBox(geo->vertexPositions);

    return geo;
}
MeshGeometry * MeshGeometry::meshGetConeData(){
    MeshGeometry * geo = new MeshGeometry();
    return geo;
}
MeshGeometry * MeshGeometry::meshGetPyramidData(){
    MeshGeometry * geo = new MeshGeometry();
    return geo;
}

static bool readOff(
    const char * filename,
    std::vector<glm::vec3> & vertexPos,
    std::vector<glm::uvec3> & faces
) {
    std::ifstream in(filename);
    if(!in) {
        std::cout << "Fail reading off file " << filename << std::endl;
        return false;
    }

    vertexPos.clear();
    faces.clear();

    std::string formatName;
    unsigned vertexCount, faceCount, edgeCount;
    in >> formatName >> vertexCount >> faceCount >> edgeCount;

    // Read vertex positions
    float x, y, z;
    vertexPos.resize(vertexCount);
    float scaleFactor = 0;
    for(unsigned i=0; i<vertexCount; i++) {
        in >> x >> y >> z;
        vertexPos[i] = {x, y, z};
        float dist = glm::l2Norm(vertexPos[i]);
        if(dist > scaleFactor) {
            scaleFactor = dist;
        }
    }

    scaleFactor = 1.0/scaleFactor;
    glm::mat4 scaleMat = glm::scale(glm::mat4(1), {scaleFactor, scaleFactor, scaleFactor});
    for(auto & pos : vertexPos) {
        pos = scaleMat*glm::vec4(pos, 1.0);
    }

    // Read indices
    unsigned poly;
    faces.reserve(faceCount);
    for(unsigned i=0; i<faceCount; i++) {
        in >> poly;
        unsigned inds[poly];
        for(unsigned j=0; j<poly; j++) {
            in >> inds[j];
        }
        if(poly == 3) {
            faces.push_back({
                inds[0], inds[1], inds[2]
            });
        }
        else {
            std::cout << "Trying to make Triangle of others primitive" << std::endl;
            assert(false);
        }
    }
    return true;
}

MeshGeometry * MeshGeometry::meshGetFromOfffile(const char * filename) {
    MeshGeometry * geo = new MeshGeometry();

    readOff(filename, geo->vertexPositions, geo->faces);
    geo->boundingBox = Geometry::BoundingBox::computeBoundingBox(geo->vertexPositions);
    geo->recomputeVertexNormals();
    
    return geo;
}

void MeshGeometry::showData() const {
    for(auto & pos : this->vertexPositions) {
        showVec(pos, "Pos");
    }
    for(auto & face : this->faces) {
        showVec(face, "Face");
    }
    for(auto & normal : this->vertexNormals) {
        showVec(normal, "Normal");
    }
}

/*Recompute vertex normals.*/
void MeshGeometry::recomputeVertexNormals() {
    vertexNormals.clear();
    vertexNormals.resize(vertexPositions.size(), {0.0,0.0,0.0});

    for(unsigned f=0; f<faces.size(); f++) {
        const glm::uvec3 & face = faces[f];
        glm::vec3 faceNormal = glm::cross(
            vertexPositions[face[1]]-vertexPositions[face[0]],
            vertexPositions[face[2]]-vertexPositions[face[0]]
        );
        vertexNormals[face[0]] += faceNormal;
        vertexNormals[face[1]] += faceNormal;
        vertexNormals[face[2]] += faceNormal;
    }

    for(unsigned v=0; v<vertexNormals.size(); v++) {
        glm::normalize(vertexNormals[v]);
    }
}

/*Recompute the texture coordinates using flat mapping on axis :
1: (x,y)
2: (x,z)
3: (y,z)*/
void MeshGeometry::recomputeVertexTexCoord_flat_mapping(
    short axis
) {
    vertexTexCoord.clear();
    vertexTexCoord.resize(vertexPositions.size(), {0.0,0.0});

    bool useX = (axis==1||axis==2);
    bool useY = (axis==1||axis==3);
    bool useZ = (axis==2||axis==3);

    float xMin = FLT_MAX, xMax = FLT_MIN;
    float yMin = FLT_MAX, yMax = FLT_MIN;
    float zMin = FLT_MAX, zMax = FLT_MIN;
    if(useX) {
        for(auto & v : vertexPositions) {
            xMin = std::min(xMin, v.x);
            xMax = std::max(xMax, v.x);
        }
    }
    if(useY) {
        for(auto & v : vertexPositions) {
            yMin = std::min(yMin, v.y);
            yMax = std::max(yMax, v.y);
        }
    }
    if(useZ) {
        for(auto & v : vertexPositions) {
            zMin = std::min(zMin, v.z);
            zMax = std::max(zMax, v.z);
        }
    }

    if(useX && useY) for(unsigned v=0; v<vertexTexCoord.size(); v++) {
        vertexTexCoord[v] = {
            (vertexPositions[v].x - xMin)/(xMax-xMin),
            (vertexPositions[v].y - yMin)/(yMax-yMin)
        };
    }
    if(useX && useZ) for(unsigned v=0; v<vertexTexCoord.size(); v++) {
        vertexTexCoord[v] = {
            (vertexPositions[v].x - xMin)/(xMax-xMin),
            (vertexPositions[v].z - zMin)/(zMax-zMin)
        };
    }
    if(useY && useZ) for(unsigned v=0; v<vertexTexCoord.size(); v++) {
        vertexTexCoord[v] = {
            (vertexPositions[v].y - yMin)/(yMax-yMin),
            (vertexPositions[v].z - zMin)/(zMax-zMin)
        };
    }
}

/*Recompute the texture coordinates using spherical mapping*/
void MeshGeometry::recomputeVertexTexCoord_sphere_mapping() {
    vertexTexCoord.clear();
    vertexTexCoord.resize(vertexPositions.size(), {0.0,0.0});
    for(unsigned v=0; v<vertexTexCoord.size(); v++) {
        const auto & vertex = vertexPositions[v];
        if(glm::length(vertex)>0) {
            // To spherical teta and phi coordinates
            float teta = glm::acos(vertex.z/glm::length(vertex));
            float phi = glm::sign(vertex.y)*glm::acos(vertex.x/glm::length(glm::vec2(vertex.x,vertex.y)));
            // Normalize
            teta = teta/M_PI;
            phi = phi/(2*M_PI);
            vertexTexCoord[v] = {
                teta, phi
            };
        }
    }
}

/*Map vertex coordinates in [-1;1] values interval.*/
void MeshGeometry::normalizeVertexPositions() {
    float xMin = FLT_MAX, xMax = FLT_MIN;
    float yMin = FLT_MAX, yMax = FLT_MIN;
    float zMin = FLT_MAX, zMax = FLT_MIN;
    for(auto & v : vertexPositions) {
        xMin = std::min(xMin, v.x);
        xMax = std::max(xMax, v.x);
        yMin = std::min(yMin, v.y);
        yMax = std::max(yMax, v.y);
        zMin = std::min(zMin, v.z);
        zMax = std::max(zMax, v.z);
    }

    for(unsigned v=0; v<vertexPositions.size(); v++) {
        vertexPositions[v] = {
            (vertexPositions[v].x - xMin)/(xMax-xMin),
            (vertexPositions[v].y - yMin)/(yMax-yMin),
            (vertexPositions[v].z - zMin)/(zMax-zMin)
        };
    }
}

void MeshGeometry::initVerticesTranforms(
    std::vector<glm::mat4> & vertexTransforms
) {
    
}
