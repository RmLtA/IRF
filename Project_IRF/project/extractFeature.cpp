#include "extractFeature.h"

std::mutex ExtractFMtx;           // mutex for critical section
std::mutex OutMtx;           // mutex for critical section

//Si ajout de features, pas besoin de modification

//v_features_to_extract == vector des features
//v_result_images_toextract_features = vector des images
void extractFeature::compute_features(vector<int>& v_of_attributes_splited, vector<string>& v_result_images_toextract_features_splited,
                                      vector<int>& v_of_attributes_global, vector<string>& v_result_images_toextract_features_global){

    time_t xt = time(NULL);
    utils & u = utils::i();
	//delete duplicates in v_of_attributes 
	sort(v_of_attributes_splited.begin(), v_of_attributes_splited.end());
	v_of_attributes_splited.erase(unique(v_of_attributes_splited.begin(), v_of_attributes_splited.end()), v_of_attributes_splited.end());
    
    //delete duplicates in v_of_attributes
    sort(v_of_attributes_global.begin(), v_of_attributes_global.end());
    v_of_attributes_global.erase(unique(v_of_attributes_global.begin(), v_of_attributes_global.end()), v_of_attributes_global.end());
    
    //nombre d'attributs par feature splited.
    nbSplitedFeatures =0;
	for (int  i = 0; i < v_of_attributes_splited.size(); i++){
		v_attributes_asked_splited.push_back(v_of_attributes_splited[i]);
        nbSplitedFeatures += nb_features_by_attributes_splited[v_of_attributes_splited[i]];
	}
    //nombre d'attributs par feature global.
     nbGlobalFeatures =0;
    for (int  i = 0; i < v_of_attributes_global.size(); i++){
        v_attributes_asked_global.push_back(v_of_attributes_global[i]);
        nbGlobalFeatures += nb_features_by_attributes_global[v_of_attributes_global[i]];
    }
    
    int nbImgTotal = (int) v_result_images_toextract_features_global.size();
    
    
    //creation du tableau ˆ la bonne dimension
    v_all_numeric_v_attributes_values = vector<vector<double>>(nbImgTotal, vector<double>(nbSplitedFeatures*u.SPLIT_FACTOR+nbGlobalFeatures));
    //init vars
    leftToProcess=nbImgTotal;
    toProcess =nbImgTotal;
    currentToProcess= 0;
	
    if(THREADS){
    
        //pour toutes les images
        vector<thread> vThreads;
        int NB_THREADS = thread::hardware_concurrency();
        if(NB_THREADS ==0)NB_THREADS = 1;
        if(u.RESULT)cout <<"Using " << NB_THREADS << " threads  "<<endl;
        
        //init processes
        for(int i=0 ; i < NB_THREADS ; i++){
            vThreads.push_back(std::thread(&extractFeature::processTask, std::ref(*this), v_result_images_toextract_features_splited, v_result_images_toextract_features_global));
        }
        //give hand to process first time
        this_thread::yield();
        
        while(leftToProcess)
        {
            OutMtx.lock();
            //output current %
            double pr=((double)currentToProcess/(double)nbImgTotal)*100;
            cout<<"\t"<<setprecision(2)<<std::fixed<< pr<<"%   \r"<< flush ;
            OutMtx.unlock();
            //sleep to let others threads procced
            sleep(1.5); //pour windows a faire
            //pour windows : http://stackoverflow.com/questions/10918206/cross-platform-sleep-function-for-c
            //ou yield si marche pas

        }
        //wait for last threads & join
        for(auto& th : vThreads){
            th.join();
        }
        
    }else{
        extractFeature::processTask(*this, v_result_images_toextract_features_splited, v_result_images_toextract_features_global);
    }
      //on ajoute la classe
	addclassto_v_class(v_result_images_toextract_features_global);
    
    if(u.RESULT){
        xt = time(NULL) - xt;
        cout << " Temps d'exec.: " << setprecision(2)<< (double)xt/60 <<" min";;
        cout <<endl;
    }
}



void extractFeature::processTask(extractFeature& self, vector<string> v_result_images_toextract_features_splited, vector<string> v_result_images_toextract_features_global){
    unsigned long i;
    utils & u = utils::i();

    while(self.leftToProcess)
    {
        ExtractFMtx.lock();
        if(self.leftToProcess==0) break;
        self.leftToProcess--;
        i = self.currentToProcess++;
        ExtractFMtx.unlock();

        //on extrait les features globales en premier lieu
        string imgGlobalName = v_result_images_toextract_features_global[i];
        self.extract_all_features_global(imgGlobalName, (int)i);
        //puis les features splited
        if(u.SPLIT_FACTOR >1){
            for(int offset=0 ; offset< u.SPLIT_FACTOR ; offset++){
                //pour chaque petite image de l'image
                
                string imgName =v_result_images_toextract_features_splited[i*u.SPLIT_FACTOR+offset];
                //on extrait toutes les features
                self.extract_all_features_splited(imgName, (int)i , offset*self.nbSplitedFeatures+self.nbGlobalFeatures);
            }
        }
    }
}


