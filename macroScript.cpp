#include <iostream>
#include <fstream>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <cstdint>
#include <X11/keysym.h>
#include <X11/Xutil.h>
using namespace std;
const string default_settings="screen_resolution: 1366,768\n\n# pixel location settings\n# upper left corner of the fishing mini game bar (X,Y)\nbar_start:407,642\n# bottom right corner of the fishing mini game bar (X,Y)\nbar_end:958,664\n# friend boost icon location. only 1 green pixel from it is needed\nfriend_boost:30,718\n# you must have the rod in your hot bar and have a pixel location of the white outline around it\nrod_equip:373,700\n\n# pixel color settings (very self explanatory)\nctrl_bar_clr_L:0xF1F1F1\nctrl_bar_clr_R:0xF1F1F1\narrow_clr_range:0x707070,0x919191\nfish_minigame_clr:0x434B5B\nfriend_boost_clr:0x9BFF9B\n\ncasting:1\nshaking:1\nfishing:1";
struct settings
{
    int screen_res[2],
        bar_start[2], bar_end[2],
        friend_boost[2], rod_equip[2];
    uint32_t ctrl_bar_clr_L, ctrl_bar_clr_R, 
        arrow_clr_range[2],
        fish_minigame_clr ,friend_boost_clr;
    bool casting, shaking, fishing;
};

void str_to_x_and_y(string s, int* arr)
{
    arr[0]=stoi(s.substr(0,s.find(','))); arr[1]=stoi(s.substr(s.find(',')+1));
}

void load_settings(settings&s)
{
    ifstream fetchfile("settings.dat");
    if(fetchfile.is_open())
    {
        string line;
        while(getline(fetchfile,line))
        {
            if(line.empty()||line[0]=='#') continue;
            else
            {
                string key=line.substr(0,line.find(':'));
                string value=line.substr(line.find(':')+1);
                if(key=="screen_resolution") str_to_x_and_y(value,s.screen_res);
                else if(key=="bar_start") str_to_x_and_y(value,s.bar_start);
                else if(key=="bar_end") str_to_x_and_y(value,s.bar_end);
                else if(key=="friend_boost") str_to_x_and_y(value,s.friend_boost);
                else if(key=="rod_equip") str_to_x_and_y(value,s.rod_equip);
                else if(key=="ctrl_bar_clr_L") s.ctrl_bar_clr_L=stoul(value,nullptr,16);
                else if(key=="ctrl_bar_clr_R") s.ctrl_bar_clr_R=stoul(value,nullptr,16);
                else if(key=="arrow_clr_range")
                {
                    s.arrow_clr_range[0]=stoul(value.substr(0,value.find(',')),nullptr,16);
                    s.arrow_clr_range[1]=stoul(value.substr(value.find(',')+1),nullptr,16);
                }
                else if(key=="fish_minigame_clr") s.fish_minigame_clr=stoul(value,nullptr,16);
                else if(key=="friend_boost_clr") s.friend_boost_clr=stoul(value,nullptr,16);
                else if(key=="casting") s.casting=stoi(value);
                else if(key=="shaking") s.shaking=stoi(value);
                else if(key=="fishing") s.fishing=stoi(value);
            }
        }
        fetchfile.close();
    }
    else
    {
        ofstream createfile("settings.dat");
        createfile<<default_settings;
        createfile.close();
        s.screen_res[0]=1366; s.screen_res[1]=768;
        s.bar_start[0]=407; s.bar_start[1]=642;
        s.bar_end[0]=958; s.bar_end[1]=664;
        s.friend_boost[0]=30; s.friend_boost[1]=718;
        s.rod_equip[0]=373; s.rod_equip[1]=700;
        s.ctrl_bar_clr_L=0xF1F1F1; s.ctrl_bar_clr_R=0xF1F1F1;
        s.arrow_clr_range[0]=0x707070; s.arrow_clr_range[1]=0x919191;
        s.fish_minigame_clr=0x434B5B; s.friend_boost_clr=0x9BFF9B;
        s.casting=1; s.shaking=1; s.fishing=1;
    }

}

int cast_(int frnd_boost[2], uint32_t frnd_clr, int rod_equip[2], int mouse_pos[2], Display* display, Window root)
{
    usleep(250000);
    XImage* check_frnd=XGetImage(display,root,frnd_boost[0],frnd_boost[1],1,1,AllPlanes,ZPixmap);
    if(frnd_clr==((uint32_t)XGetPixel(check_frnd, 0, 0) & 0xFFFFFF))
    {
        XImage* check_rod=XGetImage(display,root,rod_equip[0],rod_equip[1],1,1,AllPlanes,ZPixmap);
        if(0xFFFFFF!=((uint32_t)XGetPixel(check_rod, 0, 0) & 0xFFFFFF))
        {
            XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_1), 1, CurrentTime);
            XFlush(display);
            usleep(10000);
            XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_1), 0, CurrentTime);
            XFlush(display);
        }
        XDestroyImage(check_rod);
        XTestFakeMotionEvent(display, -1, mouse_pos[0], mouse_pos[1], CurrentTime);
        XFlush(display); usleep(1000);
        XTestFakeButtonEvent(display, 1, 1, CurrentTime);
        XFlush(display); usleep(150000);
        XTestFakeButtonEvent(display, 1, 0, CurrentTime);
        XFlush(display); usleep(100000);
    }
    XDestroyImage(check_frnd);
    return 0;
}

