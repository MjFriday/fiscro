#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
struct RGB {
    int r, g, b;
};

int main() {
    RGB color;
    Display* display = XOpenDisplay(NULL);
    if (!display) return 1;
    int whichScreen = DefaultScreen(display);
    Window root = DefaultRootWindow(display);
    int screenWidth = DisplayWidth(display, whichScreen);
    int screenHeight = DisplayHeight(display, whichScreen);
    XImage* image = XGetImage(display, root, 0, 500, screenWidth, 1, AllPlanes, ZPixmap);
    if (!image)
    {
        std::cerr << "Failed to capture pixel." << std::endl;
        XCloseDisplay(display);
        return 1;
    }
    int targetR = 255, targetG = 0, targetB = 0;

    unsigned long targetPixel = ((targetR << 16) & image->red_mask) |
    ((targetG << 8)  & image->green_mask) |
    (targetB         & image->blue_mask);
    int pixelCount = 0;
  //  for(int y=0; y<screenHeight; y++)
  //  {
        for(int x=0; x<screenWidth; x++)
        {
            unsigned long pixel = XGetPixel(image, x, 0);
            if(pixel != targetPixel)
            {
                std::cout << "Found pixel at (" << x << ", " << 500 << ")" << std::endl;
                pixelCount++;
            }
        }
 //   }
    std::cout << "Found " << pixelCount << " pixels." << std::endl;
    XDestroyImage(image);
    XCloseDisplay(display);
    return 0;
}