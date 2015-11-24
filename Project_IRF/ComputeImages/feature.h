#ifndef FEATURE_H_
#define FEATURE_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;
using namespace cv;

class feature{

public:
	Mat sourceImg;
	Mat gray;
	Mat bw;
	vector<Point> black_pixels;
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;


	int thresh = 200;

	
	feature(Mat img) : sourceImg(img){
	}

	// Count the number of black pixels
	int countBlackPixel(Mat img);

	//Count the number of white pixels;
	int countWhitePixel(Mat img);

	// Count the number of Harris Corner
	int countHarrisCorners(Mat img);

};

#endif /* FEATURE_H_ */