#include "Hud.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <iomanip>
#include <sstream>

namespace {
std::array<unsigned char,7> glyph(char c) {
    switch(c) {
        case 'a': return {{0,0,14,1,15,17,15}};
        case 'b': return {{16,16,30,17,17,17,30}};
        case 'c': return {{0,0,14,16,16,17,14}};
        case 'd': return {{1,1,15,17,17,17,15}};
        case 'e': return {{0,0,14,17,31,16,14}};
        case 'f': return {{6,8,8,30,8,8,8}};
        case 'g': return {{0,15,17,17,15,1,14}};
        case 'h': return {{16,16,30,17,17,17,17}};
        case 'i': return {{4,0,12,4,4,4,14}};
        case 'j': return {{2,0,6,2,2,18,12}};
        case 'k': return {{16,16,18,20,24,20,18}};
        case 'l': return {{12,4,4,4,4,4,14}};
        case 'm': return {{0,0,26,21,21,21,21}};
        case 'n': return {{0,0,30,17,17,17,17}};
        case 'o': return {{0,0,14,17,17,17,14}};
        case 'p': return {{0,30,17,17,30,16,16}};
        case 'q': return {{0,15,17,17,15,1,1}};
        case 'r': return {{0,0,22,25,16,16,16}};
        case 's': return {{0,0,15,16,14,1,30}};
        case 't': return {{8,8,30,8,8,9,6}};
        case 'u': return {{0,0,17,17,17,19,13}};
        case 'v': return {{0,0,17,17,17,10,4}};
        case 'w': return {{0,0,17,17,21,21,10}};
        case 'x': return {{0,0,17,10,4,10,17}};
        case 'y': return {{0,17,17,17,15,1,14}};
        case 'z': return {{0,0,31,2,4,8,31}};
        case '0': return {{14,17,19,21,25,17,14}};
        case '1': return {{4,12,4,4,4,4,14}};
        case '2': return {{14,17,1,2,4,8,31}};
        case '3': return {{30,1,1,14,1,1,30}};
        case '4': return {{2,6,10,18,31,2,2}};
        case '5': return {{31,16,16,30,1,1,30}};
        case '6': return {{14,16,16,30,17,17,14}};
        case '7': return {{31,1,2,4,8,8,8}};
        case '8': return {{14,17,17,14,17,17,14}};
        case '9': return {{14,17,17,15,1,1,14}};
        case '/': return {{1,2,2,4,8,8,16}};
        case ':': return {{0,4,4,0,4,4,0}};
        case '.': return {{0,0,0,0,0,4,4}};
        case '-': return {{0,0,0,31,0,0,0}};
        case '+': return {{0,4,4,31,4,4,0}};
        case '|': return {{4,4,4,4,4,4,4}};
        default: return {{0,0,0,0,0,0,0}};
    }
}
const math3d::vec3 white{0.88,0.91,0.85}, muted{0.56,0.65,0.62}, amber{1,0.68,0.29};
}

Hud::Hud(const std::string& directory): shader_(directory+"/hud.vert",directory+"/hud.frag") {
    glGenVertexArrays(1,&vao_);
    glGenBuffers(1,&vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Pixel),nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,sizeof(Pixel),reinterpret_cast<const void*>(offsetof(Pixel,r)));
    pixels_.reserve(80000);
}

Hud::~Hud() { glDeleteBuffers(1,&vbo_); glDeleteVertexArrays(1,&vao_); }

void Hud::rect(float x,float y,float w,float h,const math3d::vec3& c,float alpha) {
    const Pixel a{x,y,float(c.x),float(c.y),float(c.z),alpha};
    const Pixel b{x+w,y,a.r,a.g,a.b,alpha}, d{x,y+h,a.r,a.g,a.b,alpha};
    const Pixel e{x+w,y+h,a.r,a.g,a.b,alpha};
    pixels_.insert(pixels_.end(),{a,b,e,a,e,d});
}

void Hud::text(float x,float y,const std::string& value,float size,const math3d::vec3& colour) {
    for (const char c : value) {
        const auto rows=glyph(c);
        for (int row=0; row<7; ++row) for (int col=0; col<5; ++col)
            if (rows[row]&(1<<(4-col))) rect(x+col*size,y+row*size,size,size,colour);
        x+=6*size;
    }
}

void Hud::render(int width,int height,const Rocket& rocket,double fps,
                 bool padLight,bool captured,bool chaseCamera) {
    const float scale=std::min(float(width)/1280.0f,float(height)/720.0f);
    if (scale<=0) return;
    const float virtualWidth=width/scale, virtualHeight=height/scale;
    pixels_.clear();
    const math3d::vec3 panel{0.035,0.075,0.075};
    rect(24,24,365,103,panel,0.92f);
    rect(24,24,3,103,amber);
    text(43,40,"baikonur / launch control",1.8f,white);
    text(43,65,"cgf / week 7 + 8 checkpoint",1.25f,muted);
    text(43,96,"site 01 / kazakhstan",1.3f,amber);
    rect(virtualWidth-257,24,233,103,panel,0.92f);
    std::ostringstream count;
    if (fps>0) count<<std::fixed<<std::setprecision(0)<<fps<<" fps";
    else count<<"measuring fps";
    text(virtualWidth-239,41,count.str(),2,white);
    text(virtualWidth-239,68,padLight ? "pad light / on" : "pad light / off",1.3f,amber);
    text(virtualWidth-239,96,chaseCamera ? "camera / chase" : "camera / free",1.3f,muted);
    rect(24,virtualHeight-214,410,106,panel,0.92f);
    text(43,virtualHeight-198,"mission status",1.2f,muted);
    std::string status=rocket.status();
    if (rocket.stage()==Rocket::Stage::countdown) status="t - 0"+std::to_string(rocket.countdown());
    if (rocket.stage()==Rocket::Stage::complete) status="ascent complete";
    text(43,virtualHeight-174,status,2.1f,amber);
    std::ostringstream telemetry;
    telemetry<<std::fixed<<std::setprecision(1)<<"alt "<<rocket.altitude()<<" m / "<<rocket.velocity()<<" m/s";
    text(43,virtualHeight-139,telemetry.str(),1.4f,white);
    rect(24,virtualHeight-88,virtualWidth-48,64,panel,0.94f);
    text(43,virtualHeight-74,"space launch | r reset scene | l pad light | h home camera | c camera mode",1.4f,white);
    if (chaseCamera) {
        text(43,virtualHeight-49,"tracking rocket | c returns to free view",1.15f,muted);
    } else {
        text(43,virtualHeight-49,"wasd move | q/e down/up | shift faster | mouse look",1.15f,muted);
        text(virtualWidth-372,virtualHeight-49,captured ? "tab/esc release mouse" : "click/tab capture mouse",1.15f,amber);
    }
    if (captured) {
        rect(virtualWidth*0.5f-5,virtualHeight*0.5f,10,1,white,0.6f);
        rect(virtualWidth*0.5f,virtualHeight*0.5f-5,1,10,white,0.6f);
    }
    shader_.use();
    shader_.vector("canvasSize",{virtualWidth,virtualHeight,0});
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(pixels_.size()*sizeof(Pixel)),pixels_.data(),GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES,0,GLsizei(pixels_.size()));
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
