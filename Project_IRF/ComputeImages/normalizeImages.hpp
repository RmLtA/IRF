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

class normalizeImages{
public :
    const bool VERBOSE;
    const bool RESULT;;
    bool TEST;
    
    
    normalizeImages(bool verbose,bool result, bool test ) : VERBOSE(verbose), RESULT(result) , TEST(test) {}
    ~normalizeImages(){};

    void process();
private:
    Mat boundingBox(Mat src,string imgName);
    Mat Box( Mat matInput, string imgName);
    
    
    
};

#endif /* normalizeImages_hpp */
