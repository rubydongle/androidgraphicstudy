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
 
//#include <SkCanvas.h>
//#include <SkBitmap.h>
//#include <SkRect.h>
//#include <SkColor.h>
 
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
 
    sp<SurfaceControl> surfaceControl = client->createSurface(String8("rubyopengl"), mainDpyInfo.w, mainDpyInfo.h, PIXEL_FORMAT_RGBA_8888, 0);
 
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
 
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, 0, 0);
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    surface = eglCreateWindowSurface(display, config, s.get(), NULL);
    context = eglCreateContext(display, config, NULL, NULL);
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
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
