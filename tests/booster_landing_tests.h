#pragma once

std::vector<math3d::vec3> boosterTestPoints() {
    using namespace math3d;
    std::vector<vec3> points;
    const auto add=[&](const MeshData& mesh,const vec3& position,const vec3& scale) {
        const auto local=transform(position,scale);
        for (const auto& vertex:mesh.vertices)
            points.push_back(transform_point(local,{vertex.x,vertex.y,vertex.z}));
    };
    add(geometry::frustum(1),{0,3.4,0},{0.52,4.5,0.52});
    add(geometry::frustum(0),{0,6.15,0},{0.52,1.0,0.52});
    add(geometry::frustum(0.58f),{0,0.8,0},{0.68,0.7,0.68});
    add(geometry::frustum(1),{0,1.45,0},{0.53,0.32,0.53});
    add(geometry::frustum(1),{0,5.54,0},{0.58,0.20,0.58});
    add(geometry::frustum(0.58f),{0,0.30,0},{0.32,0.42,0.32});
    add(geometry::box(),{0.49,1.2,0},{0.65,1.25,0.10});
    for (double height:{2.45,5.3})
        add(geometry::box(),{-0.50,height,0},{0.14,0.24,0.20});
    return points;
}

void landingFootprintTests() {
    using namespace math3d;
    struct Zone { double minX,maxX,minZ,maxZ; };
    const Zone zones[]={{10,22,3.5,10},{-10,-3.5,10,22},
                        {-22,-10,-10,-3.5},{3.5,7.8,-22,-10}};
    const auto points=boosterTestPoints();
    Rocket rocket;
    rocket.start();rocket.update(1000);
    for (std::size_t i=0;i<Rocket::boosterCount;++i) {
        const auto model=rocket.boosterModel(i);
        const auto& zone=zones[i];
        double minX=1000,maxX=-1000,minZ=1000,maxZ=-1000;
        for (const auto& local:points) {
            const auto point=transform_point(model,local);
            minX=std::min(minX,point.x);maxX=std::max(maxX,point.x);
            minZ=std::min(minZ,point.z);maxZ=std::max(maxZ,point.z);
        }
        expect(minX>=zone.minX && maxX<=zone.maxX && minZ>=zone.minZ && maxZ<=zone.maxZ,
               "the whole booster lands in clear terrain away from the pad and site structures");
    }
}

void boosterImpactTests() {
    using namespace math3d;
    const double ground=-0.025;
    const double release=4.2+std::sqrt(2.0*54.0/1.1);
    const auto points=boosterTestPoints();
    const auto lowest=[&](const mat4& pose) {
        double value=1000;
        for (const auto& point:points) value=std::min(value,transform_point(pose,point).y);
        return value;
    };
    Rocket fine;
    fine.start();fine.update(release+3.65);
    expect(fine.boostersSeparated() && !fine.boostersLanded() && fine.boostersVisible(),
           "the boosters approach the ground after separation");
    std::array<mat4,Rocket::boosterCount> previous,contact;
    for (std::size_t i=0;i<Rocket::boosterCount;++i) previous[i]=fine.boosterModel(i);
    bool touched=false;
    double age=3.65;
    while (!fine.boostersLanded() && age<5.0) {
        age+=0.01;fine.update(0.01);
        Rocket coarse;coarse.start();coarse.update(release+age);
        const bool touching=lowest(fine.boosterModel(0))<ground+0.1;
        for (std::size_t i=0;i<Rocket::boosterCount;++i) {
            const auto pose=fine.boosterModel(i);
            const double height=lowest(pose);
            expect(height>=ground-1e-5,"every booster mesh remains above the terrain");
            if (touching) {
                expect(height<ground+0.1,"settling keeps the booster in contact with the terrain");
                if (!touched) contact[i]=pose;
                if (lowest(previous[i])<ground+0.1)
                    expect(length(transform_point(pose,{0,6.65,0})-
                                  transform_point(previous[i],{0,6.65,0}))<0.15,
                           "ground contact and settling have no pose jump");
            }
            for (const vec3 point:{vec3{0,3.2,0},vec3{0,6.65,0},vec3{0.8,1.2,0}})
                expect(length(transform_point(pose,point)-
                              transform_point(coarse.boosterModel(i),point))<1e-4,
                       "landing is independent of the update interval");
            previous[i]=pose;
        }
        touched=touched || touching;
    }
    expect(touched && fine.boostersLanded() && fine.boostersVisible(),
           "all four boosters remain visible after settling finishes");
    expect(fine.stage()==Rocket::Stage::complete && close(fine.altitude(),90) && !fine.enginesOn(),
           "booster settling leaves the completed core and engine state intact");
    Rocket coarse;coarse.start();coarse.update(std::numeric_limits<double>::max());
    std::array<mat4,Rocket::boosterCount> resting;
    for (std::size_t i=0;i<Rocket::boosterCount;++i) {
        resting[i]=fine.boosterModel(i);
        expect(std::abs(resting[i][5])<1e-5,"landed boosters rest horizontally");
        expect(lowest(resting[i])>=ground-1e-5 && lowest(resting[i])<ground+0.02,
               "the resting mesh touches the terrain without sinking");
        expect(length(transform_point(resting[i],{0,6.65,0})-
                      transform_point(contact[i],{0,6.65,0}))>0.1,
               "boosters settle beyond the initial contact pose");
        expect(resting[i]==coarse.boosterModel(i),"a large update reaches the same final resting pose");
    }
    fine.start();fine.update(1000);
    for (std::size_t i=0;i<Rocket::boosterCount;++i)
        expect(fine.boosterModel(i)==resting[i],"landed boosters remain completely still");
    fine.reset();
    Rocket initial;
    expect(!fine.boostersLanded() && !fine.boostersSeparated() && fine.boostersVisible(),
           "reset clears landing and separation");
    for (std::size_t i=0;i<Rocket::boosterCount;++i)
        expect(fine.boosterModel(i)==initial.boosterModel(i),"reset reattaches every booster");
    fine.start();fine.update(1000);
    expect(fine.boostersLanded() && fine.boostersVisible(),"a second launch lands the boosters again");
    for (std::size_t i=0;i<Rocket::boosterCount;++i)
        expect(fine.boosterModel(i)==resting[i],"the second landing repeats the same clear resting positions");
}
