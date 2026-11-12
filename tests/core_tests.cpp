#include "Camera.h"
#include "Geometry.h"
#include "Rocket.h"
#include <array>
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

void boosterTests() {
    using namespace math3d;
    const double separation = 4.2+std::sqrt(2.0*54.0/1.1);
    const vec3 pivot{0,3.2,0};
    const std::array<vec3,Rocket::boosterCount> directions{{
        {1,0,0}, {0,0,1}, {-1,0,0}, {0,0,-1}
    }};
    Rocket rocket;
    std::array<mat4,Rocket::boosterCount> initial;
    std::array<vec3,Rocket::boosterCount> before;
    expect(!rocket.boostersSeparated() && rocket.boostersVisible(),"boosters are attached and visible on the pad");
    const auto pad = transform_point(rocket.model(),pivot);
    for (std::size_t i=0; i<Rocket::boosterCount; ++i) {
        initial[i] = rocket.boosterModel(i);
        const auto center = transform_point(initial[i],pivot);
        expect(length(center-pad-directions[i]*1.38)<1e-5,"each booster sits at its cardinal mounting point");
        const auto fin = transform_point(initial[i],pivot+vec3{1,0,0});
        expect(length(fin-center-directions[i])<1e-5,"each booster's local positive x points outward");
    }
    rocket.start();
    rocket.update(separation-0.001);
    expect(!rocket.boostersSeparated() && rocket.boostersVisible(),"boosters stay attached below separation altitude");
    for (std::size_t i=0; i<Rocket::boosterCount; ++i) {
        before[i] = transform_point(rocket.boosterModel(i),pivot);
        const auto expected = transform_point(initial[i],pivot)+vec3{0,rocket.altitude(),0};
        expect(length(before[i]-expected)<1e-5,"all attached boosters follow the core");
    }
    rocket.update(0.002);
    expect(rocket.boostersSeparated(),"boosters separate at 54 scene units");
    for (std::size_t i=0; i<Rocket::boosterCount; ++i)
        expect(length(transform_point(rocket.boosterModel(i),pivot)-before[i])<0.03,
               "separation preserves each booster's center");
    rocket.update(0.8);
    const auto drifting = transform_point(rocket.boosterModel(0),pivot);
    for (std::size_t i=0; i<Rocket::boosterCount; ++i) {
        const auto center = transform_point(rocket.boosterModel(i),pivot);
        expect(dot(center-before[i],directions[i])>3,"each booster drifts away from the core");
    }
    expect(std::abs(transform_point(rocket.boosterModel(0),{0,5.8,0}).x-drifting.x)>1,
           "detached booster tumbles around its center");
    rocket.update(1.7);
    expect(transform_point(rocket.boosterModel(0),pivot).y<drifting.y,"detached booster falls while core rises");
    rocket.update(0.6);
    expect(rocket.stage()==Rocket::Stage::complete && rocket.boostersVisible(),"boosters keep falling after ascent ends");
    const auto atCoreStop = transform_point(rocket.boosterModel(0),pivot);
    rocket.update(0.9);
    expect(rocket.boostersSeparated() && rocket.boostersVisible(),"boosters stay detached during the final fall");
    expect(transform_point(rocket.boosterModel(0),pivot).y<atCoreStop.y && close(rocket.altitude(),90),
           "booster motion continues while the core is stationary");
    rocket.update(0.2);
    expect(!rocket.boostersVisible(),"boosters leave the scene after falling below the terrain");
    rocket.reset();
    expect(!rocket.boostersSeparated() && rocket.boostersVisible(),"reset restores attached booster visibility");
    for (std::size_t i=0; i<Rocket::boosterCount; ++i)
        expect(rocket.boosterModel(i)==initial[i],"reset restores each booster's pad transform");
    rocket.start();
    rocket.update(separation+0.3);
    expect(rocket.boostersSeparated() && rocket.boostersVisible(),"relaunch separates boosters again");
    Rocket fresh;
    fresh.start();
    fresh.update(separation+0.3);
    for (std::size_t i=0; i<Rocket::boosterCount; ++i)
        expect(rocket.boosterModel(i)==fresh.boosterModel(i),"relaunch does not retain previous booster motion");
}

void boosterTimingTests() {
    using namespace math3d;
    const double release = 4.2+std::sqrt(2.0*54.0/1.1);
    const double justBefore = std::nextafter(release,0.0);
    Rocket boundary;
    boundary.start();
    boundary.update(justBefore);
    expect(!boundary.boostersSeparated(),"a coarse update just below the threshold does not separate boosters");
    std::array<mat4,Rocket::boosterCount> attached;
    for (std::size_t i=0; i<Rocket::boosterCount; ++i) attached[i] = boundary.boosterModel(i);
    boundary.update(release-justBefore);
    expect(boundary.boostersSeparated() && close(boundary.altitude(),54),"boosters separate at the exact launch time");
    for (std::size_t i=0; i<Rocket::boosterCount; ++i) {
        for (const vec3 point : {vec3{0,0,0}, vec3{0,6.65,0}, vec3{0.8,1.2,0}})
            expect(length(transform_point(boundary.boosterModel(i),point)-transform_point(attached[i],point))<1e-5,
                   "the base, tip and fin have no pose jump at release");
    }

    Rocket oneStep;
    oneStep.start();
    oneStep.update(release+3.5);
    boundary.update(3.5);
    expect(oneStep.stage()==Rocket::Stage::complete && oneStep.boostersSeparated() && oneStep.boostersVisible(),
           "a single long update can reach a stationary core with falling boosters");
    for (std::size_t i=0; i<Rocket::boosterCount; ++i)
        expect(length(transform_point(oneStep.boosterModel(i),{0,6.65,0})-
                      transform_point(boundary.boosterModel(i),{0,6.65,0}))<1e-5,
               "a long update consumes only the time after release for booster motion");
    oneStep.update(std::numeric_limits<double>::max());
    expect(oneStep.boostersSeparated() && !oneStep.boostersVisible() && close(oneStep.altitude(),90),
           "a huge finite update finishes booster flight without changing core altitude");
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
        launchTests(); boosterTests(); boosterTimingTests(); transformTests(); cameraTests();
        meshTest(geometry::box()); meshTest(geometry::plane());
        meshTest(geometry::frustum(1)); meshTest(geometry::frustum(0)); meshTest(geometry::frustum(0.58f));
        std::cout<<checks<<" checks passed: launch, hierarchy, camera, geometry\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr<<"test failed: "<<e.what()<<"\n";
        return 1;
    }
}
