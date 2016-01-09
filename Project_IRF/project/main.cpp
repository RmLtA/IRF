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
extractFeature *  process_features(bool once, bool global);
void process_normalize(int norm, bool ask);
void process_features_all();

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
        exit(1);
    }
    
    if(u.EXTRACT_IMAGES){
        cout << "\nExtracting images..." <<endl;
        
        process_extract();
    }
       
    int nb =1;
    if((u.CREATE_VARIOUS && u.GET_FEATURES) || u.ALL){
        cout << "How many features files ? (" << (u.ARFF ? "Arff " : "") << (u.CSV ? "CSV " : "") << "file)"<< endl;
        cin >> nb;
    }
    
    
    for(int i=0 ; i<nb ; i++){
        
        if(u.ALL){
            process_features_all();
        }else{
            cout << "Creating feature file : "<<i+1<<  endl;
            
            if (u.NORMALIZE){
                cout << "\nNormalizing images..." << endl;
                process_normalize(0, true);
            }
            if(u.GET_FEATURES){
                cout << "\nGetting features..." <<endl;
                process_features(true, true);
            }
 
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
        else if (a == "-csv") {u.CSV = true ; u.ARFF = false ; }
        else if (a == "-arff") {u.ARFF = true;}
        else if (a == "-bw") {u.BW = true;}
        else if (a== "-af" || a =="-all_features"){u.ALL =true;}

        
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
            
            cout << "\t-csv       |    : creer un fichier csv "<< endl;;
            cout << "\t-arff      |   : creer un fichier arff "<< endl;;
            cout << "\t-csv -arff |   : creer un fichier csv et arff "<< endl;;


            cout << "\t-remove    |    : supprimer les fichiers resultats (a faire avant commit)"<< endl;;
            cout << "\t-help      | -h : cette page"<< endl;;
            exit(0);
        }
    }
    if(u.RESULT)
    {
        cout << " VERBOSE : " << u.VERBOSE;
        cout << " RESULT : " << u.RESULT;
        cout << " TEST : " << u.TEST <<endl;;
        cout << " EXTRACT_IMAGES : " << u.EXTRACT_IMAGES;
        cout << " NORMALIZE : " << u.NORMALIZE;
        cout << " GET_FEATURES : " << u.GET_FEATURES;
    }

  
    cout <<endl;

}



void process_extract()
{
    extractImages * e = new extractImages();
    e->process();
    delete e;
    
}

void process_normalize(int norm, bool ask)
{
    if(norm == 0){
        int split_factor;
        cout << "Split images in ? (1..4..9..16..25) : (last : " << lastFactor  <<" | -1 to force split) " <<endl;
        cin >> split_factor;
        while(split_factor != 1 && split_factor != 4 && split_factor != 9 && split_factor != 16 && split_factor != 25){
            lastFactor =-1;
            cout << "\n \t Force split in  1 or 4 or 9 or 16 or 25: " << flush;
            cin >> split_factor;
        }
        
        u.SPLIT_FACTOR = split_factor;
        
        //TODO optimiser
        //PREVENT TO SPLIT IF SAME AGAIN
        if(lastFactor!= u.SPLIT_FACTOR)
        {
            
            cout << "OK, Split in : " << u.SPLIT_FACTOR << endl;
            normalizeImages * n = new normalizeImages(DO_SQUARE, ask);
            lastFactor =u.SPLIT_FACTOR;
            
            n->process();
            delete n;
        }else
        {
            cout << "Same as before, no need to reprocess" << endl;
        }

    }else{
        u.SPLIT_FACTOR = norm;

        cout << "OK, Split in : " << u.SPLIT_FACTOR << endl;
        normalizeImages * n = new normalizeImages(DO_SQUARE, ask);
        
        n->process();
        delete n;
    }
    
	
}


