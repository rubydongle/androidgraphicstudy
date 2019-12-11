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
 
#include <SkCanvas.h>
#include <SkBitmap.h>
#include <SkRect.h>
#include <SkColor.h>
 
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
 
    sp<SurfaceControl> surfaceControl = client->createSurface(String8("rubyskia"), mainDpyInfo.w, mainDpyInfo.h, PIXEL_FORMAT_RGBA_8888, 0);
 
    SurfaceComposerClient::Transaction t;
    t.setLayer(surfaceControl, 0x7FFFFFFF).apply();
 
    sp<Surface> surface = surfaceControl->getSurface();
    ANativeWindow_Buffer outBuffer;
 
    SkBitmap surfaceBitmap;
    err = surface->lock(&outBuffer, NULL);
 
    ssize_t bpr = outBuffer.stride * bytesPerPixel(outBuffer.format);
    surfaceBitmap.installPixels(SkImageInfo::MakeN32Premul(outBuffer.width, outBuffer.height),outBuffer.bits, bpr);
 
    SkCanvas surfaceCanvas(surfaceBitmap);
      surfaceCanvas.save();
        surfaceCanvas.drawColor(SK_ColorRED);
      surfaceCanvas.restore();
    surface->unlockAndPost();
 
    bool isShow = true;
    while(1) {
        if (isShow) {
            t.hide(surfaceControl).apply();
            isShow = false;
        } else {
            t.show(surfaceControl).apply();
            isShow = true;
        }
        sleep(2);
    }
}
