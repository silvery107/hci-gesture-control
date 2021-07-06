# Acknowlegment

## 论文参考

[1]余超. 基于视觉的手势识别研究[D]. 中国科学技术大学, 2015.

[2]沙亮. 基于无标记全手势视觉的人机交互技术[D]. 清华大学, 2010.

[3]黄俊.基于视觉的手势识别交互系统设计[D]. 中央民族大学, 2019.

## 函数参考

### 以下函数根据论文中所提思路完善并优化

### 1. 根据 H-Cb-Cr 肤色模型及论文实验参数提取肤色图像

```C++
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
	imshow("skin img", binImg);
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
	imshow("watershed img", outputImg);
}
```

## 2. 肤色外接矩形提取

**参考论文中提出的栈实现种子填充算法，我们使用队列实现广度优先搜索算法，完成肤色图像矩形包络的准确提取。**

```C++
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
```

**同时我们也通过opencv原生函数实现了矩形包络提取。**

```C++
void findRec2(const Mat &srcImg, vector<Rect> &boundRect, int &count)
{
	Mat conImg;
	vector<vector<Point>> contours;
	vector<cv::Vec4i> hierarcy;
	cv::dilate(srcImg, conImg, Mat(), cv::Point(-1, -1), 2);
	cv::Canny(conImg, conImg, 80, 160);
	imshow("canny", conImg);
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
```

## 库引用

```C++
#include "cvui.h"
```
