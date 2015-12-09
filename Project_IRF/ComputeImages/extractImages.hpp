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
#include "fileOp.h"
#include "computeImages.h"



using namespace std;
using namespace cv;

class extractImages{
public :
    const bool VERBOSE;
    const bool RESULT;;
    bool TEST;
    const int MAXIMAGETTE = 7 ;

    
    extractImages(bool verbose,bool result, bool test ) : VERBOSE(verbose), RESULT(result) , TEST(test) {}
    
    void process();
    ~extractImages(){};


    

};




static string niceOutput(string output, bool ok){
    if(false){
        stringstream ss;
        int  FG_RED      = 31;
        int FG_GREEN    = 32;
        ss << "\033[";
        if(ok){
            ss << FG_GREEN << "m" << output;
        }else{
            ss << FG_RED << "m"  << output;
        }
        
        ss << " \033[0m";
        return ss.str();
    }
   return output;
}

#endif /* extractImages_hpp */
