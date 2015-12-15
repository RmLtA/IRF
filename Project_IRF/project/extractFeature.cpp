#include "extractFeature.h"

//Si ajout de features, pas besion de modification

//v_features_to_extract == vector des features
//v_result_images_toextract_features = vector des images
void extractFeature::compute_features(vector<int>& v_of_attributes, vector<string>& v_result_images_toextract_features){

	//delete duplicates in v_of_attributes 
	sort(v_of_attributes.begin(), v_of_attributes.end());
	v_of_attributes.erase(unique(v_of_attributes.begin(), v_of_attributes.end()), v_of_attributes.end());
    
    //nombre d'attributs par feature.
    int value_of_attributes =0;
	//affectation des attributs demandés
	for (int  i = 0; i < v_of_attributes.size()-1; i++){
		//if (v_of_attributes[i]<= NB_MAX_NUMERIC_FEATURE)
		v_attributes_asked.push_back(v_of_attributes[i]);
        value_of_attributes += nb_features_by_attributes[v_of_attributes[i]];
        
	}
    v_all_numeric_v_attributes_values = vector<vector<double>>(v_result_images_toextract_features.size()/SPLITED, vector<double>(value_of_attributes*SPLITED));

	

	//add values to vector of attributes
    int nextRealImage = 0;
    int nbFeatures = (int) value_of_attributes;
    //pour toutes les images
    for (unsigned long i = 0; i < v_result_images_toextract_features.size(); i=i+SPLITED)
    {
         for(int offset=0 ; offset< SPLITED ; offset++){
             //pour chaque petite image de l'image
             string imgName =v_result_images_toextract_features[i+offset];
             //on extrait toutes les features
             extract_all_features(imgName, nextRealImage , offset*nbFeatures);
         }
        nextRealImage++;
        cout << "." << flush;
    }
    
    
    
//    
//	vector<vector<double>> v_all_v_attributes_values;
//	for (int it = 0; it < (v_attributes_asked).size(); it++){
//        cout << v_attributes_asked[it];
//        //on extrait les features en fonction de la partie de l'image concernŽe
//        for(int offset=0 ; offset< SPLITED ; offset++){
//            cout << "." << flush;
//            vector<double> values =extract_feature_by_feature(offset, v_attributes_asked[it], v_result_images_toextract_features);
//            v_all_numeric_v_attributes_values.push_back(values);
//        }
//       
//	}
    cout <<endl;
	addclassto_v_class(v_result_images_toextract_features);
}


void extractFeature::extract_all_features(string imgName, int nextImage, int currentPos){
    Mat img = imread(imgName);
    if(img.rows ==0 || img.cols==0){
        cout << "Extract Feature:: error with " << imgName << endl;
        return; 
    }
    feature* f = new feature(img);
    vector<double> feature_result;

    for(int j = 0 ; j < (v_attributes_asked).size() ; j++)
    {
        int feature_attribute = v_attributes_asked[j];
        switch (feature_attribute){
                
            case BLACK_PIXEL:
                 feature_result.push_back(f->countBlackPixel());
                break;
            case WHITE_PIXEL:
                feature_result.push_back(f->countWhitePixel());;
                break;
            case AREA:
                feature_result.push_back(f->countArea());
                break;
            case CONTOURS_SIZE:
                feature_result.push_back(f->countLengthArea());
                break;
            case HARRIS_CORNERS:
                feature_result.push_back(f->countHarrisCorners());
                break;
            case LENGTHAREA:
                feature_result.push_back(f->countLengthArea());
                break;
            case MASSCENTER:
                feature_result.push_back(f->massCenterX());
                feature_result.push_back(f->massCenterY());
                break;
            case HOUGH_LINES:
                feature_result.push_back(f->HoughLines());
                break;

            default:
                cout << "Unknown feature" << endl;
                throw "error";
                break;
        }
    }
    //on enregistre la feature au bon endroit...
    for(int i= 0 ; i<feature_result.size(); i++)
        v_all_numeric_v_attributes_values[nextImage][currentPos+i] = feature_result[i];

}
//
////Si ajout de features, Modification ICI, AJOUT D'un case
//vector<double> extractFeature::extract_feature_by_feature(int offset , int feature_attribute, vector<string>& v_result_images_toextract_features){
//	vector<double> v_attributes_values;
//    
//    
//    for (int i = offset; i < v_result_images_toextract_features.size(); i=i+SPLITED)
//    {
//        string imgName =v_result_images_toextract_features[i];
//        //cout << imgName << endl;;
//        Mat img = imread(imgName);
//        feature* f = new feature(img);
//
//        switch (feature_attribute){
//            case BLACK_PIXEL:
//                v_attributes_values.push_back(f->countBlackPixel());
//                break;
//            case WHITE_PIXEL:
//                v_attributes_values.push_back(f->countWhitePixel());
//                break;
//            case AREA:
//                v_attributes_values.push_back(f->countArea());
//                break;
//            case CONTOURS_SIZE:
//                v_attributes_values.push_back(f->countLengthArea());
//                break;
//            case HARRIS_CORNERS:
//                v_attributes_values.push_back(f->countHarrisCorners());
//                break;
//            case LENGTHAREA:
//                v_attributes_values.push_back(f->countLengthArea());
//                break;
//            case MASSCENTER:
//                v_attributes_values.push_back(f->massCenterX());
//                break;
////            case MASSCENTER_Y:
////                v_attributes_values.push_back(f->massCenterY());
////                break;
//            default:
//                cout << "Unknown feature" << endl;
//
//                break;
//        }
//    }
//
//    return v_attributes_values;
//}
//
//No modification on adding features
void extractFeature::addclassto_v_class(vector<string>& v_result_images_toextract_features){
	for (int i = 0; i < v_result_images_toextract_features.size(); i=i+SPLITED){
		size_t point0 = v_result_images_toextract_features[i].find_last_of("/");
		string res = v_result_images_toextract_features[i].substr(point0 + 1, v_result_images_toextract_features[i].size());
		size_t point = res.find_first_of("_");
		v_class.push_back(v_result_images_toextract_features[i].substr(point0+1, point));
	}
}

