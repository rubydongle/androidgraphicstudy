#define LOG_TAG "graphictest"
 
#include <unistd.h>
 
#include <android/native_window.h>
#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/SurfaceControl.h>
#include <system/window.h>
#include <ui/DisplayInfo.h>
#include <ui/GraphicBuffer.h>
#include <utils/Log.h>
#include <utils/StrongPointer.h>
 
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/eglext.h>
 
#define DEBUG 1
 
using namespace android;
 
int main(int argc, char* const argv[])
{
 
    ALOGD("hahaha graphictest");
    status_t err;
 
    sp<SurfaceComposerClient> client = new SurfaceComposerClient();
 
    sp<IBinder> mainDpy = SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain);
    DisplayInfo mainDpyInfo;
    err = SurfaceComposerClient::getDisplayInfo(mainDpy, &mainDpyInfo);
    ALOGD("displayinfo w:%d h:%d", mainDpyInfo.w, mainDpyInfo.h);
 
    sp<SurfaceControl> surfaceControl = client->createSurface(String8("rubyegl"), mainDpyInfo.w, mainDpyInfo.h, PIXEL_FORMAT_RGBA_8888, 0);
 
    SurfaceComposerClient::Transaction t;
    t.setLayer(surfaceControl, 0x7FFFFFFF).apply();
 
    sp<Surface> s = surfaceControl->getSurface();
 
    // initialize opengl and egl
    const EGLint attribs[] = {
             EGL_RED_SIZE,   8,
             EGL_GREEN_SIZE, 8,
             EGL_BLUE_SIZE,  8,
             EGL_DEPTH_SIZE, 0,
             EGL_NONE
     };
    EGLint w, h;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;
 
    // 窗口系统,EGLDisplay封装了所有系统相关性，用于和原生系统接口。
    // 任何使用EGL的应用程序必须执行的第一个操作是创建和初始化与本地EGL显示的连接。
    // 调用下面函数打开与EGL显示服务器的连接:
    // EGLDisplay eglGetDisplay(EGLNativeDisplayType displayId)
    // displayId指定显示连接，默认连接为EGL_DEFAULT_DISPLAY
    // 如果显示连接不可用，则eglGetDisplay将返回EGL_NO_DISPLAY,这个错误表示EGL不可用。
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    // 成功打开连接后需要初始化EGL，通过如下函数完成:
    // EGLBoolean eglInitialize(EGLDisplay display, EGLint *majorVersion, EGLint * minorVersion)
    // display		指定EGL显示连接
    // majorVersion	指定EGL实现返回的主版本号，可能为NULL
    // minorVersion	指定EGL实现返回的次版本号，可能为NULL
    // 这个函数初始化EGL内部数据结构，返回EGL实现的主版本号和次版本号。如果EGL无法初始化，这个调用将返回EGL_FALSE,并将EGL错误代码设置为:
    // - EGL_BAD_DISPALY->如果display没有指定有效的EGLDisplay
    // - EGL_NOT_INITIALIZED->如果EGL不能初始化
    eglInitialize(display, 0, 0);
    // 下面的函数让EGL选择配置的EGLConfig
    // EGLBoolean eglChooseConfig(EGLDisplay	display,
    // 				  const EGLint	*attribList,
    // 				  EGLConfig	*configs,
    // 				  EGLint	maxReturnConfigs,
    // 				  EGLint	*numConfigs)
    // display		指定EGL显示连接
    // attribList	指定configs匹配的属性列表
    // configs		指定配置列表
    // maxReturnConfigs	指定配置的大小
    // numConfigs	指定返回的配置大小
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    // 创建屏幕上的渲染区域:EGL窗口
    // EGLSurface eglCreateWindowSurface(EGLDisplay		display,
    // 					 EGLConfig		config,
    // 					 EGLNativeDisplayType	window,
    // 					 const EGLint		*attribList)
    // display		指定EGL显示连接
    // config		指定配置
    // window		指定原生窗口
    // attribList	指定窗口属性列表；可能为NULL
    // 它需要原生窗口系统事先创建一个窗口，这个在Android中对应的就是Surface.
    surface = eglCreateWindowSurface(display, config, s.get(), NULL);

    // 创建屏幕外渲染区域:EGL Pbuffer	pBuffer(pixel buffer)
    // EGLSurface eglCreatePbufferSurface(EGLDisplay	display,
    // 					  EGLConfig	config
    // 					  const EGLint	*attribList)
    // display		指定EGL显示连接
    // config		指定配置
    // attribList	指定像素缓冲区属性列表；可能为NULL

    // 创建一个渲染上下文EGLContext
    // 渲染上下文是OpenGL的内部数据结构，包含操作所需的所有状态信息。
    // EGLContext eglCreateContext(EGLDisplay	display,
    // 				   EGLConfig	config,
    // 				   EGLContext	shareContext,
    // 				   const EGLint	*attribList)
    // display		指定EGL显示连接
    // config		指定配置
    // shareContext	允许多个EGL上下文共享特定类型的数据，例如着色器程序和纹理贴图；使用EGL_NO_CONTEXT表示没有共享
    // attribList	指定创建上下文使用的属性列表；只有一个可接受的属性-EGL_CONTEXT_CLIENT_VERSION
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
//    eglQuerySurface(display, surface, EGL_WIDTH, &w);
//    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
    // 指定某个EGLContext为当前上下文
    // 通过下面的调用，关联特定的EGLContext和某个EGLSurface。
    // EGLBoolean eglMakeCurrent(EGLDisplay	display,
    // 				 EGLSurface	draw,
    // 				 EGLSurface	read,
    // 				 EGLContext	context)
    // display		指定EGL显示连接
    // draw		指定EGL绘图表面
    // read		指定EGL读取表面
    // context		指定连接到该表面的渲染上下文
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
        return NO_INIT;
 
    glClearColor(255,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(display, surface);
 
    bool isShow = true;
    int time = 0;
    while(1) {
        sleep(1);
        if (isShow) {
            isShow = false;
            t.hide(surfaceControl).apply();
        } else {
            switch (time % 3) {
                case 0:
                    glClearColor(0,255,0,1);
                    break;
                case 1:
                    glClearColor(255,0,0,1);
                    break;
                case 2:
                    glClearColor(0,0,255,1);
                    break;
                default:
                    break;
            }
            time++;
            glClear(GL_COLOR_BUFFER_BIT);
            eglSwapBuffers(display, surface);
 
            t.show(surfaceControl).apply();
            isShow = true;
        }
    }
}
