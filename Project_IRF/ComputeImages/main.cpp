//
//  main.cpp
//  projectIRF
//
//  Created by Cyrille Delabre on 21/10/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#include <iostream>
#include "computeImages.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/core.hpp>
#include<dirent.h>
#include<string.h>
using namespace std;
using namespace cv;
FILE* f;
static int matchMethod = TM_SQDIFF_NORMED;
static string dirSourceName  = "sources/";
static string dirTemplName  = "templates/";
static string dirResImagesName  = "results/images/";
static string dirResSquaresName  = "results/squares/";



vector<string> readDir(string dirName);
string getFilename(string name);

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "IRF Project... \n";
    
    cout << "Looking all the images from sources "<< dirSourceName <<" \n";
    cout << "Looking all the templates images from templates "<< dirTemplName <<" \n";

    
    /// Load image and template
    vector<string> sourceDir = readDir(dirSourceName);
    vector<string> templDir = readDir(dirTemplName);
    
    for(int i = 0 ; i < sourceDir.size() ; i ++ )
    {
        cout << "processing image : " << sourceDir[i] << endl;
        string sourceName  = getFilename(sourceDir[i]);
        
        Mat img = imread(dirSourceName + sourceDir[i]);
        computeImages * ll = new computeImages(img, matchMethod);
        
        //for all the input images from the directory
        for (int j = 0 ; j < templDir.size(); j++) {
            cout << "template : "<<dirTemplName + templDir[j] << endl;
            string templName  = getFilename(templDir[j]);

            //get the img
            Mat templ = imread(dirTemplName + templDir[j]);
            //search the template area
            Mat area  = ll->findTemplArea(templ);
            //get the lines from the area
            vector<Vec4i> lines =  ll->findLines(area);
            
            //save the image with lines
            Mat areaLine = area.clone();
            for(size_t i = 0; i < lines.size(); i++ ) {
                Vec4i l = lines[i];
                line(areaLine, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
            }
            stringstream ss;
            ss<<dirResSquaresName << sourceName << "-" << templName <<".jpg";
            imwrite(ss.str(), areaLine);
            cout << "saving lines from : " << templName << " in " << ss.str() << endl;

            //get the images
            vector<Mat> result =  ll->findImages(lines, area);
            
            //save the images
            for (int k =0 ; k < result.size(); k++) {
                stringstream ss;
                ss<<dirResImagesName << sourceName << "-" << templName << "-" << k<<".jpg";
                imwrite(ss.str(), result[k]);
                cout << "saving  : "<<ss.str() << endl;

            }
            
        }
    }
    cout << "done chef ! "<<endl;
    
  
  waitKey(0);
    
    return 0;
}

vector<string> readDir(string dirName){
    vector<string> output ;
    DIR *dir;
    dir = opendir(dirName.c_str());
    struct dirent *ent;
    if (dir != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            string imgName =ent->d_name;
            if(imgName.compare(".")!= 0 && imgName.compare("..")!= 0){
                output.push_back (imgName) ;
            }
        }
        closedir (dir);
    } else {
        cout<< dirName << " not present... "<<endl;
        exit(0);
    }
    return output;
}

string getFilename(string name){
    std::size_t found = name.find_last_of(".");
    return  name.substr(0,found);
}
