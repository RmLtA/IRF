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
string getExtName(string name);


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
        
        cout << "///////////////"<< endl;
        cout << "processing image : " << sourceDir[i] << endl;
        cout << "///////////////"<< endl;

        string sourceName  = getFilename(sourceDir[i]);
        
        Mat img = imread(dirSourceName + sourceDir[i]);
        computeImages * ll = new computeImages(img, matchMethod);
        
        //for all the input images from the directory
        for (int j = 0 ; j < templDir.size(); j++) {
            cout << "template : "<<dirTemplName + templDir[j] << endl;
            string templName  = getFilename(templDir[j]);
          
            
            stringstream currentNameStream;
            currentNameStream<<dirResSquaresName << sourceName << "-" << templName <<".jpg";
            
            //get the img
            Mat templ = imread(dirTemplName + templDir[j]);
            try{
            //search the template area
                
                if(ll->findTemplArea(templ, currentNameStream.str())){
                    Mat area  = ll->getTemplArea();
                    //get the lines from the area
                    vector<Vec4i> lines =  ll->findLines(area);
                    
                    //save the image with lines
                    Mat areaLine = area.clone();
                    for(size_t i = 0; i < lines.size(); i++ ) {
                        Vec4i l = lines[i];
                        line(areaLine, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
                    }
                    imwrite(currentNameStream.str(), areaLine);
                    cout << "saving lines from : " << templName << " in " << currentNameStream.str() << endl;
                    
                    //get the images
                    vector<Mat> result =  ll->findImages(lines, area);
                    
                    //save the images
                    for (int k =0 ; k < result.size(); k++) {
                        stringstream ss;
                        ss<<dirResImagesName << sourceName << "-" << templName << "-" << k<<".jpg";
                        imwrite(ss.str(), result[k]);
                       // cout << "saving  : "<<ss.str() << endl;
                        
                    }

                }else{
                    cout << "not found...  "<< endl;
 
                }
            }catch(Exception e){
                
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
            if(getExtName(imgName) == ".jpg" || getExtName(imgName) == ".png"){
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
string getExtName(string name){
    std::size_t found = name.find_last_of(".");
    return  name.substr(found, name.length());
}
