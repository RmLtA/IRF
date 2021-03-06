#ifndef FILEOP_H_
#define FILEOP_H_


#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdio.h>



#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/core.hpp>
#include <dirent.h>
#include "extractFeature.h"
#include "utils.hpp"

using namespace std;
using namespace cv;




class fileOp{
   
    string dirSourceName;
    string dirTemplName  ;
    string dirResImagesName ;
    string dirResNormalizedName;
    string dirResSplitedName;
    string dirResArffName;
    string dirResCSVName;
    string dirResByName;

    utils & u = utils::i();


public:
    fileOp(){
         dirSourceName = "sources/";
         dirTemplName   = "templates/";
         dirResImagesName = "results/";
         dirResNormalizedName = "results/";
         dirResSplitedName ="results/";
         dirResArffName ="arff/";
         dirResCSVName ="csv/";
        dirResByName= "results/";
        
        
        string release = "release/";
        string test = "test/";
        
        if(u.TEST){
            dirSourceName += test; dirTemplName += test;
            dirResImagesName += test; dirResNormalizedName += test;
            dirResSplitedName +=test;
            dirResArffName +=test;
            dirResCSVName +=test;
            dirResByName +=test;

        }else{
            dirSourceName += release; dirTemplName += release;
            dirResImagesName +=release; dirResNormalizedName += release;
            dirResSplitedName +=release;
            dirResArffName+=release;
            dirResCSVName +=release;
            dirResByName +=release;
        }
        
        dirResImagesName += "images/";
        dirResNormalizedName+= "normalized/";
        dirResSplitedName +="splited/";
        dirResByName +="by_name/";
        
    };
    string getFilename(string name);
    string getExtName(string name);
    
    vector<string> getSourcesImages();
    vector<string> getTemplImages();
    vector<string> getResultImages();
    vector<string> getNormalizedImages();
    vector<string> getSplitedImages();

    
    void removeAllResImagesFiles();
    void removeAllResNormalizedFiles();
    void removeAllResSplittedFiles();
 
    
    void writeTxtFile(string templ, string scripter_number, int row, int columm, Mat image);
    void addclasstov_class_icon(string current);
    
    void writeSplited(string nam, Mat img);
    void writeNormalized(string nam,Mat img);
    void writeARFFFile(extractFeature& extrfeat);
    void writeARFFFileVariousSplitted(vector<extractFeature>  vector_extrfeat, int splited_image[]);
    void writeCSV(extractFeature& extrfeat);
    void writeByName(string nam, Mat img);


private:
    vector<string> readDir(string dirName);
    string setFeatureFilename(extractFeature& extrfeat, bool isCSV, bool oneSplit);
    int label_to_i(string label);




};
#endif /* FILE_OP_H_ */
