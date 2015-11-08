#ifndef COMPUTEIMAGES_H_
#define COMPUTEIMAGES_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;
using namespace cv;


static int matchMethod =CV_TM_CCOEFF_NORMED;

struct templateArea{
    Mat image;
    string name;
    int position;
};

inline bool compareStruct(templateArea a,templateArea b){
    return a.position<b.position;
}


class computeImages{

public:
    Mat sourceImg;
    int match_method;
    Mat processingImg;
    Point templPos;
    bool verbose;
    
    
    computeImages(Mat img, bool ver) : sourceImg(img), verbose(ver) , match_method(matchMethod)
    {
        processingImg = sourceImg.clone();
    }

    vector<Vec4i>  findLines(Mat imgSource);
    vector<Mat> findImages(vector<Vec4i> lines, Mat imgSource);
    bool findTemplArea(Mat templ, string currentName);
    
    Mat getTemplArea();
    int getPositionY();

    void removeZone();
    void showFinalImage(string imageName);

private:
    Mat currentTemplArea;
    int currentPosition;
    Mat getTemplateMatchingZone();

    
    static bool rectComparator(std::vector<cv::Point2f>& a, std::vector<cv::Point2f>& b);
    static bool pointsComparator(Point2f a,Point2f b);
    void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center);
    Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b) ;
    
    ~computeImages(){};

};



#endif /* COMPUTEIMAGES_H_ */