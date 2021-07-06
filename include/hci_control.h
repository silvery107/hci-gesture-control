#ifndef HCI_CONTROL_H_
#define HCI_CONTROL_H_
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
using cv::Point;
using std::cout;
using std::endl;
using std::pair;
using std::vector;

const pair<int, int> ScreenResolution(1920, 1080);
const pair<int, int> CameraResolution(640, 480);
const Point cursorOri(450, 250);
const int cursorRad = 25;
const int MOUSEMODE_BROWSE = 0;
const int MOUSEMODE_GAME = 1;

// @param enentIndex 1,2,4,5
// @param clkFlag Prevent multiple clicks
void mouseControl(int eventIndex, int curposX, int curposY, bool &clkFlag, int mouseMode = 0);
// @param shape Integer larger than 1
void keybdControl(int shape);
void traDetect(vector<Point> &trajectory, int &shape);
bool isOpRange(int curposX, int curposY);

void traDetect(vector<Point> &trajectory, int &shape)
{
    shape = 0;
    if (trajectory.size() > 0)
    {
        vector<Point> contours;
        approxPolyDP(trajectory, contours, 13, true);
        shape = contours.size();
        trajectory.clear();
    }
}

void mouseControl(int eventIndex, int curposX, int curposY, bool &clkFlag, int mouseMode)
{

    cout << "\n----------------------------------------";
    cout << "\nEvent 1: Mouse move";
    cout << "\nEvent 2: Left click";
    cout << "\nEvent 4: Double left click";
    cout << "\nEvent 5: Hold or release the left button ";
    cout << "\n----------------------------------------\n";

    SHORT state = GetAsyncKeyState(VK_LBUTTON);
    // cout << state << endl;
    switch (eventIndex)
    {
    // left click
    case 2:
        if (clkFlag)
        {
            mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            clkFlag = false;
        }
        break;

    // double left click
    case 4:
        if (clkFlag)
        {
            mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            clkFlag = false;
        }
        break;
    // hold or release left
    case 5:
        if (state == 0)
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        else
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    default:
        clkFlag = true;
        break;
    }
    // mouse move
    if (mouseMode == 0)
    {
        // joystick cursor
        if (!isOpRange(curposX, curposY))
        {
            int dx = curposX - cursorOri.x;
            int dy = curposY - cursorOri.y;
            POINT mousein;
            GetCursorPos(&mousein);
            SetCursorPos(double(mousein.x + dx * 0.5), double(mousein.y + dy * 0.5));
        }
    }
    else if (mouseMode == 1)
    {
        // global remap cursor
        SetCursorPos(double(curposX) / CameraResolution.first * ScreenResolution.first,
                     double(curposY) / CameraResolution.second * ScreenResolution.second);
    }
}

void keybdControl(int shape)
{
    cout << "\n--------------------------------";
    cout << "\nLine:      Tab";
    cout << "\nL Shape:   Enter";
    cout << "\nCircle:    Win + D";
    cout << "\nRectangle: Ctrl + Alt + Tab";
    cout << "\n--------------------------------\n";
    if (shape > 4) // Circle
    {
        // Win + D
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event(0x44, 0, 0, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(0x44, 0, KEYEVENTF_KEYUP, 0);
    }
    else
        switch (shape)
        {
        case 2: // Line
            // Tab
            keybd_event(VK_TAB, 0, 0, 0);
            keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
            break;
        case 3: // L Shape
            // Enter
            keybd_event(VK_RETURN, 0, 0, 0);
            keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
            break;
        case 4: // Rectangle
            // Ctrl + Alt + Tab
            keybd_event(VK_LCONTROL, 0, 0, 0);
            keybd_event(VK_MENU, 0, 0, 0);
            keybd_event(VK_TAB, 0, 0, 0);
            keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
            break;
        }
}
bool isOpRange(int curposX, int curposY)
{
    int dx = curposX - cursorOri.x;
    int dy = curposY - cursorOri.y;
    if ((dx * dx + dy * dy) > cursorRad * cursorRad)
    {
        return false;
    }
    return true;
}
#endif