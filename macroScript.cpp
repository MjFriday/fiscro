#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <cstdint>
#include <X11/keysym.h>
#include <X11/Xutil.h>

using namespace std;

const string default_settings="screen_resolution: 1366,768\n\n# pixel location settings\n# upper left corner of the fishing mini game bar (X,Y)\nbar_start:407,642\n# bottom right corner of the fishing mini game bar (X,Y)\nbar_end:958,664\n# friend boost icon location. only 1 green pixel from it is needed\nfriend_boost:30,718\n# you must have the rod in your hot bar and have a pixel location of the white outline around it\nrod_equip:373,700\n\n# pixel color settings (very self explanatory)\nctrl_bar_clr_L:0xF1F1F1\nctrl_bar_clr_R:0xF1F1F1\narrow_clr:0x868686\nfish_minigame_clr:0x434B5B\nfriend_boost_clr:0x9BFF9B\n\ncasting:1\nshaking:1\nfishing:1";

struct settings;
void load_settings(settings& s);

struct settings
{
    Display* display = nullptr;
    Window root = 0;
    int screen_res[2],
        bar_start[2], bar_end[2],
        friend_boost[2], rod_equip[2];
    uint32_t ctrl_bar_clr_L, ctrl_bar_clr_R, 
        arrow_clr,
        fish_minigame_clr, friend_boost_clr;
    bool casting, shaking, fishing;

    settings()
    {
        load_settings(*this);
    }

    ~settings()
    {
        if (display)
        {
            XCloseDisplay(display);
        }
    }
};

void str_to_x_and_y(string s, int* arr)
{
    arr[0] = stoi(s.substr(0, s.find(','))); 
    arr[1] = stoi(s.substr(s.find(',') + 1));
}

void load_settings(settings& s)
{
    s.display = XOpenDisplay(NULL);
    if (!s.display) 
    {
        std::cerr << "\033[31mAre you sure you are running under an X11 display?\033[0m" << std::endl;
        exit(1);
    }
    s.root = DefaultRootWindow(s.display);

    ifstream fetchfile("settings.dat");
    if(fetchfile.is_open())
    {
        string line;
        while(getline(fetchfile, line))
        {
            if(line.empty() || line[0] == '#') continue;
            else
            {
                string key = line.substr(0, line.find(':'));
                string value = line.substr(line.find(':') + 1);
                if(key == "screen_resolution") str_to_x_and_y(value, s.screen_res);
                else if(key == "bar_start") str_to_x_and_y(value, s.bar_start);
                else if(key == "bar_end") str_to_x_and_y(value, s.bar_end);
                else if(key == "friend_boost") str_to_x_and_y(value, s.friend_boost);
                else if(key == "rod_equip") str_to_x_and_y(value, s.rod_equip);
                else if(key == "ctrl_bar_clr_L") s.ctrl_bar_clr_L = stoul(value, nullptr, 16);
                else if(key == "ctrl_bar_clr_R") s.ctrl_bar_clr_R = stoul(value, nullptr, 16);
                else if(key == "arrow_clr") s.arrow_clr = stoul(value, nullptr, 16);
                else if(key == "fish_minigame_clr") s.fish_minigame_clr = stoul(value, nullptr, 16);
                else if(key == "friend_boost_clr") s.friend_boost_clr = stoul(value, nullptr, 16);
                else if(key == "casting") s.casting = stoi(value);
                else if(key == "shaking") s.shaking = stoi(value);
                else if(key == "fishing") s.fishing = stoi(value);
            }
        }
        fetchfile.close();
    }
    else
    {
        cout << "\033[33mNo settings file found, creating settings file with default settings.\033[0m\n";
        ofstream createfile("settings.dat");
        createfile << default_settings;
        createfile.close();
        s.screen_res[0]=1366; s.screen_res[1]=768;
        s.bar_start[0]=407; s.bar_start[1]=642;
        s.bar_end[0]=958; s.bar_end[1]=664;
        s.friend_boost[0]=30; s.friend_boost[1]=718;
        s.rod_equip[0]=373; s.rod_equip[1]=700;
        s.ctrl_bar_clr_L=0xF1F1F1; s.ctrl_bar_clr_R=0xF1F1F1;
        s.arrow_clr=0x868686;
        s.fish_minigame_clr=0x434B5B; s.friend_boost_clr=0x9BFF9B;
        s.casting=1; s.shaking=1; s.fishing=1;
    }
}

bool frnd_icon_check(settings& s)
{
    XImage* check = XGetImage(s.display, s.root, s.friend_boost[0], s.friend_boost[1], 1, 1, AllPlanes, ZPixmap);
    bool active = (s.friend_boost_clr == ((uint32_t)XGetPixel(check, 0, 0) & 0xFFFFFF));
    XDestroyImage(check); 
    return active;
}

