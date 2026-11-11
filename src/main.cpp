#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Hud.h"
#include "Rocket.h"
#include "Scene.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
struct Options {
    bool hidden=false, vsync=true;
    int frames=0;
    double simulate=-1.0;
    std::string capture, shaderDirectory="shaders";
};

Options options(int argc,char** argv) {
    Options out;
    for (int i=1; i<argc; ++i) {
        const std::string flag=argv[i];
        if (flag=="--hidden") out.hidden=true;
        else if (flag=="--no-vsync") out.vsync=false;
        else if (flag=="--frames" && i+1<argc) {
            out.frames=std::stoi(argv[++i]);
            if (out.frames<1) throw std::runtime_error("--frames must be positive");
        } else if (flag=="--simulate" && i+1<argc) {
            out.simulate=std::stod(argv[++i]);
            if (!std::isfinite(out.simulate) || out.simulate<0)
                throw std::runtime_error("--simulate must be finite and non-negative");
        } else if (flag=="--capture" && i+1<argc) out.capture=argv[++i];
        else if (flag=="--shaders" && i+1<argc) out.shaderDirectory=argv[++i];
        else throw std::runtime_error("unknown or incomplete option: "+flag);
    }
    return out;
}

struct State {
    explicit State(Scene& scene) : scene(scene) {}

    Camera camera;
    Rocket rocket;
    Scene& scene;
    bool padLight=true, captured=false, firstMouse=true;
    double mouseX=0,mouseY=0;
};

State& state(GLFWwindow* window) {
    return *static_cast<State*>(glfwGetWindowUserPointer(window));
}