int shake_(int frnd_boost[2], uint32_t frnd_clr, Display* display, Window root)
{
    while(1)
    {
        XImage* check_frnd=XGetImage(display,root,frnd_boost[0],frnd_boost[1],1,1,AllPlanes,ZPixmap);
        if(frnd_clr==((uint32_t)XGetPixel(check_frnd, 0, 0) & 0xFFFFFF))
        {
            XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Return), 1, CurrentTime);
            XFlush(display); usleep(5000);
            XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Return), 0, CurrentTime);
            XFlush(display); usleep(50000);
            XDestroyImage(check_frnd);
        }
        else
        {
            XDestroyImage(check_frnd);
            break;
        }
    }
    return 0;
}

int fishing_(int bar[3], uint32_t ctrl_bar_clr_L, uint32_t ctrl_bar_clr_R, uint32_t arrow_clr_range[2], uint32_t fish_minigame_clr, int frnd_boost[2], uint32_t frnd_clr, Display* display, Window root, int mouse_pos[2])
{
    bool held=0;
    while(1)
    {
        XImage* check_frnd=XGetImage(display,root,frnd_boost[0],frnd_boost[1],1,1,AllPlanes,ZPixmap);
        if(frnd_clr==((uint32_t)XGetPixel(check_frnd, 0, 0) & 0xFFFFFF)) 
        {
            XDestroyImage(check_frnd);
            return 0;
        }
        XDestroyImage(check_frnd); usleep(4);
        XImage* check_bar=XGetImage(display,root,bar[0],bar[2],bar[1],1,AllPlanes,ZPixmap);
        int bar_width=bar[1]-bar[0],ctrl_bar_pos[2]={0, bar_width-1}, priority_zone=bar_width/2;
        for(int i=0; i<bar_width-1; i++)
            {
                if(ctrl_bar_clr_L==((uint32_t)XGetPixel(check_bar,i,0)& 0xFFFFFF))
                {
                    ctrl_bar_pos[0]=i; break;
                }
            }
        for(int i=bar_width-1; i>=0; i--)
            {
                if(ctrl_bar_clr_R==((uint32_t)XGetPixel(check_bar,i,0)& 0xFFFFFF))
                {
                    ctrl_bar_pos[1]=i; break;
                }
            }
        for(int i=0; i<bar_width-1; i++)
            {
                if(fish_minigame_clr==((uint32_t)XGetPixel(check_bar,i,0)& 0xFFFFFF))
                {
                    priority_zone=i; break;
                }
            }
        XDestroyImage(check_bar);
        int mid_of_ctrl_bar_pos= (ctrl_bar_pos[0]+ctrl_bar_pos[1])/2;
        double primary_time=(mid_of_ctrl_bar_pos - priority_zone)*4020;
        double secondary_time=primary_time*0.15;
        if(primary_time>0)
        {
            XTestFakeButtonEvent(display, 1, 0, CurrentTime);
            XFlush(display); usleep(primary_time);
            XTestFakeButtonEvent(display, 1, 1, CurrentTime);
            XFlush(display); usleep(secondary_time);
            XTestFakeButtonEvent(display, 1, 0, CurrentTime);
            XFlush(display); usleep(primary_time);
            XTestFakeButtonEvent(display, 1, 1, CurrentTime);
            XFlush(display); usleep(secondary_time*1);


        }
        if(primary_time<0)
        {
            XTestFakeButtonEvent(display, 1, 1, CurrentTime);
            XFlush(display); usleep(primary_time*-1);
            XTestFakeButtonEvent(display, 1, 0, CurrentTime);
            XFlush(display); usleep(secondary_time*-1);
            XTestFakeButtonEvent(display, 1, 1, CurrentTime);
            XFlush(display); usleep(primary_time*-1);
            XTestFakeButtonEvent(display, 1, 0, CurrentTime);
            XFlush(display); usleep(secondary_time*-1);



        }
    }
    
}

int main()
{
    settings s; load_settings(s);
    int middle_of_screen[2]={s.screen_res[0]/2,s.screen_res[1]/2};
    Display* display=XOpenDisplay(NULL);
    if (!display) 
    {
        std::cerr << "Are you sure you are running under an X11 display?" << std::endl;
        return 1;
    }
    Window root = DefaultRootWindow(display);
    int mini_game_bar_[3] ={s.bar_start[0], s.bar_end[0], (s.bar_start[1]+s.bar_end[1])/2};
    while(1)
    {
        if(s.casting) cast_(s.friend_boost, s.friend_boost_clr, s.rod_equip, middle_of_screen, display, root);
        if(s.shaking) {shake_(s.friend_boost, s.friend_boost_clr, display, root); usleep(500000);}
        if(s.fishing) fishing_(mini_game_bar_, s.ctrl_bar_clr_L, s.ctrl_bar_clr_R, s.arrow_clr_range, s.fish_minigame_clr, s.friend_boost, s.friend_boost_clr, display, root, middle_of_screen);
    }
    return 0;
}