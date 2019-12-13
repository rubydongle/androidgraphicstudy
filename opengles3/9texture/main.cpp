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
 
#include "esutil.h"
 
#define DEBUG 1
 
using namespace android;

typedef struct
{
    GLuint programObject;

    GLint samplerLoc;

    GLuint textureId;

    int numIndices;
    GLfloat *vertices;
    GLfloat *normals;
    GLuint *indices;
} UserData;

GLuint CreateSimpleTextureCubemap()
{
    GLuint textureId;
    GLubyte cubePixels[6][3] =
    {
        {255, 0,   0},
	{0,   255, 0},
	{0,   0,   255},
	{255, 255, 0},
	{255, 0,   255},
	{255, 255, 255}
    };

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[1]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[2]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[3]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[4]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[5]);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureId;
}

int Init(ESContext *esContext)
{
    UserData *userData = (UserData*)esContext->userData;
    const char vShaderStr[] =
            "#version 300 es				\n"
	    "layout(location=0) in vec4 a_position;	\n"
	    "layout(location=1) in vec3 a_normal;	\n"
	    "out vec3 v_normal;				\n"
	    "void main()				\n"
	    "{						\n"
	    "    gl_Position = a_position;		\n"
	    "    v_normal = a_normal;			\n"
	    "}";

    const char fShaderStr[] =
	    "#version 300 es					\n"
	    "precision mediump float;				\n"
	    "in vec3 v_normal;					\n"
	    "layout(location = 0) out vec4 outColor;		\n"
	    "uniform samplerCube s_texture;			\n"
	    "void main()					\n"
	    "{							\n"
	    "    outColor = texture(s_texture, v_normal);	\n"
	    "}";

    GLuint programObject;

    programObject = esLoadProgram(vShaderStr, fShaderStr);

    if (programObject == 0) {
        return GL_FALSE;
    }

    userData->programObject = programObject;

    userData->samplerLoc = glGetUniformLocation(userData->programObject, "s_texture");
    userData->textureId = CreateSimpleTextureCubemap();

    userData->numIndices = esGenSphere(20, 0.75f, &userData->vertices, &userData->normals,
                                       NULL, &userData->indices);

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    return GL_TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = (UserData*)esContext->userData;
//    GLfloat color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
//    GLfloat vertexPos[3*3] = {0.0f, 0.5f, 0.0f, // v0
//                              -0.5f, -0.5f, 0.0f, //v1
//			      0.5f, -0.5f, 0.0f}; //v2

    glViewport(0, 0, esContext->width, esContext->width);//height);
    glClear(GL_COLOR_BUFFER_BIT);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glUseProgram(userData->programObject);

//    glVertexAttrib4fv(0, color);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, userData->vertices);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, userData->normals);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, userData->textureId);

    glUniform1i(userData->samplerLoc, 0);
    glDrawElements(GL_TRIANGLES, userData->numIndices,
                   GL_UNSIGNED_INT, userData->indices);
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
    GLboolean result = esCreateWindow(&context, "templateesv3", 0, 0, ES_WINDOW_RGB);
    if (result == GL_FALSE) {
        ALOGD("esCreateWindowAndroid return false");
    }
 
    // --------> opengl operation bellow
//    glClearColor(255,0,0,1);
//    glClear(GL_COLOR_BUFFER_BIT);
//    eglSwapBuffers(context.eglDisplay, context.eglSurface);
 
    t.show(surfaceControl).apply();
    Init(&context);

    bool isShow = true;
    int time = 0;
    while(1) {
        sleep(1);
        if (isShow) {
            isShow = false;
//            t.hide(surfaceControl).apply();
        } else {
		/*
            switch (time % 3) {
                case 0:
                    glClearColor(0.0f, 1.0f, 0.0f, 0.5f);
                    break;
                case 1:
                    glClearColor(1.0f, 0.0f, 0.0f, 0.5f);
                    break;
                case 2:
                    glClearColor(0.0f, 0.0f, 1.0f, 0.5f);
                    break;
                default:
                    break;
            }
	    */
            time++;
//            glClear(GL_COLOR_BUFFER_BIT);
            Draw(&context);
            eglSwapBuffers(context.eglDisplay, context.eglSurface);

//            t.show(surfaceControl).apply();
            isShow = true;
        }
    }
}
