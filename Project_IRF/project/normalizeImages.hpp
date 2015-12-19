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
#include <iomanip>
#include <thread>
#include <mutex>
#include <iostream>
#include "fileOp.h"
#include "computeImages.h"
#include "utils.hpp"


using namespace std;
using namespace cv;

static const int MAX_SIZE = 130;
static const int THRESH_VALUE = 220; // /254
static const int IMG_GAP = 10; //Valeur de décalage (limite rectangle noir autours)

class normalizeImages{
public :
    utils & u = utils::i();;
    const bool saveNormalized;
    const bool squareImg;
    double moy_cols, moy_rows;

    unsigned long int leftToProcess;
    unsigned long int currentToProcess;
    int nErroImg;

    normalizeImages(bool snormalized, bool simg) : saveNormalized(snormalized), squareImg(simg){
        moy_cols = 0, moy_rows = 0; nErroImg = 0;
    };
    
    
    ~normalizeImages(){};

    
    void process();
private:
    static void processTask(normalizeImages& self,vector<string> resultImages);

    static Mat boundingBox(const cv::Mat& img,string imgName);
    Mat Box( Mat matInput, string imgName);
    static Mat getSquareImage(const cv::Mat& img, string imgName);
    static vector<Mat>  splitImage(int x, Mat const & src);
    


    
    
};

#endif /* normalizeImages_hpp */
