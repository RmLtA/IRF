#include "feature.h"

// Count the number of black pixels
int feature::countBlackPixel(Mat src){
	if (!src.data)
		cerr << "Problem loading image from : countBlackPixel(Mat src)" << endl;

	if (src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = src;
	}

	// Transform it to binary and invert it. White on black is needed.
	threshold(gray, bw, 40, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

	// output, locations of non-zero pixels
	cv::findNonZero(bw, black_pixels);

	return black_pixels.size();
}

int feature::countWhitePixel(Mat src){

	countBlackPixel(src);
	// extract only the external blob
	findContours(bw.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	Mat mask = Mat::zeros(bw.size(), CV_8UC1);
	vector<Point> all_pixels;   // output, locations of non-zero pixels
	cv::findNonZero(mask, all_pixels);

	int white_pixels = all_pixels.size() - black_pixels.size();
	return white_pixels;
}

// Count the number of Harris Corner
int feature::countHarrisCorners(Mat src){

	Mat dst, dst_norm, dst_norm_scaled;
	dst = Mat::zeros(src.size(), CV_32FC1);

	/// Detector parameters
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;

	/// Detecting corners
	cornerHarris(gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

	/// Normalizing
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);

	int cornerHarris = 0;
	/// Drawing a circle around corners
	for (int j = 0; j < dst_norm.rows; j++)
	{
		for (int i = 0; i < dst_norm.cols; i++)
		{
			if ((int)dst_norm.at<float>(j, i) > thresh)
			{
				cornerHarris++;
			}
		}
	}

	return cornerHarris;
}

