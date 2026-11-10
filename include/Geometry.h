#pragma once
#include <vector>

struct Vertex { float x,y,z,nx,ny,nz,u,v; };
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

namespace geometry {
MeshData box();
MeshData plane();
MeshData frustum(float topRadius, int segments = 32);
}
