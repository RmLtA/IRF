//
//  normalizeImages.hpp
//  ProjectIRF
//
//  Created by Cyrille Delabre on 08/12/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#ifndef normalizeImages_hpp
#define normalizeImages_hpp

#include <stdio.h>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <ctime>
#include "fileOp.h"
#include "computeImages.h"



using namespace std;
using namespace cv;

static const int MAX_SIZE = 140;
static const int THRESH_VALUE = 200; // /254
static const int IMG_GAP = 10; //Valeur de décalage (limite rectangle noir autours)

class normalizeImages{
public :
    const bool VERBOSE;
    const bool RESULT;;
    const int SPLIT_FACTOR;
    
    bool TEST;
    
    
    normalizeImages(bool verbose,bool result, bool test, int split ) : VERBOSE(verbose), RESULT(result) , TEST(test), SPLIT_FACTOR(split) {}
    ~normalizeImages(){};

    
    void process(bool saveNormalized);
private:
    Mat boundingBox(const cv::Mat& img,string imgName);
    Mat Box( Mat matInput, string imgName);
    Mat getSquareImage(const cv::Mat& img, string imgName);
    vector<Mat>  splitImage(int x, Mat const & src);
    


    
    
};

#endif /* normalizeImages_hpp */
