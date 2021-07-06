#include <opencv2/opencv.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME "Hci For You"
#include "gesture.h"
#include <iostream>
// using namespace cv;
// using namespace std;
extern int ctrlMode;
extern int mouseMode;
extern int faceMode;
extern int capMode;
int main(void)
{
    cv::Mat frame = cv::Mat(600, 500, CV_8UC3);
    // Init a OpenCV window and tell cvui to use it.
    cv::namedWindow(WINDOW_NAME);
    cvui::init(WINDOW_NAME);
    bool choice1 = false;
    bool choice2 = false;
    bool choice3 = false;
    bool choice4 = false;
    bool choice5 = false;
    bool choice6 = false;
    bool choice7 = false;
    while (true)
    {
        // Fill the frame with a nice color
        frame = cv::Scalar(43, 183, 179);
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
        if (cvui::button(frame, 30, 30, "gesture recognition"))
        {
            gesture();
        }

        cvui::text(frame, 30, 350, "---------shape recognition tips-----------");
        cvui::text(frame, 30, 370, "Line:      Tab");
        cvui::text(frame, 30, 390, "L Shape:   Enter");
        cvui::text(frame, 30, 410, "Circle:    Win + D");
        cvui::text(frame, 30, 430, "Rectangle: Ctrl + Alt + Tab");
        cvui::text(frame, 30, 470, "---------traject recognition tips---------");
        cvui::text(frame, 30, 490, "Event 1: Mouse move");
        cvui::text(frame, 30, 510, "Event 2: Left click");
        cvui::text(frame, 30, 530, "Event 4: Double left click");
        cvui::text(frame, 30, 550, "Event 5: Hold or release the left button");
        cvui::text(frame, 30, 570, "------------------------------------------");
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
