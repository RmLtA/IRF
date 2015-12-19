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
    bool isGlobal;
    string imgName;
    
    Mat sourceImg;
	Mat graySourceImg;
    Mat binaryImage;

    //TODO see if this is useful
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
    vector<Moments> imMoments;

    Point2f massCenter;
    Point2f cornerHarrisPoint;

	
	feature(Mat img, bool _isGlobal, string _imgName) : sourceImg(img), isGlobal(_isGlobal), imgName(_imgName){
        try{
         cvtColor( img, this->graySourceImg, CV_BGR2GRAY);
            massCenter = Point2f(-1,-1);
            cornerHarrisPoint = Point2f(-1,-1);
        }catch(Exception e){
            cout << e.msg <<endl;
            throw e;
        }
	}

	
	// Count the number of black pixels
    //return % of black points
	double countBlackPixel();


	// Get the mass center of all the Harris Corners
    //return % of X pos
    double harrisCornerX();
    //return % of Y pos
    double harrisCornerY();

    //Get the mass center of the drawn image
    //return % of X pos
    double massCenterX();
    //return % of Y pos
    double massCenterY();
    
    

    double HoughLines();
    double countLengthArea();
    double countArea();
    
    //1 if rows are bigger than cols else 0;
    int isLongerRowsOrCols();


    
private:
    void countMassCenter();
    void countHarrisCorners();


};

#endif /* FEATURE_H_ */
