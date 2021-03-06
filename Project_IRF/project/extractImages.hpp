//
//  extractImages.hpp
//  ProjectIRF
//
//  Created by Cyrille Delabre on 08/12/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#ifndef extractImages_hpp
#define extractImages_hpp

#include <stdio.h>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <ctime>
#include <iostream>
#include <thread>
#include <time.h>
#include <iomanip>
#include <mutex>
#include "fileOp.h"
#include "computeImages.h"



using namespace std;
using namespace cv;
const int MAXIMAGETTE = 7 ;

class extractImages{
private :
    utils & u = utils::i();
    unsigned long int leftToProcess;
    unsigned long int currentToProcess;

    unsigned long int toProcess;

    int nTotalImg;
    int nErroImg;
    
    static void processTask(extractImages& self,const vector<string>&  sourcesImages,const vector<string>& templatesImages);

public :

    extractImages()
    {
        nErroImg = 0;
        nTotalImg = 0;
    }
    
    void process();

    ~extractImages(){};


};



#endif /* extractImages_hpp */
