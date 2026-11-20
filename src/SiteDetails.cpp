#include "Scene.h"

using math3d::vec3;
namespace {
const vec3 cream{0.87,0.86,0.77}, orange{0.82,0.25,0.085}, dark{0.12,0.18,0.18};
const vec3 steel{0.27,0.34,0.33}, yellow{0.78,0.53,0.14};
const vec3 rubber{0.075,0.085,0.08}, glass{0.12,0.25,0.29};
}

void Scene::addBeam(std::vector<Part>& list, Shape shape, const vec3& a,
                    const vec3& b, double radius, const vec3& colour, float specular) {
    list.push_back({shape,math3d::beam(a,b,radius),colour,specular,0,false});
}

void Scene::appendParts(std::vector<Part>& target, const std::vector<Part>& parts,
                        const math3d::mat4& parent) {
    for (auto part:parts) {
        part.local=math3d::multiply(parent,part.local);
        target.push_back(part);
    }
}

void Scene::buildEquipment() {
    const vec3 wood{0.48,0.32,0.15}, band{0.23,0.26,0.23};
    for (const vec3 center:{vec3{11.6,0.325,-1.8},vec3{11.6,0.325,-2.6},vec3{11.6,1.025,-1.8}}) {
        add(site_,box,center,{0.72,0.70,0.72},wood,0.05f);
        for (double offset:{-0.23,0.23}) {
            add(site_,box,center+vec3{offset,0.355,0},{0.055,0.02,0.74},band,0.25f);
            for (int side:{-1,1})
                add(site_,box,center+vec3{offset,0,side*0.366},{0.055,0.70,0.02},band,0.25f);
        }
    }
    for (double z:{-2.3,-3.7}) {
        addBeam(site_,cylinder,{9.7,0.47,z-0.25},{9.7,0.47,z+0.25},0.38,dark,0.1f);
        for (int side:{-1,1}) {
            addBeam(site_,cylinder,{9.7,0.47,z+side*0.28-0.035},{9.7,0.47,z+side*0.28+0.035},0.49,wood,0.08f);
            addBeam(site_,cylinder,{9.7,0.47,z+side*0.32-0.01},{9.7,0.47,z+side*0.32+0.01},0.12,steel,0.5f);
        }
        for (double offset:{-0.14,0.0,0.14})
            addBeam(site_,cylinder,{9.7,0.47,z+offset-0.025},{9.7,0.47,z+offset+0.025},0.395,rubber,0.05f);
    }
    std::vector<Part> cart;
    add(cart,box,{0,0.39,0},{1.4,0.12,0.82},steel,0.3f);
    add(cart,box,{0,0.47,0},{1.27,0.045,0.70},wood,0.08f);
    for (int side:{-1,1}) {
        for (double x:{-0.48,0.48})
            addBeam(cart,cylinder,{x,0.16,side*0.37-0.055},{x,0.16,side*0.37+0.055},0.16,rubber,0.05f);
        addBeam(cart,box,{0.59,0.42,side*0.32},{0.93,0.96,side*0.32},0.055,steel,0.3f);
    }
    addBeam(cart,box,{0.93,0.96,-0.32},{0.93,0.96,0.32},0.065,dark,0.15f);
    add(cart,box,{-0.16,0.67,0},{0.48,0.36,0.48},orange,0.1f);
    appendParts(site_,cart,math3d::transform({10,-0.025,-0.5},{1,1,1},0.1));
}
