#ifndef OPENCV_GESTURE_H
#define OPENCV_GESTURE_H
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "hci_control.h"
// using namespace cv;
using cv::cvtColor;
using cv::imread;
using cv::imshow;
using cv::Mat;
using cv::Point;
using cv::Rect;
using cv::Scalar;
using cv::Size;
using cv::waitKey;
using std::cout;
using std::endl;
using std::vector;

// Global variables
int ctrlMode = 0;
int mouseMode = MOUSEMODE_BROWSE;
// Functions
// find bounding rectangle by BFS
void findRec(const Mat &srcImg, vector<Rect> &boundRect, int &count);
// find bounding rectangle by cv::boundingRect
void findRec2(const Mat &srcImg, vector<Rect> &boundRect, int &count);
// get skin segmentation image
void getSkinImg(Mat &inputImg, Mat &outputImg);
// gesture template matching
bool tmpPicMatch(const Mat *tmpPic, Mat &gesture, int &finger);
// main function
// int gesture();
int gesture()
{
	cv::VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cout<<"Open cam failed!!!"<<endl;
		return -1;
	}

	// load template pictures
	Mat tmpPic[5];
	tmpPic[0] = imread("../images/hand1.png", cv::IMREAD_GRAYSCALE);
	tmpPic[1] = imread("../images/hand2.png", cv::IMREAD_GRAYSCALE);
	tmpPic[2] = imread("../images/hand3.png", cv::IMREAD_GRAYSCALE);
	tmpPic[3] = imread("../images/hand4.png", cv::IMREAD_GRAYSCALE);
	tmpPic[4] = imread("../images/hand5.png", cv::IMREAD_GRAYSCALE);

	std::string textStr;
	bool clkFlg = true;
	Mat inputImg, outputImg, ROI;
	vector<Point> trajectory;
	while (true)
	{
		cap >> inputImg;
		getSkinImg(inputImg, outputImg);
		// find envelope squares
		int count = 0;
		vector<Rect> boundRect;
		findRec(outputImg, boundRect, count);
		// cout << count << " ";
		if (count > 1)
		{
			int finger = 0;
			int shape = 0;
			// gesture resize for template match
			Mat gesture = outputImg(boundRect[1]);
			cv::resize(gesture, ROI, Size(150, 256));

			// match templates
			tmpPicMatch(tmpPic, ROI, finger);
			if (ctrlMode == 0)
			{
				// mouse control
				mouseControl(finger, boundRect[1].x, boundRect[1].y, clkFlg, mouseMode);
				if (mouseMode == MOUSEMODE_BROWSE)
					cv::circle(inputImg, cursorOri, cursorRad, Scalar(0, 0, 255), 4);
			}
			else if (ctrlMode == 1)
			{
				// trajectory detect
				if (finger > 1)
				{
					trajectory.insert(trajectory.begin(), Point(boundRect[1].x, boundRect[1].y));
					for (size_t i = 0; i < trajectory.size() - 1; i++)
						line(inputImg, trajectory[i], trajectory[i + 1], Scalar(0, 0, 255), 2);
				}
				else
				{
					traDetect(trajectory, shape);
				}

				if (shape > 1)
				{
					// cout << shape << endl;
					keybdControl(shape);
				}
			}

			// draw markers
			Point gesPoint = Point(boundRect[1].x + boundRect[1].width / 2, boundRect[1].y);
			textStr = "Finger" + std::to_string(finger);
			// putText(outputImg, textStr, gesPoint, FONT_HERSHEY_DUPLEX, 1, Scalar(255, 0, 0), 2);
			cv::putText(inputImg, textStr, gesPoint, cv::FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0), 2);

			cv::rectangle(inputImg, boundRect[1], Scalar(0, 255, 0), 2);
		}
		imshow("input", inputImg);
		if (waitKey(30) == 27)
		{
			break;
		}
	}
	cv::destroyWindow("input");
	return 0;
}

