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


using namespace std;
using namespace cv;






class fileOp{
     string dirSourceName;
     string dirTemplName;
     string dirResImagesName;
     string dirResSquaresName;

public:
    fileOp(bool isTest){
        dirSourceName = "sources/";
        dirTemplName  = "templates/";
        dirResImagesName  = "results/images/";
        dirResSquaresName  = "results/squares/";
        
        
        if(isTest){
            dirSourceName += "test/";
            dirTemplName += "test/";
        }
    };
    string getFilename(string name);
    string getExtName(string name);
    
    vector<string> getSourcesImages();
    vector<string> getTemplImages();
    
    void writeTxtFile(string templ, string scripter_number, int row, int columm, Mat image, bool verbose = false);
    
private:
    vector<string> readDir(string dirName);
    
   	//vector<string> find_image_name(void);




};
#endif /* FILE_OP_H_ */