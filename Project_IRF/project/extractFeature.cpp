#include "extractFeature.h"

//Si ajout de features, pas besion de modification

//v_features_to_extract == vector des features
//v_result_images_toextract_features = vector des images
void extractFeature::compute_features(vector<int>& v_of_attributes, vector<string>& v_result_images_toextract_features){

	//delete duplicates in v_of_attributes 
	sort(v_of_attributes.begin(), v_of_attributes.end());
	v_of_attributes.erase(unique(v_of_attributes.begin(), v_of_attributes.end()), v_of_attributes.end());

	//affectation des attributs demandés
	for (int i = 0; i < v_of_attributes.size(); i++){
		if (v_of_attributes[i]<= NB_MAX_NUMERIC_FEATURE)
		v_attributes_asked.push_back(v_of_attributes[i]);
	}
	

	//add values to vector of attributes
	vector<vector<double>> v_all_v_attributes_values;
	for (int it = 0; it < (v_attributes_asked).size(); it++){
        //on extrait les features en fonction de la partie de l'image concernŽe
        for(int offset=0 ; offset< SPLITED ; offset++){
            vector<int> values =extract_feature_by_feature(offset, v_attributes_asked[it], v_result_images_toextract_features);
            v_all_numeric_v_attributes_values.push_back(values);
        }
       
	}
	addclassto_v_class(v_result_images_toextract_features);
}


//Si ajout de features, Modification ICI, AJOUT D'un case
vector<int> extractFeature::extract_feature_by_feature(int offset , int feature_attribute, vector<string>& v_result_images_toextract_features){
	vector<int> v_attributes_values;
    for (int i = offset; i < v_result_images_toextract_features.size(); i=i+SPLITED)
    {
        string imgName =v_result_images_toextract_features[i];
        //cout << imgName << endl;;
        Mat img = imread(imgName);
        feature* f = new feature(img);

        switch (feature_attribute){
            case BLACK_PIXEL:
                v_attributes_values.push_back(f->countBlackPixel());
                break;
            case WHITE_PIXEL:
                v_attributes_values.push_back(f->countWhitePixel());
                break;
            case AREA:
                v_attributes_values.push_back(f->countArea());
                break;
            case CONTOURS_SIZE:
                v_attributes_values.push_back(f->countLengthArea());
                break;
            case HARRIS_CORNERS:
                v_attributes_values.push_back(f->countHarrisCorners());
                break;
            case LENGTHAREA:
                v_attributes_values.push_back(f->countLengthArea());
                break;
            /*case MASSCENTER:
                v_attributes_values.push_back(f->countMassCenter());
                break*/
            default:
                cout << "Unknown feature" << endl;

                break;
        }
    }

    return v_attributes_values;
}

//No modification on adding features
void extractFeature::addclassto_v_class(vector<string>& v_result_images_toextract_features){
	for (int i = 0; i < v_result_images_toextract_features.size(); i++){
		size_t point0 = v_result_images_toextract_features[i].find_last_of("/");
		string res = v_result_images_toextract_features[i].substr(point0 + 1, v_result_images_toextract_features[i].size());
		size_t point = res.find_first_of("_");
		v_class.push_back(v_result_images_toextract_features[i].substr(point0+1, point));
	}
}

