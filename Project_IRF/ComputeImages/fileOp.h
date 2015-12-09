#ifndef FILEOP_H_
#define FILEOP_H_


#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>


#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/core.hpp>
#include <dirent.h>
#include "extractFeature.h"

using namespace std;
using namespace cv;






class fileOp{
   
    string dirSourceName;
    string dirTemplName  ;
    string dirResImagesName ;
    string dirResNormalizedName;
    string dirResSplitedName;
    


public:
    fileOp(bool isTest){
         dirSourceName = "sources/";
         dirTemplName   = "templates/";
         dirResImagesName = "results/";
         dirResNormalizedName = "results/";
         dirResSplitedName ="results/";
        
        string release = "release/";
        string test = "test/";
        
        if(isTest){
            dirSourceName += test; dirTemplName += test;
            
            dirResImagesName += test; dirResNormalizedName += test;
            dirResSplitedName +=test;

        }else{
            dirSourceName += release; dirTemplName += release;
            
            dirResImagesName +=release; dirResNormalizedName += release;
             dirResSplitedName +=release;
        }
        
        dirResImagesName += "images/";
        dirResNormalizedName+= "normalized/";
        dirResSplitedName +="splited/";
        
    };
    string getFilename(string name);
    string getExtName(string name);
    
    vector<string> getSourcesImages();
    vector<string> getTemplImages();
    vector<string> getResultImages();
    
 
    
    void writeTxtFile(string templ, string scripter_number, int row, int columm, Mat image, bool verbose = false);
    void addclasstov_class_icon(string current);
    
    void writeSplited(string nam, Mat img, bool verbose);
    void writeNormalized(string nam,Mat img,bool verbose);
    void writeARFFFile(extractFeature& extrfeat);

    //For features ! attention l'ordre ici est important, le même que dans le main pour v_features_available si modification
    //car c'est ce que l'utilisateur va prendre en compte car instructions imprimées à l'écran
    //Si ajout de features, ajouter une constante ! il y a un 2e enum dans extractFeature.h
    enum Features_Available_FileOp { BLACK_PIXEL=1, WHITE_PIXEL, AREA, CONTOURS_SIZE, HARRIS_CORNERS, LENGTHAREA, MASSCENTER, CLASS = INT_MAX };
    
private:
    vector<string> readDir(string dirName);
    
    //vector<string> find_image_name(void);




};
#endif /* FILE_OP_H_ */