#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int HoughDetection(const Mat& src_gray, const Mat& src_display, int cannyThreshold, int accumulatorThreshold)
{
		// will hold the results of the detection
		std::vector<Vec3f> circles;
		// runs the actual detection
		HoughCircles(src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows / 8, cannyThreshold, accumulatorThreshold, 0, 0);

		// clone the colour, input image for displaying purposes
		int numberOfcircles = 0;
		Mat display = src_display.clone();
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			// circle center
			circle(display, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			// circle outline
			circle(display, center, radius, Scalar(0, 0, 255), 3, 8, 0);

			numberOfcircles++;
		}

		// shows the results
		imshow("resultat", display);
		//return the number of circles
		return numberOfcircles;
		
}
/** @function main */
int main(int argc, char** argv)
{
	Mat src, src_gray;

	/// Read the image
	src = imread("test4.jpg");

	if (!src.data)
	{
		return -1;
	}
	/// Reduce the noise so we avoid false circle detection
	GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);
	/// Convert it to gray
	cvtColor(src, src_gray, CV_BGR2GRAY);
	//runs the detection, and update the display
	int accumulatorThreshold = 30;
	int cannyThreshold = 97;
	
	int number=HoughDetection(src_gray, src, cannyThreshold, accumulatorThreshold);
	cout << number;
	//
	waitKey(0);
	return 0;
}
