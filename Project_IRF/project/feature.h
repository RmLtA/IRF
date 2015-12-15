#ifndef FEATURE_H_
#define FEATURE_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "utils.hpp"

#include <iostream>
using namespace std;
using namespace cv;

static int thresh = 145; //100

class feature{

    
public:
    //instance
    utils & u = utils::i();
    
    Mat sourceImg;
	Mat graySourceImg;
    Mat binaryImage;

    //TODO see if this is useful
    vector<Point> black_pixels;
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
    vector<Moments> imMoments;

    Point2f massCenter;

	
	feature(Mat img) : sourceImg(img){
        try{
         cvtColor( img, this->graySourceImg, CV_BGR2GRAY);
         massCenter = Point2f(-1,-1);
        }catch(Exception e){
            cout << e.msg <<endl;
            throw e;
        }
	}

	
	// Count the number of black pixels
	int countBlackPixel();

	//Count the number of white pixels;
	int countWhitePixel();

	// Count the number of Harris Corner
	int countHarrisCorners();
   // int findMoments();
    
    
    double massCenterX();
    double massCenterY();

    double HoughLines();
    double countLengthArea();
    double countArea();



    
private:
    void computeBlackPixels();
    void countMassCenter();


};

#endif /* FEATURE_H_ */