extractFeature * process_features(bool once, bool global)
{    
    try{
        fileOp *  op = new fileOp();

        //Name of Images used to extract featires
        //ON UTILISE LES IMAGES NORMALIZEES
        //VOIR IMAGES SPLITTED
        vector<string> v_result_images_toextract_features_splited = op->getSplitedImages();
        vector<string> v_result_images_toextract_features_global = op->getNormalizedImages();
    
        if(v_result_images_toextract_features_splited.size() % u.SPLIT_FACTOR != 0 && u.SPLIT_FACTOR != 1)
        {
            cout << "error : Splitted images are not equivalent to SPLIT_FACTOR" << endl;
            cout << "try again with NORMALIZE to reconstruct images (-normalize) or FEATURES with options [-features -split 9] to tell last nb of splitted images " <<endl;
            exit(0);
        }
        
        if(v_result_images_toextract_features_global.size()== 0)
        {
            cout << "error : Your are not saving normalized images" << endl;
            cout << "change save normalized to true  " <<endl;
            exit(0);
            
        }
        
        vector<int> v_features_to_extract_splited;
        int features_toextract;

        if(once){
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
            //if we choosed to split images
            if(u.SPLIT_FACTOR >1){
                //Features to extract
                cout <<endl<< "SPLITED FEATURES" <<endl;
                cout << "All features available for splited image :" << endl;
                for (int i = 0; i<v_features_available_splited.size();i++){
                    cout << "   -"<<v_features_available_splited[i] << "\t\t\t : Tape "<< i+1<< endl;
                }
                cout << "To finish : Tape 0" << endl;
                
                cin >> features_toextract;
                while (features_toextract != 0) {
                    if(features_toextract <= v_features_available_splited.size())
                        v_features_to_extract_splited.push_back(features_toextract);
                    cin >> features_toextract;
                    
                }
            }

        }else{
            v_features_to_extract_splited = {1,2,3,4,5,6};
        }
        vector<int> v_features_to_extract_global;
    
        if(global){
            if(once){
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
                    "Rows or Cols Longer  ",
                    "All Pixels (only squares)"
                };
                
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
                
                //getting size of image to calculate pixels
                Mat img = imread(v_result_images_toextract_features_global[0]);
                if(img.rows >= img.cols){
                    u.SIZE_IMAGE = img.rows;
                }else{
                    u.SIZE_IMAGE = img.cols;
                }
                
            }else{
                v_features_to_extract_global = {1,2,3,4,5,6,7};

            }

        }
        
        
        cout << "Ok, computing... "<<endl;
       
        extractFeature * extract_feature = new extractFeature();
        
        
        extract_feature->compute_features(v_features_to_extract_splited,v_result_images_toextract_features_splited,
                                          v_features_to_extract_global, v_result_images_toextract_features_global);
        
        
        if(once){
            if(u.CSV)op->writeCSV(*extract_feature);
            
            if(u.ARFF)op->writeARFFFile(*extract_feature);
            
  
        }
        
        delete op;
        if(once){
            delete extract_feature;
            return NULL;

        }
        else
        {
            return extract_feature;
        }
        
            



    }catch(Exception e){
        cout << "Error in part two..." << endl;
        cout << e.what() << endl;
    }
    
    
    
    return NULL;
}


void process_features_all()
{
    int splited_image[] = {4,9,16,25};
    vector<extractFeature> vector_extr_features;
    cout << "Split in 4" << endl;
    process_normalize(4, false);
    extractFeature * first_4 = process_features(false, true);
    vector_extr_features.push_back(*first_4);

    cout << "Split in 9" << endl;

    process_normalize(9, false);
    extractFeature * first_9 = process_features(false, false);
    vector_extr_features.push_back(*first_9);
    cout << "Split in 16" << endl;
    process_normalize(16, false);
    extractFeature * first_16 = process_features(false, false);
    vector_extr_features.push_back(*first_16);
    
    
    cout << "Split in 25" << endl;
    process_normalize(25, false);
    extractFeature * first_25 = process_features(false, false);
    vector_extr_features.push_back(*first_25);

    
    fileOp *  op = new fileOp();
    op->writeARFFFileVariousSplitted(vector_extr_features,splited_image);
//    process_normalize(4);
//    extractFeature * first_4 = process_features(false);
    
}

