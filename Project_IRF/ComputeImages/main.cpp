//
//  main.cpp
//  projectIRF
//

#include <iostream>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <ctime>


#include "fileOp.h"
#include "extractImages.hpp"
#include "normalizeImages.hpp"

#include "feature.h"


/* Utility functions*/
void get_args(int argc, const char * argv[]);
/* Process functions*/

void process_part_one();
void process_part_two();
void process_part_three();


using namespace std;
using namespace cv;


//debugging on console
static bool VERBOSE = true;
static bool RESULT = true;

/*unused*/
//use folder test or default folder
static  bool TEST = true;



static bool PART_1 = false;
static bool PART_2 = false;
static bool PART_3 = true;
/*
 // USAGE ::
 ./projetIRF -v [mode verbose]
 ./projetIRF -r [mode resultats]
 ./projetIRF -a [mode resultats + verbose]
 */
/* si en mode test (sources des images sources/test)
 ./projetIRF -a -test [mode resultats + verbose]
 */




int main(int argc, const char * argv[]) {

    get_args(argc, argv);
    
    
    if(PART_1){
        if(VERBOSE)cout << "Part one..." <<endl;
        process_part_one();
    }
    if(PART_2){
        if(VERBOSE)cout << "Part two..." <<endl;
        process_part_two();
    }
    if (PART_3){
		if (VERBOSE)cout << "Part three..." << endl;
		process_part_three();
	}
	cout << "Finished..." << endl;


    
    waitKey(0);
    return 0; 
}

/// ./projetIRF -none -part2 -test

void get_args(int argc, const char * argv[]){
   // cout << argc << endl;;
        for(int i=1 ; i<argc ; i++){
            string a = argv[i];
           // cout <<a << endl;
            if      (a== "-verbose"  || a=="-v") VERBOSE =true;
            else if (a=="-result"    || a=="-r") RESULT =true;
            else if (a=="-none"      || a=="-n") RESULT = VERBOSE =false;
            else if(a == "-1" || a =="-part1") PART_1 =true;
            else if(a == "-2" || a =="-part2") PART_2 =true;
            else if (a == "-3" || a == "-part3") PART_3 = true;
            else if(a == "-a" || a =="-all") PART_2 = PART_1 = PART_3= true;
            else if(a == "-test" || a =="-t") TEST =true;
            
        }
    cout << "VERBOSE : " << VERBOSE;
    cout << " RESULT : " << RESULT;
    cout << " PART1 : " << PART_1;
    cout << " PART2 : " << PART_2;
    cout << " TEST : " << TEST;
    cout <<endl;

}



void process_part_one()
{
    extractImages * e = new extractImages(VERBOSE, RESULT, TEST);
    e->process();
    delete e;
    
}




void process_part_three()
{
	normalizeImages * n = new normalizeImages(VERBOSE, RESULT, TEST);
    
    n->process();
    delete n;
}


void process_part_two()
{
    fileOp *  op = new fileOp(TEST);
    try{
        
        
        
        //A décommenter en cas de test
        // Test feature for black_pixel
        vector<string> resultImagesToComputeBlackPixelAttribute = op->getResultImages();
        cout << "Taille result : "  << resultImagesToComputeBlackPixelAttribute.size() << endl;
        
        string current;
        //add attributes
        op->v_attributes.push_back("black_pixel");
        op->v_attributes.push_back("HarrisCorners");
        op->v_attributes.push_back("Airs");
        op->v_attributes.push_back("Taille_coutours");
        op->v_attributes.push_back("class");
        
        //add values to vector of attributes
        for (int i = 0; i < resultImagesToComputeBlackPixelAttribute.size(); i++){
            Mat img = imread(resultImagesToComputeBlackPixelAttribute[i]);
            current = op->getFilename(resultImagesToComputeBlackPixelAttribute[i]);
            cout << "Process... : " << current<< endl;
            cout << i << endl;
            
            
            //ajout de la classe de l'icon
            //op->addclasstov_class_icon(current);
            
            //ajout des features
            feature* f = new feature(img);
            vector<Mat> v = f->splitImage(16);
            
            /*
             op->v_nb_black_pixels.push_back(f->countBlackPixel());
             op->v_nb_harris_corners.push_back(f->countHarrisCorners());
             op->v_nb_area.push_back(f->countArea());
             op->v_nb_lenght.push_back(f->countLengthArea());
             */
            
        }
        
        // op->writeARFFFile();
        
        
        
    }catch(Exception e){
        cout << "Error in part two..." << endl;
        cout << e.what() << endl;
    }
    
    
    delete op;
    
}

