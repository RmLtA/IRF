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


/*
 // USAGE ::
 ./projetIRF -v [mode verbose]
 ./projetIRF -r [mode resultats]
 ./projetIRF -a [mode resultats + verbose]
 */
/* si en mode test (sources des images sources/test)
 ./projetIRF -a -test [mode resultats + verbose]
 */

int lastFactor =0;
static bool SAVE_NORMALIZED = true;
static bool DO_SQUARE= false;
utils & u = utils::i();

int main(int argc, const char * argv[]) {
    get_args(argc, argv);
    
    if(u.REMOVE_SOURCE){
        int approve = 0;
        cout << "Are you sure you wan't to remove all source files from " << (u.TEST ? "test" : "release") << " [ Yes : 1] "<< endl;
        cin >> approve;
        if(approve ==1){
            fileOp * op = new fileOp();
            op->removeAllResImagesFiles();
            op->removeAllResNormalizedFiles();
            op->removeAllResSplittedFiles();
        }
        cout << "Ending...  "<<endl;
        return 0;
    }
    
    if(u.EXTRACT_IMAGES){
        cout << "\nExtracting images..." <<endl;
        
        process_extract();
    }
    
    int nb =1;
    if(u.CREATE_VARIOUS && u.GET_FEATURES){
     
        cout << "How many Arff Files ? " << endl;
        cin >> nb;
    }
    
    
    
    for(int i=0 ; i<nb ; i++){
        cout << "Creating Arff : "<<i+1<<  endl;
        
        if (u.NORMALIZE){
            cout << "\nNormalizing images..." << endl;
            process_normalize();
        }
        if(u.GET_FEATURES){
            cout << "\nGetting features..." <<endl;
            process_features();
        }

        
        cout << "End of " << i+1 << endl;
        cout <<"~~~~~~~~~~~~~~~~"<<endl;
        
    }
    
	cout << "Finished..." << endl;


    
    waitKey(0);
    return 0; 
}

/// ./projetIRF -none -part2 -test

void get_args(int argc, const char * argv[]){
   // cout << argc << endl;;
    for(int i=1 ; i<argc ; i++)
    {
        string a = argv[i];
       // cout <<a << endl;
        if      (a== "-verbose"  || a=="-v") u.VERBOSE =true;
        else if (a=="-result"    || a=="-r") u.RESULT =true;
        else if (a=="-none") u.RESULT = u.VERBOSE =false;
        else if (a == "-e" || a =="-extract") u.EXTRACT_IMAGES =true;
        else if (a == "-n" || a == "-normalize") u.NORMALIZE = true;
        else if (a == "-f" || a =="-features") u.GET_FEATURES =true;
        else if (a == "-a" || a =="-all") u.GET_FEATURES = u.EXTRACT_IMAGES = u.NORMALIZE= true;
        else if (a == "-test" || a =="-t") u.TEST =true;
        else if (a == "-remove") u.REMOVE_SOURCE = true;
        else if (a == "-split" || a== "-s")
        {
            if(i+1 < argc){
                istringstream ss(argv[++i]);
                int x;
                if (!(ss >> x)){
                    cerr << "Invalid number " << argv[i] <<endl << " Exit." <<endl;;
                    exit(0);
                }else{
                    u.SPLIT_FACTOR = stoi(argv[i]);
                    lastFactor = u.SPLIT_FACTOR;
                    cout << "\n Split Factor is now  " << u.SPLIT_FACTOR << endl;
                }
            }else{
                cout << "Error in args -split" << endl;
            }
        }
        else if(a == "-help" || a== "-h")
        {
            cout << "Projet IRF " << endl;;
            cout << "Options : "<<endl;
            cout << "\t-verbose   | -v : verbose    "<< endl;;
            cout << "\t-result    | -r : resultats  "<< endl;;
            cout << "\t-test      | -t : fichier de test  "<< endl;;
            cout << "\t-split [x] | -s [x] : change split factor to x[int]" <<endl;

            cout << "Usage : "<<endl;
            cout << "\t-extract   | -e : extraire les images"<< endl;;
            cout << "\t-normalize | -n : normaliser les images"<< endl;;
            cout << "\t-features  | -f : extraire les features "<< endl;;
            
            cout << "Autres : "<<endl;
            

            cout << "\t-remove    |    : supprimer les fichiers resultats (a faire avant commit)"<< endl;;
            cout << "\t-help      | -h : cette page"<< endl;;
            exit(0);
        }
    }
    cout << " VERBOSE : " << u.VERBOSE;
    cout << " RESULT : " << u.RESULT;
    cout << " TEST : " << u.TEST <<endl;;
    cout << " EXTRACT_IMAGES : " << u.EXTRACT_IMAGES;
    cout << " NORMALIZE : " << u.NORMALIZE;
    cout << " GET_FEATURES : " << u.GET_FEATURES;

  
    cout <<endl;

}



void process_extract()
{
    extractImages * e = new extractImages();
    e->process();
    delete e;
    
}

