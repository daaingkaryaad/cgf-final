#include "Scene.h"
#include <algorithm>

using math3d::vec3;
namespace {
const vec3 cream{0.87,0.86,0.77}, orange{0.82,0.25,0.085}, dark{0.12,0.18,0.18};
const vec3 steel{0.27,0.34,0.33}, yellow{0.78,0.53,0.14};
}

Scene::Scene(const std::string& directory)
    : shader_(directory+"/basic.vert",directory+"/basic.frag"),
      sky_(directory+"/sky.vert",directory+"/sky.frag") {
    meshes_[box].reset(new Mesh(geometry::box()));
    meshes_[plane].reset(new Mesh(geometry::plane()));
    meshes_[cylinder].reset(new Mesh(geometry::frustum(1.0f)));
    meshes_[cone].reset(new Mesh(geometry::frustum(0.0f)));
    meshes_[taper].reset(new Mesh(geometry::frustum(0.58f)));
    glGenVertexArrays(1,&skyVao_);
    buildSite();
    buildRocket();
}

Scene::~Scene() { glDeleteVertexArrays(1,&skyVao_); }

void Scene::add(std::vector<Part>& list, Shape shape, const vec3& pos,
                const vec3& scale, const vec3& colour, float specular,
                float repeat, double yaw, bool emissive) {
    list.push_back({shape,math3d::transform(pos,scale,yaw),colour,specular,repeat,emissive});
}

void Scene::buildSite() {
    add(site_,plane,{0,-0.025,0},{480,1,480},{0.54,0.47,0.32},0.0f);
    add(site_,box,{0,0.01,32},{6,0.07,50},{0.34,0.35,0.30},0.03f,6);
    add(site_,box,{0,0.10,0},{17,0.20,17},{0.8,0.79,0.72},0.05f,5);
    add(site_,cylinder,{0,0.33,0},{6.3,0.46,6.3},{0.74,0.75,0.70},0.05f,4);
    add(site_,box,{0,0.575,0},{3.4,0.04,5.4},dark,0.1f);
    for (int i=0; i<16; ++i) {
        const double a=2.0*math3d::pi*i/16;
        add(site_,box,{5.45*std::sin(a),0.576,5.45*std::cos(a)},
            {0.6,0.025,0.18},yellow,0.0f,0,a);
    }
    for (int i=0; i<6; ++i)
        add(site_,box,{0,0.055,12.0+i*7},{0.12,0.015,2.5},{0.83,0.77,0.57},0);
    const double tx=-6.7, tz=-2.3;
    add(site_,box,{tx,0.3,tz},{4.3,0.6,4.3},{0.65,0.64,0.56},0.05f,2);
    for (int x=-1; x<=1; x+=2) for (int z=-1; z<=1; z+=2)
        add(site_,box,{tx+x*1.2,7.3,tz+z*1.2},{0.26,14,0.26},steel,0.5f);
    for (int level=0; level<6; ++level) {
        const double y=0.6+level*2.7;
        add(site_,box,{tx,y,tz},{2.9,0.15,2.9},steel,0.4f);
        if (level==5) continue;
        for (int side=-1; side<=1; side+=2) {
            const vec3 a{tx-1.2,y,tz+side*1.2}, b{tx+1.2,y+2.7,tz+side*1.2};
            site_.push_back({box,math3d::beam(a,b,0.13),yellow,0.3f,0,false});
            site_.push_back({box,math3d::beam({b.x,y,b.z},{a.x,y+2.7,a.z},0.13),steel,0.3f,0,false});
            site_.push_back({box,math3d::beam({tx+side*1.2,y,tz-1.2},
                {tx+side*1.2,y+2.7,tz+1.2},0.13),steel,0.3f,0,false});
        }
    }
    add(site_,box,{-3.7,10.4,-2.3},{3.4,0.22,0.9},yellow,0.3f);
    add(site_,box,{tx,14.4,tz},{3.2,0.22,3.2},steel,0.4f);
    add(site_,cylinder,{tx,15.3,tz},{0.045,1.7,0.045},cream,0.2f);
    add(site_,cone,{tx,16.25,tz},{0.13,0.25,0.13},orange,0,0,0,true);
    add(site_,box,{12,1.75,-11},{7,3.5,5},cream,0.08f);
    add(site_,box,{12,3.6,-11},{7.5,0.25,5.5},steel,0.2f);
    add(site_,box,{10.2,1.2,-8.485},{1.3,2.4,0.04},dark,0.2f);
    for (int i=0; i<3; ++i)
        add(site_,box,{12.0+i*1.1,2,-8.47},{0.8,0.9,0.06},{0.15,0.32,0.36},0.9f);
    add(site_,box,{13.5,3.95,-11},{2,0.5,1.5},{0.55,0.59,0.54},0.3f);
    for (int i=0; i<2; ++i) {
        add(site_,cylinder,{14+i*3.0,2.5,0},{1.15,5,1.15},cream,0.65f);
        add(site_,taper,{14+i*3.0,5.2,0},{1.15,0.4,1.15},steel,0.6f);
        add(site_,cylinder,{14+i*3.0,1.0,0},{1.17,0.14,1.17},orange,0.25f);
    }
    add(site_,box,{9,0.3,0},{8,0.15,0.15},steel,0.6f);
    add(site_,cylinder,{8,3.5,5},{0.1,7,0.1},steel,0.5f);
    add(site_,box,{8,7.1,5},{1.1,0.5,0.5},dark,0.3f);
    for (int i=0; i<100; ++i) {
        const double x=((i*73)%197)-98, z=((i*47)%193)-96;
        if ((std::abs(x)<23 && std::abs(z)<23) || (std::abs(x)<5 && z>0)) continue;
        add(site_,cone,{x,0.18,z},{0.33,0.36,0.28},{0.34,0.36,0.20},0);
    }
    for (int i=0; i<13; ++i)
        add(site_,cone,{-160.0+i*28,-1.5,-130.0-((i*11)%25)},
            {30.0+((i*7)%12),9.0+((i*13)%8),24},{0.43,0.45,0.35},0);
}

