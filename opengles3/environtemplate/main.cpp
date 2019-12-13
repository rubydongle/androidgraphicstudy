//#define LOG_TAG "graphictest"
 
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
 
//#include <GLES/gl.h>
//#include <GLES/glext.h>
//#include <EGL/eglext.h>

#include "esutil.h"
 
#define DEBUG 1
 
using namespace android;
 
int main(int argc, char* const argv[])
{
 
    ALOGD("hahaha graphictest");
    status_t err;
    ESContext context;

    // WinCreate begin----------------->
    sp<SurfaceComposerClient> client = new SurfaceComposerClient();
 
    sp<IBinder> mainDpy = SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain);
    DisplayInfo mainDpyInfo;
    err = SurfaceComposerClient::getDisplayInfo(mainDpy, &mainDpyInfo);
    ALOGD("displayinfo w:%d h:%d", mainDpyInfo.w, mainDpyInfo.h);
    context.width = mainDpyInfo.w;
    context.height = mainDpyInfo.h;
 
    sp<SurfaceControl> surfaceControl =
            client->createSurface(String8("rubyopengltemplateesv3"), mainDpyInfo.w, mainDpyInfo.h, PIXEL_FORMAT_RGBA_8888, 0); 
 
    SurfaceComposerClient::Transaction t;
    t.setLayer(surfaceControl, 0x7FFFFFFF).apply();
 
    sp<Surface> s = surfaceControl->getSurface();
    context.eglNativeWindow = s.get();
    context.eglNativeDisplay = EGL_DEFAULT_DISPLAY;
    // WinCreate end<-------------------------

    //GLboolean result = esCreateWindowAndroid(&context, "templateesv3");//, 0, 0, 0)
    GLboolean result = esCreateWindow(&context, "templateesv3", 0, 0, 0);
    if (result == GL_FALSE) {
        ALOGD("esCreateWindowAndroid return false");
    }
 
    // --------> opengl operation bellow
    glClearColor(255,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(context.eglDisplay, context.eglSurface);
 
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
            eglSwapBuffers(context.eglDisplay, context.eglSurface);

            t.show(surfaceControl).apply();
            isShow = true;
        }
    }
}