void process_normalize()
{
    int split_factor = u.SPLIT_FACTOR;
    
    cout << "Split images in ? (1..4..9..16..25) :";
    cin >> split_factor;
    while(split_factor != 1 && split_factor != 4 && split_factor != 9 && split_factor != 16 && split_factor != 25){
        cout << "\nOnly 1 or 4 or 9 or 16 or 25: " << flush;
        cin >> split_factor;
    }
    
    u.SPLIT_FACTOR = split_factor;
    cout << "OK, Split in : " << u.SPLIT_FACTOR << endl;
    //TODO optimiser
    fileOp * op = new fileOp();
    vector<string> splittedImages = op->getSplitedImages();
    //PREVENT TO SPLIT IF SAME AGAIN
    if(lastFactor!= u.SPLIT_FACTOR)
    {
        
        if(u.VERBOSE)cout <<"Save Normalized is set to : " << SAVE_NORMALIZED << endl;;
        normalizeImages * n = new normalizeImages(SAVE_NORMALIZED, DO_SQUARE);
        lastFactor =u.SPLIT_FACTOR;

        n->process();
        delete n;
    }else
    {
        cout << "Same as before, no need to reprocess" << endl;
    }
    
    delete op;
	
}


void process_features()
{    
    fileOp *  op = new fileOp();
    try{
        
  

        //Name of Images used to extract featires
        //ON UTILISE LES IMAGES NORMALIZEES
        //VOIR IMAGES SPLITTED
        vector<string> v_result_images_toextract_features_splited = op->getSplitedImages();
        vector<string> v_result_images_toextract_features_global = op->getNormalizedImages();

        
        if(v_result_images_toextract_features_splited.size() % u.SPLIT_FACTOR != 0)
        {
            cout << "error : Splitted images are not equivalent to SPLIT_FACTOR" << endl;
            cout << "try again with NORMALIZE to reconstruct images (-normalize) or FEATURES with options [-features -split 9] to tell last nb of splitted images " <<endl;
            return;
        }
        
        if(v_result_images_toextract_features_global.size()== 0)
        {
            cout << "error : Your are not saving normalized images" << endl;
            cout << "change save normalized to true  " <<endl;
            return;
        }
        
        

        //Print features available, ! attention l'ordre ici est important, le même que pour l'enum Features_available si modification
        //car c'est ce que l'utilisateur va prendre en compte car instructions imprimées à l'écran
        //ici features pour les images splited
        vector<string> v_features_available_splited = {
            "Black_Pixel          ",
            "Area                 ",
            "Harris_Corners       ",
            "Mass_center          ",
            "Hough_Lines          ",
            "Hough_Circles        "

        };

        //Features to extract
        vector<int> v_features_to_extract_splited;
        cout <<endl<< "SPLITED FEATURES" <<endl;
        cout << "All features available for splited image :" << endl;
        for (int i = 0; i<v_features_available_splited.size();i++){
            cout << "   -"<<v_features_available_splited[i] << "\t\t\t : Tape "<< i+1<< endl;
        }
        cout << "To finish : Tape 0" << endl;

        int features_toextract;
        cin >> features_toextract;
        while (features_toextract != 0) {
            if(features_toextract <= v_features_available_splited.size())
                v_features_to_extract_splited.push_back(features_toextract);
            cin >> features_toextract;
            
        }
        
        //ici features pour les images globales
        //peuvent etre différentes ==
        //idée ajouter longueur ou largeur image est plus grande ? (1 ou 0 ) (différentie deja quelques images...)
        //et implementer...
        vector<string> v_features_available_global= {
            "Black_Pixel          ",
            "Area                 ",
            "Harris_Corners       ",
            "Mass_center          ",
            "Hough_Lines          ",
            "Hough_Circles        ",
            "Rows or Cols Longer  "

        };
        
        //Features to extract
        vector<int> v_features_to_extract_global;
        cout <<endl<< "GLOBAL FEATURES" <<endl;
        cout << "All features available for the global image :" << endl;
        for (int i = 0; i<v_features_available_global.size();i++){
            cout << "   -"<<v_features_available_global[i] << "\t\t\t : Tape "<< i+1<< endl;
        }
        cout << "To finish : Tape 0" << endl;
        
        cin >> features_toextract;
        while (features_toextract != 0) {
            if(features_toextract <= v_features_available_global.size())
                v_features_to_extract_global.push_back(features_toextract);
            cin >> features_toextract;
            
        }

        
        cout << "Ok, computing... "<<endl;
       
        extractFeature * extract_feature = new extractFeature();
        
        
        extract_feature->compute_features(v_features_to_extract_splited,v_result_images_toextract_features_splited,
                                          v_features_to_extract_global, v_result_images_toextract_features_global);

        op->writeARFFFile(*extract_feature);
        
        delete extract_feature;
        
        
    }catch(Exception e){
        cout << "Error in part two..." << endl;
        cout << e.what() << endl;
    }
    
    
    delete op;
    
}

