#include <opencv2/opencv.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME "Hci For You"
#include "gesture.h"
#include <iostream>
extern int ctrlMode;
extern int mouseMode;
int main(void)
{
    cv::Mat frame = cv::Mat(480, 400, CV_8UC3);
    // Init a OpenCV window and tell cvui to use it.
    cv::namedWindow(WINDOW_NAME);
    cvui::init(WINDOW_NAME);
    bool choice1 = false;
    bool choice2 = false;
    bool choice3 = false;
    bool choice4 = false;
    while (true)
    {
        // Fill the frame with a nice color
        frame = cv::Scalar(67, 63, 0);
        // Show a button at position (110, 80)
        if (choice1)
        {
            ctrlMode = 0;
        }
        cvui::checkbox(frame, 30, 70, "shape recognition", &choice1);
        if (choice3)
        {
            mouseMode = 0;
        }
        cvui::checkbox(frame, 50, 90, "Trackpoint", &choice3);
        if (choice4)
        {
            mouseMode = 1;
        }
        cvui::checkbox(frame, 50, 110, "Touchpad", &choice4);
        if (choice2)
        {
            ctrlMode = 1;
        }
        cvui::checkbox(frame, 30, 130, "trajectory recognition", &choice2);
        if (cvui::button(frame, 30, 30, "gesture recognition",cvui::DEFAULT_FONT_SCALE, 0x2BB7B3))
        {
            gesture();
        }

        cvui::text(frame, 30, 220, "---------Shape Recognition Tips-----------");
        cvui::text(frame, 30, 240, "Line:      Tab");
        cvui::text(frame, 30, 260, "L Shape:   Enter");
        cvui::text(frame, 30, 280, "Circle:    Win + D");
        cvui::text(frame, 30, 300, "Rectangle: Ctrl + Alt + Tab");
        cvui::text(frame, 30, 340, "----------Traject Recognition Tips----------");
        cvui::text(frame, 30, 360, "Event 1: Mouse move");
        cvui::text(frame, 30, 380, "Event 2: Left click");
        cvui::text(frame, 30, 400, "Event 4: Double left click");
        cvui::text(frame, 30, 420, "Event 5: Hold or release the left button");
        cvui::text(frame, 30, 440, "--------------------------------");
        cvui::update();
        // Show everything on the screen
        cv::imshow(WINDOW_NAME, frame);
        // Check if ESC key was pressed
        if (cv::waitKey(20) == 27)
        {
            break;
        }
    }
    return 0;
}