void captureMouse(GLFWwindow* window,bool capture) {
    auto& app=state(window);
    app.captured=capture;
    app.firstMouse=true;
    glfwSetInputMode(window,GLFW_CURSOR,capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void onKey(GLFWwindow* window,int key,int,int action,int) {
    if (action!=GLFW_PRESS) return;
    auto& app=state(window);
    switch(key) {
        case GLFW_KEY_SPACE: app.rocket.start(); break;
        case GLFW_KEY_R:
            app.rocket.reset();
            app.camera.home();
            app.scene.resetExhaust();
            app.padLight=true;
            break;
        case GLFW_KEY_H: app.camera.home(); break;
        case GLFW_KEY_L: app.padLight=!app.padLight; break;
        case GLFW_KEY_TAB: captureMouse(window,!app.captured); break;
        case GLFW_KEY_ESCAPE:
            if (app.captured) captureMouse(window,false);
            else glfwSetWindowShouldClose(window,GLFW_TRUE);
            break;
        default: break;
    }
}

void onMouse(GLFWwindow* window,double x,double y) {
    auto& app=state(window);
    if (!app.captured) return;
    if (app.firstMouse) { app.mouseX=x; app.mouseY=y; app.firstMouse=false; return; }
    app.camera.look(x-app.mouseX,y-app.mouseY);
    app.mouseX=x; app.mouseY=y;
}

void onClick(GLFWwindow* window,int button,int action,int) {
    if (button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS) captureMouse(window,true);
}

void onFocus(GLFWwindow* window,int focused) {
    if (!focused) captureMouse(window,false);
}

bool down(GLFWwindow* window,int key) { return glfwGetKey(window,key)==GLFW_PRESS; }

void moveCamera(GLFWwindow* window,double dt) {
    if (!glfwGetWindowAttrib(window,GLFW_FOCUSED)) return;
    state(window).camera.move(double(down(window,GLFW_KEY_W))-double(down(window,GLFW_KEY_S)),
        double(down(window,GLFW_KEY_D))-double(down(window,GLFW_KEY_A)),
        double(down(window,GLFW_KEY_E))-double(down(window,GLFW_KEY_Q)),dt,
        down(window,GLFW_KEY_LEFT_SHIFT)||down(window,GLFW_KEY_RIGHT_SHIFT));
}

void screenshot(const std::string& path,int width,int height) {
    std::vector<unsigned char> pixels(std::size_t(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    glPixelStorei(GL_PACK_ALIGNMENT,4);
    std::ofstream file(path,std::ios::binary);
    if (!file) throw std::runtime_error("cannot write capture: "+path);
    file<<"P6\n"<<width<<" "<<height<<"\n255\n";
    for (int y=height-1; y>=0; --y)
        file.write(reinterpret_cast<const char*>(pixels.data()+std::size_t(y)*width*3),width*3);
    if (!file) throw std::runtime_error("capture write failed: "+path);
    std::cout<<"saved capture: "<<path<<"\n";
}

void run(GLFWwindow* window,const Options& settings) {
    Scene scene(settings.shaderDirectory);
    State app(scene);
    glfwSetWindowUserPointer(window,&app);
    glfwSetKeyCallback(window,onKey);
    glfwSetCursorPosCallback(window,onMouse);
    glfwSetMouseButtonCallback(window,onClick);
    glfwSetWindowFocusCallback(window,onFocus);
    Hud hud(settings.shaderDirectory);
    if (settings.simulate>=0) { app.rocket.start(); app.rocket.update(settings.simulate); }
    double last=glfwGetTime(), fpsStart=last, fps=0;
    int frameCount=0,totalFrames=0;
    std::cout<<"space launch | r reset scene | l pad light | h home camera\n"
             <<"wasd move | q/e down/up | shift faster | click/tab capture mouse\n"
             <<"esc releases mouse; esc again closes the window\n";
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) break;
        const double now=glfwGetTime();
        const double dt=std::min(0.05,std::max(0.0,now-last));
        last=now;
        int width=0,height=0;
        glfwGetFramebufferSize(window,&width,&height);
        if (width==0 || height==0) { glfwWaitEventsTimeout(0.05); continue; }
        moveCamera(window,dt);
        app.rocket.update(dt);
        glViewport(0,0,width,height);
        glClearColor(0.65f,0.73f,0.72f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        scene.updateExhaust(app.rocket,dt);
        const auto projection=math3d::perspective(50.0*math3d::pi/180.0,double(width)/height,0.1,600.0);
        scene.render(app.camera.view(),projection,app.camera.position(),app.rocket,app.padLight);
        hud.render(width,height,app.rocket,fps,app.padLight,app.captured);
        ++frameCount;
        ++totalFrames;
        if (now-fpsStart>=0.5) {
            fps=frameCount/(now-fpsStart);
            frameCount=0; fpsStart=now;
            std::ostringstream title;
            title<<"baikonur | "<<app.rocket.status()<<" | "<<std::fixed<<std::setprecision(0)<<fps<<" fps";
            glfwSetWindowTitle(window,title.str().c_str());
        }
        const bool finalFrame=settings.frames>0 && totalFrames>=settings.frames;
        if (!settings.capture.empty() && (finalFrame || settings.frames==0)) {
            screenshot(settings.capture,width,height);
            if (settings.frames==0) glfwSetWindowShouldClose(window,GLFW_TRUE);
        }
        const GLenum error=glGetError();
        if (error!=GL_NO_ERROR) throw std::runtime_error("opengl error: "+std::to_string(error));
        glfwSwapBuffers(window);
        if (finalFrame) break;
    }
    std::cout<<"rendered "<<totalFrames<<" frames; state="<<app.rocket.status()
             <<"; altitude="<<app.rocket.altitude()<<" m\n";
}
}

int main(int argc,char** argv) {
    try {
        const Options settings=options(argc,argv);
        glfwSetErrorCallback([](int code,const char* message) {
            std::cerr<<"glfw "<<code<<": "<<message<<"\n";
        });
        if (!glfwInit()) throw std::runtime_error("glfw initialization failed");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GLFW_TRUE);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,GLFW_FALSE);
#endif
        glfwWindowHint(GLFW_DEPTH_BITS,24);
        glfwWindowHint(GLFW_SAMPLES,4);
        if (settings.hidden) glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        std::unique_ptr<GLFWwindow,void(*)(GLFWwindow*)> window(
            glfwCreateWindow(1280,720,"baikonur / launch control",nullptr,nullptr),glfwDestroyWindow);
        if (!window) throw std::runtime_error("cannot create an opengl 3.3 core window");
        glfwMakeContextCurrent(window.get());
        glewExperimental=GL_TRUE;
        const GLenum status=glewInit();
        if (status!=GLEW_OK) throw std::runtime_error(
            reinterpret_cast<const char*>(glewGetErrorString(status)));
        while (glGetError()!=GL_NO_ERROR) {}
        glfwSwapInterval(settings.vsync ? 1 : 0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_MULTISAMPLE);
        std::cout<<"opengl: "<<glGetString(GL_VERSION)<<"\n";
        run(window.get(),settings);
        window.reset();
        glfwTerminate();
        return 0;
    } catch (const std::exception& error) {
        std::cerr<<"error: "<<error.what()<<"\nrun from the project root, or supply --shaders /path/to/shaders\n";
        glfwTerminate();
        return 1;
    }
}
