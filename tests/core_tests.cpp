#include "Camera.h"
#include "Geometry.h"
#include "Rocket.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace {
int checks=0;
void expect(bool condition,const char* message) {
    ++checks;
    if (!condition) throw std::runtime_error(message);
}
bool close(double a,double b,double tolerance=1e-5) { return std::abs(a-b)<tolerance; }

void launchTests() {
    Rocket rocket;
    expect(rocket.stage()==Rocket::Stage::ready,"initial state");
    rocket.update(30);
    expect(close(rocket.altitude(),0),"ready rocket remains grounded");
    rocket.start();
    expect(rocket.countdown()==3,"countdown begins at three");
    rocket.update(1.1);
    expect(rocket.countdown()==2,"countdown advances");
    rocket.start();
    expect(rocket.countdown()==2,"repeated start is ignored");
    rocket.update(1.9);
    expect(rocket.stage()==Rocket::Stage::ignition,"countdown transitions to ignition");
    expect(rocket.enginesOn(),"engines are on during ignition");
    rocket.update(1.2);
    expect(rocket.stage()==Rocket::Stage::ascent,"ignition transitions to ascent");
    rocket.update(2);
    expect(close(rocket.altitude(),2.2),"time-based altitude");
    expect(close(rocket.velocity(),2.2),"time-based velocity");
    rocket.reset();
    expect(rocket.stage()==Rocket::Stage::ready && close(rocket.altitude(),0),"reset during ascent");
    expect(!rocket.enginesOn() && close(rocket.velocity(),0),"reset clears engine and velocity");
    rocket.start(); rocket.update(1); rocket.reset();
    expect(rocket.countdown()==0,"reset during countdown");
    rocket.start(); rocket.update(3.5); rocket.reset();
    expect(!rocket.enginesOn(),"reset during ignition");
    rocket.start(); rocket.update(1000);
    expect(rocket.stage()==Rocket::Stage::complete && close(rocket.altitude(),90),"ascent terminates");
    expect(!rocket.enginesOn() && close(rocket.velocity(),0),"complete state is stationary");
    rocket.update(10);
    expect(close(rocket.altitude(),90),"complete state holds");
    rocket.reset(); rocket.start(); rocket.update(5.2);
    Rocket manyFrames; manyFrames.start();
    for (int i=0; i<520; ++i) manyFrames.update(0.01);
    expect(close(rocket.altitude(),manyFrames.altitude()),"launch is independent of update frequency");
    const double oldAltitude=rocket.altitude();
    rocket.update(-1); rocket.update(std::numeric_limits<double>::quiet_NaN());
    expect(close(rocket.altitude(),oldAltitude),"invalid update durations are ignored");
}

void transformTests() {
    using namespace math3d;
    const vec3 localPoint{0.4,1,0};
    const mat4 child=transform({1.38,3.4,0},{0.52,4.5,0.52});
    const mat4 parent=translation({0,20.65,0});
    const vec3 before=transform_point(child,localPoint);
    const vec3 after=transform_point(multiply(parent,child),localPoint);
    expect(close(after.y-before.y,20.65),"parent height propagates to booster vertices");
    expect(close(after.x,before.x)&&close(after.z,before.z),"parent preserves booster offsets");
    const mat4 segment=beam({1,2,3},{4,7,9},0.2);
    const vec3 bottom=transform_point(segment,{0,-0.5,0});
    const vec3 top=transform_point(segment,{0,0.5,0});
    expect(length(bottom-vec3{1,2,3})<1e-5 && length(top-vec3{4,7,9})<1e-5,"beam endpoints");
}

