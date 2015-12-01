#ifndef FEATURE_H_
#define FEATURE_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;
using namespace cv;

static int thresh = 100;

class feature{

public:
	Mat sourceImg;
	Mat graySourceImg;
    Mat binaryImage;

    //TODO see if this is useful
    vector<Point> black_pixels;
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
    vector<Moments> imMoments;



	
	feature(Mat img) : sourceImg(img){
         cvtColor( img, this->graySourceImg, CV_BGR2GRAY);

	}

	//Divide the image into x parts;
	vector<Mat> splitImage(int);
	
	// Count the number of black pixels
	int countBlackPixel();

	//Count the number of white pixels;
	int countWhitePixel();

	// Count the number of Harris Corner
	int countHarrisCorners();
   // int findMoments();
    
    
    Point2f countMassCenter();
    double countLengthArea();
    double countArea();



    
private:
    void computeBlackPixels();


};

#endif /* FEATURE_H_ */
