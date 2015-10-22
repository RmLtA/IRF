

#ifndef COMPUTEIMAGES_H_
#define COMPUTEIMAGES_H_


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;
using namespace cv;



/*
*
*/

class computeImages{

public:
    Mat sourceImg;
    Mat result;
    int match_method;
    
    computeImages(Mat img, int match) : sourceImg(img) , match_method(match) {}

    vector<Vec4i>  findLines(Mat imgSource);
    vector<Mat> findImages(vector<Vec4i> lines, Mat imgSource);
    Mat findTemplArea(Mat templ);
private:
    static bool rectComparator(std::vector<cv::Point2f>& a, std::vector<cv::Point2f>& b);
    static bool pointsComparator(Point2f a,Point2f b);
    void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center);
    Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b) ;

};



#endif /* COMPUTEIMAGES_H_ */