void cameraTests() {
    Camera one,many;
    one.move(1,0,0,1,false);
    for (int i=0; i<100; ++i) many.move(1,0,0,0.01,false);
    expect(math3d::length(one.position()-many.position())<1e-5,"camera speed is frame-rate independent");
    one.move(0,0,-1,100,false);
    expect(one.position().y>=0.8,"camera cannot descend below ground");
    one.look(1e6,1e6);
    for (float f:one.view()) expect(std::isfinite(f),"extreme mouse input keeps view finite");
    one.home(); Camera initial;
    expect(math3d::length(one.position()-initial.position())<1e-5,"home restores camera");

    one.move(0,1,0,0.5,false);
    one.look(70,-20);
    const auto freeView = one.view();
    const math3d::vec3 onPad{0,6,0}, ascending{0,26,0};
    one.toggleChase(onPad);
    const auto initialChasePosition = one.position();
    const auto centered = math3d::transform_point(one.view(), onPad);
    expect(one.chasing() && close(centered.x,0) && close(centered.y,0) && centered.z<0,
           "chase view centers the rocket on entry");
    one.follow(ascending,0.2);
    const double rise = one.position().y-initialChasePosition.y;
    expect(rise>0 && rise<20,"chase view follows ascent smoothly");
    many.toggleChase(onPad);
    for (int i=0; i<20; ++i) many.follow(ascending,0.01);
    expect(math3d::length(one.position()-many.position())<1e-5,
           "chase smoothing uses elapsed time");
    one.move(1,1,1,0.5,true);
    one.look(60,30);
    one.toggleChase(ascending);
    expect(!one.chasing() && one.view()==freeView,"leaving chase restores the original free view");
    one.toggleChase(ascending);
    one.home();
    expect(!one.chasing() && one.view()==initial.view(),"home resets chase to the initial free view");

    const auto rest = one.position();
    one.updateIgnitionShake(false,0.08);
    expect(one.view()==initial.view(),"camera stays steady before ignition");
    one.updateIgnitionShake(true,0.08);
    const double vibration = math3d::length(one.position()-rest);
    expect(vibration>0.01 && vibration<0.4,"ignition shake adds a small visible offset");
    one.updateIgnitionShake(true,1.0);
    expect(math3d::length(one.position()-rest)<vibration,"ignition shake fades during ignition");
    one.updateIgnitionShake(false,0.02);
    expect(one.view()==initial.view(),"leaving ignition clears shake without camera drift");

    one.toggleChase(onPad);
    const auto steadyChase = one.view();
    one.updateIgnitionShake(true,0.08);
    expect(one.view()!=steadyChase,"ignition shake also works in chase mode");
    one.home();
    expect(!one.chasing() && one.view()==initial.view(),"reset clears ignition shake and chase mode");
}

void meshTest(const MeshData& mesh) {
    using namespace math3d;
    expect(!mesh.vertices.empty() && mesh.indices.size()%3==0,"valid triangle mesh");
    for (const auto& v:mesh.vertices) {
        expect(close(length({v.nx,v.ny,v.nz}),1,1e-4),"unit mesh normals");
        expect(v.u>=0 && v.u<=1 && v.v>=0 && v.v<=1,"uv coordinates in range");
    }
    for (std::size_t i=0; i<mesh.indices.size(); i+=3) {
        for (int j=0; j<3; ++j) expect(mesh.indices[i+j]<mesh.vertices.size(),"index is in range");
        const auto a=mesh.vertices[mesh.indices[i]],b=mesh.vertices[mesh.indices[i+1]],c=mesh.vertices[mesh.indices[i+2]];
        const auto geometric=cross(vec3{b.x-a.x,b.y-a.y,b.z-a.z},vec3{c.x-a.x,c.y-a.y,c.z-a.z});
        expect(length(geometric)>1e-8,"triangle is non-degenerate");
        const vec3 normal{a.nx+b.nx+c.nx,a.ny+b.ny+c.ny,a.nz+b.nz+c.nz};
        expect(dot(geometric,normal)>0,"winding agrees with outward normal");
    }
}
}

int main() {
    try {
        launchTests(); transformTests(); cameraTests();
        meshTest(geometry::box()); meshTest(geometry::plane());
        meshTest(geometry::frustum(1)); meshTest(geometry::frustum(0)); meshTest(geometry::frustum(0.58f));
        std::cout<<checks<<" checks passed: launch, hierarchy, camera, geometry\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr<<"test failed: "<<e.what()<<"\n";
        return 1;
    }
}
