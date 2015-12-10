#ifndef EXTRACT_FEATURE_H_
#define EXTRACT_FEATURE_H_

#include <iostream>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <ctime>
#include <algorithm>

#include "computeImages.h"
#include "feature.h"
#define NB_MAX_NUMERIC_FEATURE 9

class extractFeature{

public :
	
	vector<vector<double>> v_all_numeric_v_attributes_values;
	vector<string> v_class;
	vector<int> v_attributes_asked;
    int SPLITED;

	//Respect the order,  attention l'ordre ici est important, le même que dans le main pour v_features_available si modification
	//car c'est le numéro  que l'utilisateur va prendre en compte car instructions imprimées à l'écran
	//Si ajout de features, ajouter une constante ! il y a un 2e enum dans fileOp.h
	enum Features_Available { BLACK_PIXEL = 1, WHITE_PIXEL, AREA, CONTOURS_SIZE, HARRIS_CORNERS, LENGTHAREA, MASSCENTER_X, MASSCENTER_Y, CLASS = INT_MAX };

	extractFeature(int split){
        SPLITED = split;
	}

	~extractFeature(){}
	vector<int> getVectorAttributesAsked(){
		return v_attributes_asked;
	}
	
	double getSizeVectorClass(){
		return  v_class.size();
	}

	double  getSizeOfv_all_numeric(){
		return  v_all_numeric_v_attributes_values.size();
	}

	 double  getSizeOfv_attribute_asked(){
		return (v_attributes_asked).size();
	}

	double getSizeVectorOfOneAttribute(){
		return  v_all_numeric_v_attributes_values.size()/(v_attributes_asked).size();
	}

	void compute_features(vector<int>& v_of_attributes, vector<string>& v_result_images_toextract_features);
	vector<double> extract_feature_by_feature(int offset, int feature_attribute, vector<string>& v_result_images_toextract_features);
	void addclassto_v_class(vector<string>& v_result_images_toextract_features);
};

#endif /*EXTRACT_FEATURE_H_*/