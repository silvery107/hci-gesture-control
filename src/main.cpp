#include <opencv2/opencv.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME "HCI Gesture Control"
#include "gesture.h"
#include <iostream>
extern int ctrlMode;
extern int mouseMode;
using cv::Mat;
using cvui::button;
using cvui::checkbox;
using cvui::text;

int main(void)
{
    Mat frame = Mat(430, 400, CV_8UC3);
    bool choice1 = true;
    bool choice2 = false;
    bool choice3 = true;
    bool choice4 = false;

    cvui::init(WINDOW_NAME);
    cv::namedWindow(WINDOW_NAME);
    cvui::watch(WINDOW_NAME);

    while (true)
    {
        frame = cv::Scalar(67, 63, 0);

        if (checkbox(frame, 30, 70, "Shape Recognition", &choice1))
        {
            ctrlMode = 0;
            choice2 = false;
        }
        if (checkbox(frame, 50, 90, "Rad Dot", &choice3))
        {
            mouseMode = 0;
            choice1 = true;
            choice2 = false;
            choice4 = false;
        }
        if (checkbox(frame, 50, 110, "Touch Pad", &choice4))
        {
            mouseMode = 1;
            choice1 = true;
            choice2 = false;
            choice3 = false;
        }
        if (checkbox(frame, 30, 130, "Trajectory Recognition", &choice2))
        {
            ctrlMode = 1;
            choice1 = false;
            choice3 = false;
            choice4 = false;
        }
        if (button(frame, 30, 30, "Gesture Recognition", cvui::DEFAULT_FONT_SCALE, 0x2BB7B3))
        {
            gesture();
        }

        text(frame, 30, 180, "---------Shape Recognition Tips----------");
        text(frame, 30, 200, "Line: Tab");
        text(frame, 30, 220, "L Shape: Enter");
        text(frame, 30, 240, "Circle: Win + D");
        text(frame, 30, 260, "Rectangle: Ctrl + Alt + Tab");
        text(frame, 30, 300, "---------Traject Recognition Tips----------");
        text(frame, 30, 320, "Finger 1: Mouse move");
        text(frame, 30, 340, "Finger 2: Left click");
        text(frame, 30, 360, "Finger 4: Double left click");
        text(frame, 30, 380, "Finger 5: Hold or release the left button");
        text(frame, 30, 400, "----------------------------------");
        cvui::update();
        // Show everything on the screen
        cvui::imshow(WINDOW_NAME, frame);
        // Check if ESC key was pressed
        if (cv::waitKey(20) == 27)
        {
            break;
        }
    }
    cv::destroyWindow(WINDOW_NAME);
    return EXIT_SUCCESS;
}
