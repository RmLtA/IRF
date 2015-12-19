#include "extractFeature.h"
#include <iomanip>

//Si ajout de features, pas besion de modification

//v_features_to_extract == vector des features
//v_result_images_toextract_features = vector des images
void extractFeature::compute_features(vector<int>& v_of_attributes_splited, vector<string>& v_result_images_toextract_features_splited,
                                      vector<int>& v_of_attributes_global, vector<string>& v_result_images_toextract_features_global){

	//delete duplicates in v_of_attributes 
	sort(v_of_attributes_splited.begin(), v_of_attributes_splited.end());
	v_of_attributes_splited.erase(unique(v_of_attributes_splited.begin(), v_of_attributes_splited.end()), v_of_attributes_splited.end());
    
    //delete duplicates in v_of_attributes
    sort(v_of_attributes_global.begin(), v_of_attributes_global.end());
    v_of_attributes_global.erase(unique(v_of_attributes_global.begin(), v_of_attributes_global.end()), v_of_attributes_global.end());
    
    
    
    //nombre d'attributs par feature splited.
    int nbSplitedFeatures =0;
	for (int  i = 0; i < v_of_attributes_splited.size(); i++){
		v_attributes_asked_splited.push_back(v_of_attributes_splited[i]);
        nbSplitedFeatures += nb_features_by_attributes_splited[v_of_attributes_splited[i]];
	}
    //nombre d'attributs par feature global.
    int nbGlobalFeatures =0;
    for (int  i = 0; i < v_of_attributes_global.size(); i++){
        v_attributes_asked_global.push_back(v_of_attributes_global[i]);
        nbGlobalFeatures += nb_features_by_attributes_global[v_of_attributes_global[i]];
    }
    
    int nbImgTotal = (int) v_result_images_toextract_features_global.size();
    
    
    //creation du tableau ˆ la bonne dimension
    //+1 is class
    v_all_numeric_v_attributes_values = vector<vector<double>>(nbImgTotal, vector<double>(nbSplitedFeatures*SPLITED+nbGlobalFeatures));

	

    
    //pour toutes les images
    for (unsigned int i = 0; i < nbImgTotal; i++)
    {
        //on extrait les features globales en premier lieu
        string imgGlobalName = v_result_images_toextract_features_global[i];
        extract_all_features_global(imgGlobalName, i);
        //puis les features splited
        for(int offset=0 ; offset< SPLITED ; offset++){
             //pour chaque petite image de l'image
            
             string imgName =v_result_images_toextract_features_splited[i*SPLITED+offset];
             //on extrait toutes les features
             extract_all_features_splited(imgName, i , offset*nbSplitedFeatures+nbGlobalFeatures);
         }
        
        
        //affichage
        double pr=((double)i/(double)nbImgTotal)*100;
        cout<<setprecision(2)<< pr<<"%   \r"<< flush ;
    }
    
    

    cout <<endl;
    //on ajoute la classe
	addclassto_v_class(v_result_images_toextract_features_global);
}


void extractFeature::extract_all_features_splited(string imgName, int nextImage, int currentPos){
    Mat img = imread(imgName);
    if(img.rows ==0 || img.cols==0){
        cout << "Extract Feature:: error with " << imgName << endl;
        return; 
    }
    //feature :: is global ? = false
    feature* f = new feature(img, false, imgName);
    vector<double> feature_result;

    for(int j = 0 ; j < v_attributes_asked_splited.size() ; j++)
    {
        int feature_attribute = v_attributes_asked_splited[j];
        switch (feature_attribute){
                
            case BLACK_PIXEL:
                 feature_result.push_back(f->countBlackPixel());
                break;
            case AREA:
                feature_result.push_back(f->countArea());
                break;
            case HARRIS_CORNERS:
                feature_result.push_back(f->harrisCornerX());
                feature_result.push_back(f->harrisCornerY());
                break;
            case LENGTH_AREA:
                feature_result.push_back(f->countLengthArea());
                break;
            case MASSCENTER:
                feature_result.push_back(f->massCenterX());
                feature_result.push_back(f->massCenterY());
                break;
            case HOUGH_LINES:
                feature_result.push_back(f->houghLinesVerticals());
                feature_result.push_back(f->houghLinesHorizontals());
                feature_result.push_back(f->houghLinesDiagonalPos());
                feature_result.push_back(f->houghLinesDiagonalNegs());

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
    
    delete f;

}

void extractFeature::extract_all_features_global(string imgName, unsigned int currImage){
    Mat img = imread(imgName);
    if(img.rows ==0 || img.cols==0){
        cout << "Extract Feature:: error with " << imgName << endl;
        return;
    }
    //feature :: is global = true
    feature* f = new feature(img, true, imgName);
    vector<double> feature_result;
    
    for(int j = 0 ; j < v_attributes_asked_global.size() ; j++)
    {
        int feature_attribute = v_attributes_asked_global[j];
        switch (feature_attribute){
                
            case BLACK_PIXEL_GLOBAL:
                feature_result.push_back(f->countBlackPixel());
                break;
            case AREA_GLOBAL:
                feature_result.push_back(f->countArea());
                break;
            case HARRIS_CORNERS_GLOBAL:
                feature_result.push_back(f->harrisCornerX());
                feature_result.push_back(f->harrisCornerY());
                break;
            case LENGTH_AREA_GLOBAL:
                feature_result.push_back(f->countLengthArea());
                break;
            case MASSCENTER_GLOBAL:
                feature_result.push_back(f->massCenterX());
                feature_result.push_back(f->massCenterY());
                break;
            case HOUGH_LINES_GLOBAL:
                feature_result.push_back(f->houghLinesVerticals());
                feature_result.push_back(f->houghLinesHorizontals());
                feature_result.push_back(f->houghLinesDiagonalPos());
                feature_result.push_back(f->houghLinesDiagonalNegs());

                break;
            case ROWS_OR_COLS_GLOBAL:
                feature_result.push_back(f->isLongerRowsOrCols());
                break;
                
            default:
                cout << "Unknown feature" << endl;
                throw "error";
                break;
        }
    }
    //on enregistre la feature au bon endroit...
    for(int i= 0 ; i<feature_result.size(); i++)
        v_all_numeric_v_attributes_values[currImage][i] = feature_result[i];
    
    delete f;
    
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