int rod_equiping(settings& s)
{
    XImage* check = XGetImage(s.display, s.root, s.rod_equip[0], s.rod_equip[1], 1, 1, AllPlanes, ZPixmap);
    if(0xFFFFFF != ((uint32_t)XGetPixel(check, 0, 0) & 0xFFFFFF))
    {
        XTestFakeKeyEvent(s.display, XKeysymToKeycode(s.display, XK_1), 1, CurrentTime);
        XFlush(s.display);
        usleep(1000);
        XTestFakeKeyEvent(s.display, XKeysymToKeycode(s.display, XK_1), 0, CurrentTime);
        XFlush(s.display);
    }
    XDestroyImage(check);
    return 1;
}

int cast_(settings& s)
{
    if(frnd_icon_check(s))
    {
        rod_equiping(s);
        XTestFakeMotionEvent(s.display, -1, (s.screen_res[0] / 2), (s.screen_res[1] / 2), CurrentTime);
        XFlush(s.display); usleep(1000);
        XTestFakeButtonEvent(s.display, 1, 1, CurrentTime);
        XFlush(s.display); usleep(200000);
        XTestFakeButtonEvent(s.display, 1, 0, CurrentTime);
        XFlush(s.display); usleep(10000);
    }
    return 0;
}

int shake_(settings& s)
{
    while(1)
    {
        if(frnd_icon_check(s))
        {
            XTestFakeKeyEvent(s.display, XKeysymToKeycode(s.display, XK_Return), 1, CurrentTime);
            XFlush(s.display); usleep(5000);
            XTestFakeKeyEvent(s.display, XKeysymToKeycode(s.display, XK_Return), 0, CurrentTime);
            XFlush(s.display); usleep(50000);
        }
        else
        {
            return 0;
        }
    }
}

int fishing_(settings& s)
{
    int mini_game_bar_[3] = {s.bar_start[0], s.bar_end[0], (s.bar_start[1] + s.bar_end[1]) / 2};
    while(1)
    {
        if(frnd_icon_check(s)) 
        {
            return 0;
        }
        
        int bar_width = mini_game_bar_[1] - mini_game_bar_[0];
        XImage* check_bar = XGetImage(s.display, s.root, mini_game_bar_[0], mini_game_bar_[2], bar_width, 1, AllPlanes, ZPixmap);
        
        int ctrl_bar_pos[2] = {0, bar_width - 1};
        int priority_zone = bar_width / 2;

        for(int i = 0; i < bar_width - 1; i++)
        {
            if(s.ctrl_bar_clr_L == ((uint32_t)XGetPixel(check_bar, i, 0) & 0xFFFFFF) || s.arrow_clr == ((uint32_t)XGetPixel(check_bar, i, 0) & 0xFFFFFF))
            {
                ctrl_bar_pos[0] = i; break;
            }
        } usleep(1000);
        for(int i = bar_width - 1; i >= 0; i--)
        {
            if(s.ctrl_bar_clr_R == ((uint32_t)XGetPixel(check_bar, i, 0) & 0xFFFFFF) || s.arrow_clr == ((uint32_t)XGetPixel(check_bar, i, 0) & 0xFFFFFF))
            {
                ctrl_bar_pos[1] = i; break;
            }
        } usleep(1000);
        for(int i = 0; i < bar_width - 1; i++)
        {
            if(s.fish_minigame_clr == ((uint32_t)XGetPixel(check_bar, i, 0) & 0xFFFFFF))
            {
                priority_zone = i; break;
            }
        }
        XDestroyImage(check_bar);

        int mid_of_ctrl_bar_pos = (ctrl_bar_pos[0] + ctrl_bar_pos[1]) / 2;
        int ctrl_bar_size = ctrl_bar_pos[1] - ctrl_bar_pos[0];
        double how_far = abs(mid_of_ctrl_bar_pos - priority_zone);
        if(how_far < 0) how_far *= -1;
        double primary_time = 25000 * std::sqrt(how_far)+1000;
        double secondary_time = 6000 * std::sqrt(how_far)+600;
        if(priority_zone <= (ctrl_bar_size * 0.6))
        {
            XTestFakeButtonEvent(s.display, 1, 0, CurrentTime);
            XFlush(s.display); usleep(primary_time);
        }
        else if(priority_zone >= (bar_width - (ctrl_bar_size * 0.6)))
        {
            XTestFakeButtonEvent(s.display, 1, 1, CurrentTime);
            XFlush(s.display); usleep(primary_time); 
        }
        else
        {
            if(priority_zone <= mid_of_ctrl_bar_pos)
            {
                XTestFakeButtonEvent(s.display, 1, 0, CurrentTime);
                XFlush(s.display); usleep(primary_time);
                XTestFakeButtonEvent(s.display, 1, 1, CurrentTime);
                XFlush(s.display); usleep(secondary_time);
            }
            if(priority_zone > mid_of_ctrl_bar_pos)
            {
                XTestFakeButtonEvent(s.display, 1, 1, CurrentTime);
                XFlush(s.display); usleep(primary_time);
                XTestFakeButtonEvent(s.display, 1, 0, CurrentTime);
                XFlush(s.display); usleep(secondary_time);
            }
        }
    }
}

int main()
{
    settings s;
    while(1)
    {
        if(s.casting) cast_(s);
        if(s.shaking) shake_(s);
        if(s.fishing) {sleep(1);fishing_(s);}
    }
    return 0;
}