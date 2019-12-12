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
 
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/eglext.h>

#include "esutil.h"
 
#define DEBUG 1
 
using namespace android;

typedef struct
{
    GLuint programObject;
} UserData;


GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
	if (infoLen > 1) {
            char *infoLog = (char*)malloc(sizeof(char) * infoLen);
	    glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
	    esLogMessage("Error compiling shader:\n%s\n", infoLog);
	    free(infoLog);
	}
	glDeleteShader(shader);
	return 0;
    }

    return shader;
}

int Init(ESContext *esContext)
{
    UserData *userData = (UserData*)esContext->userData;
    char vShaderStr[] =
	    "#version 300 es				\n"
	    "layout(location = 0) in vec4 vPosition;	\n"
	    "void main()				\n"
	    "{						\n"
	    "    gl_Position = vPosition;		\n"
	    "}						\n";

    char fShaderStr[] =
	    "#version 300 es				\n"
	    "precision mediump float;			\n"
	    "out vec4 fragColor;			\n"
	    "void main()				\n"
	    "{						\n"
	    "    fragColor = vec4(1.0, 0.0, 0.0, 1.0);	\n"
	    "}						\n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    programObject = glCreateProgram();

    if (programObject == 0) {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;
	glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
	if (infoLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
	    glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
	    esLogMessage("Error linking program:\n%s\n", infoLog);
	    free(infoLog);
	}
	glDeleteProgram(programObject);
        return FALSE;
    }
    
    userData->programObject = programObject;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = (UserData*)esContext->userData;
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f,
	                   -0.5f, -0.5f, 0.0f,
			   0.5f, -0.5f, 0.0f};

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = (UserData*)esContext->userData;
    glDeleteProgram(userData->programObject);
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
 
    if (!Init(&context)) {
        esLogMessage("Init failed");
        return GL_FALSE;
    }
    // --------> opengl operation bellow
//    glClearColor(255,0,0,1);
//    glClear(GL_COLOR_BUFFER_BIT);
//    eglSwapBuffers(context.eglDisplay, context.eglSurface);
 
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

	    /*
            glClear(GL_COLOR_BUFFER_BIT);
            eglSwapBuffers(context.eglDisplay, context.eglSurface);
	    */
            Draw(&context);

            eglSwapBuffers(context.eglDisplay, context.eglSurface);

            t.show(surfaceControl).apply();
            isShow = true;
        }
    }
}
