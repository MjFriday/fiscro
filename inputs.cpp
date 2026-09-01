#include <iostream>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <X11/keysym.h>
int main()
{
    Display* display = XOpenDisplay(NULL);
    if(!display) {std::cerr<<"fuckass error couldnt find your screen bro"<<std::endl; return 1;}
    int whichScreen = DefaultScreen(display);
    int screenWidth = DisplayWidth(display, whichScreen);
    int screenHeight = DisplayHeight(display, whichScreen);
    int x = screenWidth / 2; int y = screenHeight / 2;
    for(int i = 0; i < 15; ++i)
    {
        XTestFakeMotionEvent(display, whichScreen, x, y, 0);
        XTestFakeButtonEvent(display, 1, True, 0);
        XFlush(display);
        usleep(50000);
        XTestFakeButtonEvent(display, 1, False, 0);
        XFlush(display);
        usleep(50000);
    }
    XCloseDisplay(display);
    return 0;
}