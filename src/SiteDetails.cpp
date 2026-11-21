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

void Scene::buildVehicle() {
    add(site_,plane,{19.5,-0.019,-4.2},{6.0,1,4.2},{0.60,0.58,0.49},0.02f,3);
    for (double x:{16.65,22.35})
        add(site_,box,{x,-0.005,-4.2},{0.07,0.015,3.9},cream,0);
    std::vector<Part> van;
    add(van,box,{0,0.47,0},{4.0,0.20,1.76},dark,0.35f);
    add(van,box,{-0.35,1.03,0},{3.1,1.15,1.68},cream,0.45f);
    add(van,box,{1.43,0.83,0},{0.96,0.70,1.68},cream,0.45f);
    add(van,box,{0.64,1.66,0},{1.24,0.12,1.78},steel,0.45f);
    add(van,box,{1.27,1.38,0},{0.025,0.45,1.42},glass,0.9f);
    add(van,box,{-0.8,1.67,0},{0.6,0.10,0.16},yellow,0.3f);
    for (int side:{-1,1}) {
        add(van,box,{0.61,1.38,side*0.849},{1.20,0.45,0.025},glass,0.9f);
        add(van,box,{-0.1,0.95,side*0.855},{3.65,0.13,0.025},orange,0.3f);
        add(van,box,{-0.49,1.14,side*0.858},{0.025,0.84,0.025},steel,0.3f);
        add(van,box,{-0.26,1.19,side*0.882},{0.21,0.045,0.025},steel,0.6f);
        add(van,box,{1.93,0.92,side*0.60},{0.035,0.17,0.28},cream,0.5f);
        add(van,box,{-1.92,0.79,side*0.63},{0.035,0.21,0.16},orange,0.3f);
        for (double x:{-1.24,1.24}) {
            const double z=side*0.91;
            addBeam(van,cylinder,{x,0.37,z-0.13},{x,0.37,z+0.13},0.37,rubber,0.08f);
            const double outer=z+side*0.14;
            addBeam(van,cylinder,{x,0.37,outer-0.015},{x,0.37,outer+0.015},0.17,steel,0.65f);
        }
    }
    for (int end:{-1,1})
        add(van,box,{end*1.99,0.52,0},{0.13,0.20,1.84},steel,0.45f);
    add(van,box,{1.93,0.69,0},{0.035,0.19,0.68},dark,0.2f);
    appendParts(site_,van,math3d::transform({20,-0.025,-4.2},{1,1,1},-0.08));
}

void Scene::buildServiceArm() {
    add(site_,box,{-5.40,10.4,-2.3},{0.12,0.65,0.75},steel,0.4f);
    add(arm_,cylinder,{0,0,0},{0.20,0.55,0.20},dark,0.55f);
    add(arm_,box,{2.9,0,0},{5.8,0.22,0.65},yellow,0.35f);
    add(arm_,box,{2.9,0.14,0},{5.7,0.055,0.57},steel,0.3f);
    for (int side:{-1,1}) {
        addBeam(arm_,box,{0.3,0.45,side*0.29},{5.6,0.45,side*0.29},0.055,yellow,0.3f);
        for (double x:{0.4,2.1,3.8,5.5})
            addBeam(arm_,box,{x,0.15,side*0.29},{x,0.46,side*0.29},0.045,steel,0.3f);
    }
    add(arm_,box,{5.77,0.06,0},{0.17,0.35,0.59},dark,0.35f);
}

void Scene::buildWorkers() {
    const vec3 positions[]={{9.1,-0.025,-5.3},{9.6,-0.025,-6.7},
                            {10.7,-0.025,-4.4},{21,-0.025,-2.2}};
    const double angles[]={-0.9,0.5,-1.2,2.5};
    for (int i=0;i<4;++i) {
        std::vector<Part> worker;
        const vec3 jacket=i==1 ? yellow : orange;
        for (int side:{-1,1}) {
            add(worker,box,{side*0.11,0.065,0.045},{0.18,0.13,0.29},rubber,0.05f);
            add(worker,cylinder,{side*0.11,0.43,0},{0.085,0.60,0.085},dark,0.05f);
            const vec3 shoulder{side*0.27,1.17,0};
            const vec3 hand=(i==0 && side==1) ? vec3{0.42,1.57,0.04} :
                            (i==1 ? vec3{side*0.16,0.99,0.28} : vec3{side*0.33,0.81,0.07});
            addBeam(worker,cylinder,shoulder,hand,0.075,jacket,0.05f);
            add(worker,box,hand,{0.11,0.12,0.10},{0.62,0.44,0.30},0.05f);
        }
        add(worker,box,{0,0.73,0},{0.37,0.18,0.25},dark,0.05f);
        add(worker,box,{0,1.02,0},{0.44,0.56,0.28},jacket,0.08f);
        for (int side:{-1,1})
            add(worker,box,{0,1.10,side*0.145},{0.45,0.055,0.015},cream,0.15f);
        add(worker,cylinder,{0,1.43,0},{0.14,0.25,0.14},{0.62,0.44,0.30},0.06f);
        add(worker,cylinder,{0,1.54,0},{0.22,0.045,0.20},cream,0.3f);
        add(worker,taper,{0,1.62,0},{0.18,0.15,0.17},cream,0.3f);
        if (i==1) add(worker,box,{0,1.0,0.31},{0.31,0.35,0.035},steel,0.1f);
        appendParts(workers_,worker,math3d::transform(positions[i],{1,1,1},angles[i]));
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