void extractFeature::extract_all_features_splited(string imgName, int nextImage, int currentPos){
    Mat img = imread(imgName);
    if(img.rows ==0 || img.cols==0){
        return; 
    }
    //feature :: is global ? = false
    feature* f = new feature(img, false, getName(imgName));
    vector<double> feature_result;

    for(int j = 0 ; j < v_attributes_asked_splited.size() ; j++)
    {
        int feature_attribute = v_attributes_asked_splited[j];
        switch (feature_attribute){
                
            case BLACK_PIXEL:
                 feature_result.push_back(f->countBlackPixel());
                break;
            case AREA:
                feature_result.push_back(f->centerAreaRes().x);
                feature_result.push_back(f->centerAreaRes().y);
                feature_result.push_back(f->radiusAreaRes());
                
                feature_result.push_back(f->triangleArea1().x);
                feature_result.push_back(f->triangleArea1().y);
                
                feature_result.push_back(f->triangleArea2().x);
                feature_result.push_back(f->triangleArea2().y);
                
                feature_result.push_back(f->triangleArea3().x);
                feature_result.push_back(f->triangleArea3().y);

                feature_result.push_back(f->lengthContoursArea());
                feature_result.push_back(f->contoursArea());

                break;
            case HARRIS_CORNERS:
                feature_result.push_back(f->harrisCornerX());
                feature_result.push_back(f->harrisCornerY());
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
                feature_result.push_back(f->nbHoughLinesResult());
                break;
            case HOUGH_CIRCLES:
                feature_result.push_back(f->houghCirclesX());
                feature_result.push_back(f->houghCirclesY());
                feature_result.push_back(f->houghCirclesRadius());
                feature_result.push_back(f->nbHoughCirclesResult());
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
    feature* f = new feature(img, true, getName(imgName));
    vector<double> feature_result;
    
    for(int j = 0 ; j < v_attributes_asked_global.size() ; j++)
    {
        int feature_attribute = v_attributes_asked_global[j];
        switch (feature_attribute){
            case BLACK_PIXEL_GLOBAL:
                feature_result.push_back(f->countBlackPixel());
                break;
            case AREA_GLOBAL:
                feature_result.push_back(f->centerAreaRes().x);
                feature_result.push_back(f->centerAreaRes().y);
                feature_result.push_back(f->radiusAreaRes());

                feature_result.push_back(f->triangleArea1().x);
                feature_result.push_back(f->triangleArea1().y);

                feature_result.push_back(f->triangleArea2().x);
                feature_result.push_back(f->triangleArea2().y);

                feature_result.push_back(f->triangleArea3().x);
                feature_result.push_back(f->triangleArea3().y);
                
                feature_result.push_back(f->lengthContoursArea());
                feature_result.push_back(f->contoursArea());

                break;
            case HARRIS_CORNERS_GLOBAL:
                feature_result.push_back(f->harrisCornerX());
                feature_result.push_back(f->harrisCornerY());
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
                feature_result.push_back(f->nbHoughLinesResult());

                break;
            case HOUGH_CIRCLES_GLOBAL:
                feature_result.push_back(f->houghCirclesX());
                feature_result.push_back(f->houghCirclesY());
                feature_result.push_back(f->houghCirclesRadius());
                feature_result.push_back(f->nbHoughCirclesResult());
                
                break;
            case ROWS_OR_COLS_GLOBAL:
                feature_result.push_back(f->isLongerRowsOrCols());
                break;
            case PIXELS_GLOBAL:
                if(true){
                    vector<int> res = f->getPixels();
                    for(int i=0 ; i<res.size();i++)
                        feature_result.push_back(res[i]);
                }
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

string extractFeature::getGlobalFeatureName(int f){
    switch (f) {
        case BLACK_PIXEL_GLOBAL:
            return "Black Pixels Global " + to_string(f);
            break;
        case AREA_GLOBAL:
            return "Area Global " + to_string(f);
            break;
        case HARRIS_CORNERS_GLOBAL:
            return "Harris Corners Global " + to_string(f);
            break;
        case MASSCENTER_GLOBAL:
            return "Mass Center Global " + to_string(f);
            break;
        case HOUGH_LINES_GLOBAL:
            return "Hough Lines Global " + to_string(f);
            break;
        case HOUGH_CIRCLES_GLOBAL:
            return "Hough Circles Global " + to_string(f);
            break;
        case ROWS_OR_COLS_GLOBAL:
            return "Rows (1) or Cols Global " + to_string(f);
            break;
        default:
            return "No Name set";
    }
}
string extractFeature::getSplitedFeatureName(int f){
    switch (f) {
        case BLACK_PIXEL:
            return "Black Pixels " + to_string(f);
            break;
        case AREA:
            return "Area " + to_string(f);
            break;
        case HARRIS_CORNERS:
            return "Harris Corners " + to_string(f);
            break;
        case MASSCENTER:
            return "Mass Center " + to_string(f);
            break;
        case HOUGH_LINES:
            return "Hough Lines " + to_string(f);
            break;
        case HOUGH_CIRCLES:
            return "Hough Circles " + to_string(f);
            break;
       
        default:
            return "No Name set";
    }
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


string extractFeature::getName(string name)
{
    size_t point0 = name.find_last_of("/");
    string res = name.substr(point0 + 1, name.size());
    return res;
}