void findRec(const Mat &srcImg, vector<Rect> &boundRect, int &count)
{
	if (srcImg.empty())
		return;
	vector<Point> pattern = {Point(-1, 0), Point(1, 0), Point(0, -1), Point(0, 1),
							 Point(-1, -1), Point(1, -1), Point(-1, 1), Point(1, 1)};
	int rows = srcImg.rows;
	int cols = srcImg.cols;
	int x1, x2, y1, y2;
	int width, height;
	Mat tmpImg;
	srcImg.convertTo(tmpImg, CV_32SC1);
	Point seed;
	for (int j = 1; j < cols - 1; j++)
	{
		for (int i = 1; i < rows - 1; i++)
		{
			if (tmpImg.at<int>(i, j) != 0)
				continue;
			x1 = x2 = j;
			y1 = y2 = i;
			std::queue<Point> bfs;
			bfs.push(Point(x1, y1));
			while (!bfs.empty())
			{
				seed = bfs.front();
				int tmpx = seed.x;
				int tmpy = seed.y;
				tmpImg.at<int>(tmpy, tmpx) = 255;
				bfs.pop();
				if ((tmpx > 0) && (tmpy > 0) && (tmpx < (cols - 1)) && (tmpy < (rows - 1)))
				{
					for (size_t i = 0; i < pattern.size(); i++)
					{
						tmpx = seed.x + pattern[i].x;
						tmpy = seed.y + pattern[i].y;
						if (tmpImg.at<int>(tmpy, tmpx) == 0)
						{
							bfs.push(Point(tmpx, tmpy));
							tmpImg.at<int>(tmpy, tmpx) = 128;
							if (tmpy > y2)
								y2 = tmpy;
							if (tmpy < y1)
								y1 = tmpy;
							if (tmpx > x2)
								x2 = tmpx;
							if (tmpx < x1)
								x1 = tmpx;
						}
					}
				}
			}
			width = x2 - x1 + 1;
			height = y2 - y1 + 1;
			if (((width) > 50) && ((width) < 300) &&
				((height) > 150) && ((height) < 450))
			{
				boundRect.push_back(Rect(x1, y1, width, height));
			}
		}
	}
	count = boundRect.size();
}

void findRec2(const Mat &srcImg, vector<Rect> &boundRect, int &count)
{
	Mat conImg;
	vector<vector<Point>> contours;
	vector<cv::Vec4i> hierarcy;
	cv::dilate(srcImg, conImg, Mat(), cv::Point(-1, -1), 2);
	cv::Canny(conImg, conImg, 80, 160);
	// imshow("canny", conImg);
	cv::findContours(conImg, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	for (size_t i = 0; i < contours.size(); i++)
	{
		Rect tmpRec = cv::boundingRect(contours[i]);
		int width = tmpRec.width;
		int height = tmpRec.height;
		if (((width) > 50) && ((width) < 300) &&
			((height) > 150) && ((height) < 450))
		{
			boundRect.push_back(tmpRec);
		}
	}
	count = boundRect.size();
}

void getSkinImg(Mat &inputImg, Mat &outputImg)
{
	Mat binImg, YCrCbImag, HSVImag;
	Mat Cr, Cb, H;
	vector<Mat> yChannels, hChannels;
	cv::flip(inputImg, inputImg, 1);
	cvtColor(inputImg, binImg, cv::COLOR_BGR2GRAY); // RGB to GRAY
	inputImg.copyTo(YCrCbImag);
	cvtColor(YCrCbImag, YCrCbImag, cv::COLOR_BGR2YCrCb); // RGB to YCrCb
	inputImg.copyTo(HSVImag);
	cvtColor(HSVImag, HSVImag, cv::COLOR_BGR2HSV); // RGB to HSV
	// channels separate
	cv::split(YCrCbImag, yChannels);
	cv::split(HSVImag, hChannels);
	Cr = yChannels.at(1); // get Cr
	Cb = yChannels.at(2); // get Cb
	H = hChannels.at(0);  // get H
	// skin region detect
	for (int j = 1; j < Cr.rows - 1; j++)
	{
		uchar *curCr = Cr.ptr<uchar>(j);
		uchar *curCb = Cb.ptr<uchar>(j);
		uchar *curH = H.ptr<uchar>(j);
		uchar *curImg = binImg.ptr<uchar>(j);

		for (int i = 1; i < Cb.cols - 1; i++)
		{
			if ((curCr[i] >= 135) && (curCr[i] <= 170) &&
				(curCb[i] >= 94) && (curCb[i] <= 125) &&
				(curH[i] >= 1) && (curH[i] <= 23))
				curImg[i] = 255;
			else
				curImg[i] = 0;
		}
	}
	// imshow("skin img", binImg);
	// watershed algorithm
	Mat markerImg(binImg.size(), CV_8U, Scalar(0));
	Mat fg, bg;
	cv::erode(binImg, fg, Mat(), Point(-1, -1), 7);
	cv::dilate(binImg, bg, Mat(), Point(-1, -1), 7);
	cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);
	markerImg = fg + bg;
	markerImg.convertTo(markerImg, CV_32SC1);
	cv::watershed(inputImg, markerImg);
	markerImg.convertTo(outputImg, CV_8UC1);
	// imshow("watershed img", outputImg);
}

bool tmpPicMatch(const Mat *tmpPic, Mat &gesture, int &finger)
{
	double minVal, maxVal;
	double tempMatch = 1e10;
	Point minLoc, maxLoc;
	Point matchLoc;
	Mat result;
	for (int index = 0; index < 5; index++)
	{
		cv::matchTemplate(gesture, tmpPic[index], result, cv::TM_SQDIFF_NORMED);
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
		if (tempMatch > minVal)
		{
			tempMatch = minVal;
			matchLoc = minLoc;
			finger = index + 1;
		}
	}
	if (finger == 0)
		return false;
	else
		return true;
}
#endif //OPENCV_GESTURE_H