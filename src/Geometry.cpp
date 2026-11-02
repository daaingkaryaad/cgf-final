#include "Geometry.h"
#include "Math3D.h"
#include <stdexcept>

namespace geometry {
MeshData box() {
    MeshData data;
    const math3d::vec3 normals[] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
    const math3d::vec3 axes[] = {{0,0,-1},{0,0,1},{1,0,0},{1,0,0},{1,0,0},{-1,0,0}};
    for (int face=0; face<6; ++face) {
        const auto n=normals[face], u=axes[face], v=math3d::cross(n,u);
        const unsigned int base=unsigned(data.vertices.size());
        for (int j=0; j<4; ++j) {
            const float x=(j==1 || j==2) ? 1.0f : 0.0f;
            const float y=j>=2 ? 1.0f : 0.0f;
            const auto p=n*0.5 + u*(x-0.5) + v*(y-0.5);
            data.vertices.push_back({float(p.x),float(p.y),float(p.z),float(n.x),float(n.y),float(n.z),x,y});
        }
        data.indices.insert(data.indices.end(),{base,base+1,base+2,base,base+2,base+3});
    }
    return data;
}

MeshData plane() {
    return {{{-0.5f,0,-0.5f,0,1,0,0,0},{-0.5f,0,0.5f,0,1,0,0,1},
             {0.5f,0,0.5f,0,1,0,1,1},{0.5f,0,-0.5f,0,1,0,1,0}},
            {0,1,2,0,2,3}};
}

MeshData frustum(float topRadius, int segments) {
    if (segments<3 || topRadius<0) throw std::runtime_error("invalid frustum");
    MeshData data;
    const float slope = 1.0f-topRadius;
    const float invLength = 1.0f/std::sqrt(1.0f+slope*slope);
    for (int i=0; i<=segments; ++i) {
        const double a=2.0*math3d::pi*i/segments;
        const float c=float(std::cos(a)), s=float(std::sin(a)), u=float(i)/segments;
        data.vertices.push_back({c,-0.5f,s,c*invLength,slope*invLength,s*invLength,u,0});
        data.vertices.push_back({c*topRadius,0.5f,s*topRadius,c*invLength,slope*invLength,s*invLength,u,1});
    }
    for (int i=0; i<segments; ++i) {
        const unsigned int b=unsigned(i*2);
        data.indices.insert(data.indices.end(),{b,b+1,b+2});
        if (topRadius>0) data.indices.insert(data.indices.end(),{b+1,b+3,b+2});
    }
    for (int cap=0; cap<2; ++cap) {
        const float radius=cap ? topRadius : 1.0f;
        if (radius==0) continue;
        const float y=cap ? 0.5f : -0.5f, normal=cap ? 1.0f : -1.0f;
        const unsigned int base=unsigned(data.vertices.size());
        data.vertices.push_back({0,y,0,0,normal,0,0.5f,0.5f});
        for (int i=0; i<=segments; ++i) {
            const double a=2.0*math3d::pi*i/segments;
            const float c=float(std::cos(a)), s=float(std::sin(a));
            data.vertices.push_back({radius*c,y,radius*s,0,normal,0,c*0.5f+0.5f,s*0.5f+0.5f});
        }
        for (int i=0; i<segments; ++i) {
            const unsigned int k=base+1+unsigned(i);
            if (cap) data.indices.insert(data.indices.end(),{base,k+1,k});
            else data.indices.insert(data.indices.end(),{base,k,k+1});
        }
    }
    return data;
}
}
