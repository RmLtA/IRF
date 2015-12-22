#ifndef EXTRACT_FEATURE_H_
#define EXTRACT_FEATURE_H_

#include <iostream>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <algorithm>
#include <thread>
#include <time.h>
#include <iomanip>
#include <mutex>

#include "computeImages.h"
#include "feature.h"
#define NB_MAX_NUMERIC_FEATURE 9

class extractFeature{

public :
    
    
   
	
	vector<vector<double>> v_all_numeric_v_attributes_values;
	vector<string> v_class;
	vector<int> v_attributes_asked_splited;
    vector<int> v_attributes_asked_global;

	//Respect the order,  attention l'ordre ici est important, le même que dans le main pour v_features_available si modification
	//car c'est le numéro  que l'utilisateur va prendre en compte car instructions imprimées à l'écran
	//Si ajout de features, ajouter une constante ! il y a un 2e enum dans fileOp.h
	enum Features_Available_Splited
    {
        BLACK_PIXEL = 1,
        AREA,
        HARRIS_CORNERS,
        MASSCENTER,
        HOUGH_LINES,
        HOUGH_CIRCLES
    };
    enum Features_Available_Global
    {
        BLACK_PIXEL_GLOBAL =1,
        AREA_GLOBAL,
        HARRIS_CORNERS_GLOBAL,
        MASSCENTER_GLOBAL,
        HOUGH_LINES_GLOBAL,
        HOUGH_CIRCLES_GLOBAL,
        ROWS_OR_COLS_GLOBAL
    };



    vector<int> nb_features_by_attributes_splited;
    vector<int> nb_features_by_attributes_global;
	extractFeature(){
        //nombre de features differentes par features (voir mass center par exemple (2)
        nb_features_by_attributes_splited = { 0 ,1 ,11 ,2 ,2 ,5, 4};
        nb_features_by_attributes_global =  { 0 ,1 ,11 ,2 ,2 ,5, 4, 1};
        
	}
    

	~extractFeature(){}
    
    
    void compute_features(vector<int>& v_of_attributes_splited, vector<string>& v_result_images_toextract_features_splited,
                          vector<int>& v_of_attributes_global, vector<string>& v_result_images_toextract_features_global);
    
    string getSplitedFeatureName(int f);
    string getGlobalFeatureName(int f);


    
private:
    static void processTask(extractFeature& self, vector<string> v_result_images_toextract_features_splited, vector<string> v_result_images_toextract_features_global);

    int nbSplitedFeatures;
    int nbGlobalFeatures;
    unsigned long int leftToProcess;
    unsigned long int currentToProcess;
    unsigned long int toProcess;
    
    void extract_all_features_splited(string imgName, int i, int offset);
    void extract_all_features_global(string imgName, unsigned int currImage);
    vector<double> extract_feature_by_feature(int offset, int feature_attribute, vector<string>& v_result_images_toextract_features);

    void addclassto_v_class(vector<string>& v_result_images_toextract_features);

};

#endif /*EXTRACT_FEATURE_H_*/