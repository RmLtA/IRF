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

static const int MAX_SIZE = 130; //si choix faire carré avec image
static const int THRESH_VALUE = 230; // /254
static const int IMG_GAP = 10; //valeur de décalage (limite rectangle noir autours)
static const int MIN_SIZE = 40; //valeur minimum de selection (sinon trop de bruit)
class normalizeImages{
public :
    utils & u = utils::i();;
    bool squareImg;
    int sizeImg;
    double moy_cols, moy_rows;
    double max_cols, max_rows;
    double min_cols, min_rows;
    bool askSquared;

    unsigned long int leftToProcess;
    unsigned long int currentToProcess;
    int nErroImg;

    normalizeImages(bool simg, bool ask) : squareImg(simg), sizeImg(MAX_SIZE), askSquared(ask){
        moy_cols = 0, moy_rows = 0; nErroImg = 0;
        min_cols = min_rows = 1000;
        max_cols = max_rows = 0;
        
    };
    
    
    ~normalizeImages(){};

    
    void process();
private:
    static void processTask(normalizeImages& self,vector<string> resultImages);
    static vector<string> getImages();
    
    static Mat boundingBox(const cv::Mat& img,string imgName);
    Mat Box( Mat matInput, string imgName);
    static Mat getSquareImage(const cv::Mat& img, string imgName, int sizeImg);
    static vector<Mat>  splitImage(int x, Mat const & src);
    
};

#endif /* normalizeImages_hpp */