void Scene::buildRocket() {
    add(rocket_,cylinder,{0,5.25,0},{0.86,6.6,0.86},cream,0.7f);
    add(rocket_,cylinder,{0,8.55,0},{0.87,0.22,0.87},orange,0.4f);
    add(rocket_,cone,{0,9.8,0},{0.86,2.3,0.86},cream,0.7f);
    add(rocket_,taper,{0,1.5,0},{1.05,0.9,1.05},steel,0.6f);
    add(rocket_,cylinder,{0,2.05,0},{0.88,0.20,0.88},orange,0.5f);
    add(rocket_,taper,{0,0.62,0},{0.48,0.72,0.48},dark,0.8f);
    for (int i=0; i<4; ++i) {
        const double a=i*math3d::pi*0.5;
        const auto booster=math3d::multiply(math3d::translation({1.38*std::cos(a),0,1.38*std::sin(a)}),math3d::rotation_y(-a));
        const std::size_t first=rocket_.size();
        add(rocket_,cylinder,{0,3.4,0},{0.52,4.5,0.52},cream,0.65f);
        add(rocket_,cone,{0,6.15,0},{0.52,1.0,0.52},cream,0.65f);
        add(rocket_,taper,{0,0.8,0},{0.68,0.7,0.68},steel,0.55f);
        add(rocket_,cylinder,{0,1.45,0},{0.53,0.32,0.53},orange,0.35f);
        add(rocket_,taper,{0,0.30,0},{0.32,0.42,0.32},dark,0.8f);
        add(rocket_,box,{0.49,1.2,0},{0.65,1.25,0.10},steel,0.45f);
        for (std::size_t j=first; j<rocket_.size(); ++j)
            rocket_[j].local=math3d::multiply(booster,rocket_[j].local);
    }
    add(rocket_,box,{0,5.6,0.861},{0.16,2.6,0.025},orange,0.2f);
}

void Scene::draw(const Part& part, const math3d::mat4& parent) {
    shader_.matrix("model",math3d::multiply(parent,part.local));
    shader_.vector("baseColour",part.colour);
    shader_.scalar("specularStrength",part.specular);
    shader_.scalar("textureRepeat",part.textureRepeat);
    shader_.integer("emissive",part.emissive ? 1 : 0);
    meshes_[part.shape]->draw();
}

void Scene::render(const math3d::mat4& view, const math3d::mat4& projection,
                   const vec3& eye, const Rocket& rocket, bool padLight) {
    // fit the transition from 8 to 48 scene units into the short demo ascent.
    const float ascent = std::max(0.0f,std::min(1.0f,float((rocket.altitude()-8.0)/40.0)));
    const float skyDarkness = ascent*ascent*(3.0f-2.0f*ascent);
    const vec3 darkHorizon{0.095,0.145,0.235};
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    sky_.use();
    sky_.scalar("skyDarkness",skyDarkness);
    sky_.vector("darkHorizon",darkHorizon);
    glBindVertexArray(skyVao_);
    glDrawArrays(GL_TRIANGLES,0,3);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    shader_.use();
    shader_.matrix("view",view);
    shader_.matrix("projection",projection);
    shader_.vector("eyePosition",eye);
    shader_.scalar("skyDarkness",skyDarkness);
    shader_.vector("darkHorizon",darkHorizon);
    shader_.vector("sunDirection",math3d::normalized({-0.5,0.8,0.35}));
    shader_.vector("padLightPosition",{8,7.0,4.7});
    shader_.integer("padLightOn",padLight ? 1 : 0);
    shader_.integer("surfaceTexture",0);
    concrete_.bind();
    const auto identity=math3d::identity();
    for (const auto& part : site_) draw(part,identity);
    for (const auto& part : rocket_) draw(part,rocket.model());
    Part lens{box,math3d::transform({8,7.1,4.74},{0.85,0.3,0.03}),
        padLight ? vec3{1,0.82,0.40} : vec3{0.2,0.21,0.18},0.1f,0,padLight};
    draw(lens,identity);
    if (rocket.enginesOn()) {
        Part nozzle{cylinder,math3d::transform({0,0.263,0},{0.39,0.018,0.39}),{1,0.55,0.12},0,0,true};
        draw(nozzle,rocket.model());
        for (int i=0; i<4; ++i) {
            const double a=i*math3d::pi*0.5;
            nozzle.local=math3d::transform({1.38*std::cos(a),0.08,1.38*std::sin(a)},{0.27,0.018,0.27});
            draw(nozzle,rocket.model());
        }
    }
}
