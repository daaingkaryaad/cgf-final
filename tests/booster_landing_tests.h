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
