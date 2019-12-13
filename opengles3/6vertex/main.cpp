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

typedef struct
{
    GLuint programObject;
} UserData;

int Init(ESContext *esContext)
{
    UserData *userData = (UserData*)esContext->userData;
    const char vShaderStr[] =
            "#version 300 es				\n"
	    "layout(location=0) in vec4 a_color;	\n"
	    "layout(location=1) in vec4 a_position;	\n"
	    "out vec4 v_color;				\n"
	    "void main()				\n"
	    "{						\n"
	    "    v_color = a_color;			\n"
	    "    gl_Position = a_position;		\n"
	    "}";

    const char fShaderStr[] =
	    "#version 300 es				\n"
	    "precision mediump float;			\n"
	    "in vec4 v_color;				\n"
	    "out vec4 o_fragColor;			\n"
	    "void main()				\n"
	    "{						\n"
	    "    o_fragColor = v_color;			\n"
	    "}";

    GLuint programObject;

    programObject = esLoadProgram(vShaderStr, fShaderStr);

    if (programObject == 0) {
        return GL_FALSE;
    }

    userData->programObject = programObject;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return GL_TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = (UserData*)esContext->userData;
    GLfloat color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat vertexPos[3*3] = {0.0f, 0.5f, 0.0f, // v0
                              -0.5f, -0.5f, 0.0f, //v1
			      0.5f, -0.5f, 0.0f}; //v2

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);
    glVertexAttrib4fv(0, color);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, vertexPos);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(1);
}

int main(int argc, char* const argv[])
{
 
    ALOGD("hahaha graphictest");
    status_t err;
    ESContext context;

    context.userData = malloc(sizeof(UserData));

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
//    glClearColor(255,0,0,1);
//    glClear(GL_COLOR_BUFFER_BIT);
//    eglSwapBuffers(context.eglDisplay, context.eglSurface);
 
    Init(&context);

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
//            glClear(GL_COLOR_BUFFER_BIT);
            Draw(&context);
            eglSwapBuffers(context.eglDisplay, context.eglSurface);

            t.show(surfaceControl).apply();
            isShow = true;
        }
    }
}
