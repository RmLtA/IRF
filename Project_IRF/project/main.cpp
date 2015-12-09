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

void process_extract();
void process_features();
void process_normalize();


using namespace std;
using namespace cv;


//debugging on console
static bool VERBOSE = false;
static bool RESULT = true;

/*unused*/
//use folder test or default folder
static  bool TEST = true;
static bool REMOVE_SOURCE = false;


static bool EXTRACT_IMAGES = true;
static bool GET_FEATURES = true;
static bool NORMALIZE = true;
/*
 // USAGE ::
 ./projetIRF -v [mode verbose]
 ./projetIRF -r [mode resultats]
 ./projetIRF -a [mode resultats + verbose]
 */
/* si en mode test (sources des images sources/test)
 ./projetIRF -a -test [mode resultats + verbose]
 */

int SPLIT_FACTOR = 4;
static bool SAVE_NORMALIZED = false;


int main(int argc, const char * argv[]) {

    get_args(argc, argv);
    if(REMOVE_SOURCE){
        int approve = 0;
        cout << "Are you sure you wan't to remove all source files from " << (TEST ? "test" : "release") << " [ Yes : 1] "<< endl;
        cin >> approve;
        if(approve ==1){
            fileOp * op = new fileOp(TEST);
            op->removeAllResImagesFiles();
            op->removeAllResNormalizedFiles();
            op->removeAllResSplittedFiles();
        }
        cout << "Ending...  "<<endl;
        return 0;
    }
    
    
    if(NORMALIZE || GET_FEATURES)
    {
        int split_factor = SPLIT_FACTOR;
        
        cout << " Split images in ? (1..4..9..16) :";
        cin >> split_factor;
        while(split_factor != 1 && split_factor != 4 && split_factor != 9 && split_factor != 16){
            cout << "\nOnly 1 or 4 or 9 or 16: ";
            cin >> split_factor;
        }
        
        SPLIT_FACTOR = split_factor;
        cout << "\nOK, Split in : " << SPLIT_FACTOR << endl;
    }
    
    
    if(EXTRACT_IMAGES){
        if(VERBOSE || RESULT)cout << "\nExtracting images..." <<endl;
        
        process_extract();
    }
    if (NORMALIZE){
        if (VERBOSE || RESULT)cout << "\nNormalizing images..." << endl;
        process_normalize();
    }
    if(GET_FEATURES){
        if(VERBOSE || RESULT)cout << "\nGetting features..." <<endl;
        process_features();
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
        else if(a == "-e" || a =="-extract") EXTRACT_IMAGES =true;
        else if (a == "-n" || a == "-normalize") NORMALIZE = true;
        else if(a == "-f" || a =="-features") GET_FEATURES =true;
        else if(a == "-a" || a =="-all") GET_FEATURES = EXTRACT_IMAGES = NORMALIZE= true;
        else if(a == "-test" || a =="-t") TEST =true;
        else if(a == "-remove"){
         
        }
        
    }
    cout << " VERBOSE : " << VERBOSE;
    cout << " RESULT : " << RESULT;
    cout << " TEST : " << TEST <<endl;;
    cout << " EXTRACT_IMAGES : " << EXTRACT_IMAGES;
    cout << " NORMALIZE : " << NORMALIZE;
    cout << " GET_FEATURES : " << GET_FEATURES;

  
    cout <<endl;

}



void process_extract()
{
    extractImages * e = new extractImages(VERBOSE, RESULT, TEST);
    e->process();
    delete e;
    
}

void process_normalize()
{
 
    
	normalizeImages * n = new normalizeImages(VERBOSE, RESULT, TEST, SPLIT_FACTOR);
   
    bool saveNormalized = SAVE_NORMALIZED;
    cout <<"Save Normalized is set to : " << SAVE_NORMALIZED << endl;;
    n->process(saveNormalized);
    delete n;
}


void process_features()
{    
    fileOp *  op = new fileOp(TEST);
    try{
        
  

        //Name of Images used to extract featires
        //ON UTILISE LES IMAGES NORMALIZEES
        //VOIR IMAGES SPLITTED
        vector<string> v_result_images_toextract_features = op->getSplitedImages();
        
        if(v_result_images_toextract_features.size() % SPLIT_FACTOR != 0)
        {
            cout << "error : Splitted images are not equivalent to SPLIT_FACTOR" << endl;
            cout << "try again with NORMALIZE (-n -normalize) option to reconstruct images" <<endl;
            return;
        }

        //Print features available, ! attention l'ordre ici est important, le même que pour l'enum Features_available si modification
        //car c'est ce que l'utilisateur va prendre en compte car instructions imprimées à l'écran
        vector<string> v_features_available = {"Black_Pixel","White_Pixel","Airs","Contours_Size","Harris_Corners", "Length Area"};

        //Features to extract
        vector<int> v_features_to_extract;
        cout << "All features available :" << endl;
        for (int i = 0; i<v_features_available.size();i++){
            cout << "   -"<<v_features_available[i] << " : Tape "<< i+1<< endl;
        }
        cout << "To finish : Tape 0" << endl;

        

        int features_toextract;
        cin >> features_toextract;
        while (features_toextract != 0){
            v_features_to_extract.push_back(features_toextract);
            cin >> features_toextract;
            
        }
        
        cout << "Ok, features are :"<<endl;
        for (int i = 0; i<v_features_to_extract.size();i++){
            cout << "\t| "<<v_features_to_extract[i] ;
        }
        //Pour mettre l'attribut class à la fin
        v_features_to_extract.push_back(INT_MAX);
        cout << "\ncomputing ... " << endl;


        extractFeature * extract_feature = new extractFeature(SPLIT_FACTOR);
        
        //v_features_to_extract == vector des features
        //v_result_images_toextract_features = vector des images
        extract_feature->compute_features(v_features_to_extract,v_result_images_toextract_features);

        op->writeARFFFile(*extract_feature);
        
        delete extract_feature;
        
        
    }catch(Exception e){
        cout << "Error in part two..." << endl;
        cout << e.what() << endl;
    }
    
    
    delete op;
    
}

