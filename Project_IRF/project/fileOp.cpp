
#include "fileOp.h"

void fileOp::writeNormalized(string nam, Mat img){
	stringstream name_img,name;
	name << nam;
	name_img << dirResNormalizedName <<name.str()<< ".jpg";

	//if (verbose)cout << "  " << getFilename(name_img.str());
	//enregistrment de l'imagette
	imwrite(name_img.str(), img);

}


void fileOp::writeSplited(string nam, Mat img){
    stringstream name_img,name;
    name << nam;
    name_img << dirResSplitedName <<name.str()<< ".jpg";
    
    if (u.VERBOSE)cout << "  " << getFilename(name_img.str());
    //enregistrment de l'imagette
    imwrite(name_img.str(), img);
    
}

/**
* \fn void write_txtfile(string icone, int scripter_number, int page_number, int row, int columm)
* \brief Fonction de création et de remplissage du fichier ".txt" associé aux imagettes
*
* \param string templ Le nom du symbole de l'imagette
* \param int scripter_number Le numéro du scripter
* \param int page_number Le numéro de la page
* \param int row Le numéro de la ligne de l'imagette commence a 0
* \param int columm Le numéro de la colonne de l'imagette commence a 0
*/

void fileOp::writeTxtFile(string templ, string sourceName, int row, int columm, Mat image){

    stringstream tmp,name, name_img;
    //00102.png
    //scripter = 001
    string scripter =sourceName.substr(3,sourceName.size());
    //page = 02
    string page =sourceName.substr(0,3);
    
    tmp << templ << "_" << scripter << "_" << page << "_" << row << "_" << columm;
    name <<dirResImagesName << tmp.str() << ".txt";
    name_img <<dirResImagesName<<tmp.str() << ".jpg";
    
   // if(u.VERBOSE)cout<< "  " << getFilename(name_img.str());
    //enregistrment de l'imagette
    imwrite(name_img.str(), image);
    if(u.RESULT){
        //crŽation du fichier .txt
        ofstream pFile(name.str(), ios::out);
        
        //remplissage du fichier
        pFile << "Label " << templ << endl;
        pFile << "Form " << sourceName << endl;
        pFile << "Scripteur " <<scripter << endl;
        pFile << "Page " << page << endl;
        pFile << "Row " << row << endl;
        pFile << "Columm " << columm << endl;
        
        //fermeture du fichier
        pFile.close();

    }
  }

///Retourne les images sources
vector<string> fileOp::getSourcesImages(){
    vector<string> sourceDir = readDir(dirSourceName);
    return sourceDir;
}


///Retourne les imagettes
vector<string> fileOp::getResultImages(){
    vector<string> resultDir = readDir(dirResImagesName);
    return resultDir;
}

///Retourne les imagettes templates sources
vector<string> fileOp::getTemplImages(){
    vector<string> templDir = readDir(dirTemplName);
    return templDir;
}



///Retourne les imagettes normalisée
vector<string> fileOp::getNormalizedImages(){
    vector<string> templDir = readDir(dirResNormalizedName);
    return templDir;
}


///Retourne les imagettes splitte
vector<string> fileOp::getSplitedImages(){
    vector<string> templDir = readDir(dirResSplitedName);
    return templDir;
}


void fileOp::removeAllResImagesFiles(){
    vector<string> files = readDir(dirResImagesName);
    for(int i=0; i<files.size(); i++){
        if( remove(files[i].c_str()) != 0 )
            perror( "Error deleting file" );
    }
}

void fileOp::removeAllResNormalizedFiles(){
    vector<string> files = readDir(dirResNormalizedName);
    for(int i=0; i<files.size(); i++){
        if( remove(files[i].c_str()) != 0 )
            perror( "Error deleting file" );
    }
}

void fileOp::removeAllResSplittedFiles(){
    vector<string> files = readDir(dirResSplitedName);
    for(int i=0; i<files.size(); i++){
        if( remove(files[i].c_str()) != 0 )
            perror( "Error deleting file" );
    }
}

///Fonction utilitaire pour lire dans le dir
vector<string> fileOp::readDir(string dirName){
    vector<string> output ;
    DIR *dir;
    dir = opendir(dirName.c_str());
    struct dirent *ent;
    if (dir != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            string imgName =ent->d_name;
            if(getExtName(imgName) == ".jpg" || getExtName(imgName) == ".png"){
                output.push_back (dirName + imgName) ;
            }
        }
        closedir (dir);
    } else {
        cout<< dirName << " not present... "<<endl;
        //closedir (dir);
        exit(0);
    }
    return output;
}

string fileOp::getFilename(string name){
    size_t  dir = name.find_last_of("/");
    string res = name.substr(dir, name.size());
    size_t  point = res.find_last_of(".");
   return res.substr(1, point-1);
}
string fileOp::getExtName(string name){
    size_t found = name.find_last_of(".");
    //cout << found ;
    if(found != -1)
    return  name.substr(found, name.length());
    else
    return "null";
}

void fileOp::writeARFFFile(extractFeature& extrfeat){
    cout <<  "Saving Arff FILE"<<endl;;
    string name;
    cout << "Name of Arff file : " << endl;
    cin >> name;
    if(name != "" || name != "\0")
        name +=".arff";
    else
        name = "IRF.arff";

    ofstream pFile(dirResArffdName + name, ios::out);


    /*Begin Head File*/
    pFile << "%1. Title : " << endl;
    pFile << "%" << endl;
    pFile << "%2. Sources : " << endl;
    pFile << "%" << endl;
    pFile << "%" << endl;
    pFile << "%" << endl;
    pFile << "@RELATION Imagette" << endl;
    pFile << endl;

    
    pFile << "% Global features from the images :" << endl;
    for (int i = 0; i < extrfeat.v_attributes_asked_global.size(); i++){
        int attribut = extrfeat.v_attributes_asked_global[i];
        switch (attribut){
            case extractFeature::BLACK_PIXEL_GLOBAL:
                pFile<< "@ATTRIBUTE "<< " Black_Pixel_Global" << " NUMERIC" << endl;;
                break;
            case extractFeature::AREA_GLOBAL :
                pFile<< "@ATTRIBUTE " <<" Airs_Global"<< " NUMERIC" << endl;;
                break;
            case extractFeature::HARRIS_CORNERS_GLOBAL:
                pFile<< "@ATTRIBUTE " <<" Harris_Corners_X_Global"<< " NUMERIC" << endl;;
                pFile<< "@ATTRIBUTE " <<" Harris_Corners_Y_Global"<< " NUMERIC" << endl;;

                break;
            case extractFeature::LENGTH_AREA_GLOBAL:
                pFile<< "@ATTRIBUTE " << " Length_Area_Global"<< " NUMERIC" << endl;;
                break;
            case extractFeature::MASSCENTER_GLOBAL:
                pFile<< "@ATTRIBUTE " << " Mass_Center_X_Global" << " NUMERIC" << endl;
                pFile<< "@ATTRIBUTE " << " Mass_Center_Y_Global" << " NUMERIC" << endl;
                break;
            case extractFeature::HOUGH_LINES_GLOBAL:
                pFile<< "@ATTRIBUTE " << " Hough_Lines_Vertical_Global"<< " NUMERIC" << endl;
                pFile<< "@ATTRIBUTE " << " Hough_Lines_Horizontal_Global"<< " NUMERIC" << endl;
                pFile<< "@ATTRIBUTE " << " Hough_Lines_DiagonalPos_Global"<< " NUMERIC" << endl;
                pFile<< "@ATTRIBUTE " << " Hough_Lines_DiagonalNeg_Global"<< " NUMERIC" << endl;
                break;
            case extractFeature::ROWS_OR_COLS_GLOBAL:
                pFile<< "@ATTRIBUTE " << " Rows_Or_Cols_Global"<< " NUMERIC" << endl;
                break;
                
                
        }
    }
    pFile << "% Features from the images splited in " <<extrfeat.SPLITED << " : " << endl;
    for(int j =0 ; j < extrfeat.SPLITED ; j++){
        for (int i = 0; i < extrfeat.v_attributes_asked_splited.size(); i++){
            int attribut = extrfeat.v_attributes_asked_splited[i];
                switch (attribut){
                    case extractFeature::BLACK_PIXEL:
                        pFile<< "@ATTRIBUTE "<< " Black_Pixel_" <<j << " NUMERIC" << endl;;
                        break;
                    case extractFeature::AREA :
                        pFile<< "@ATTRIBUTE " <<" Airs_" <<j << " NUMERIC" << endl;;
                        break;
                    case extractFeature::HARRIS_CORNERS:
                        pFile<< "@ATTRIBUTE " <<" Harris_Corners_X_" <<j << " NUMERIC" << endl;;
                        pFile<< "@ATTRIBUTE " <<" Harris_Corners_Y_" <<j << " NUMERIC" << endl;;
                        break;
                    case extractFeature::LENGTH_AREA:
                        pFile<< "@ATTRIBUTE " << " Length_Area_" <<j << " NUMERIC" << endl;;
                        break;
                    case extractFeature::MASSCENTER:
                        pFile<< "@ATTRIBUTE " << " Mass_Center_X_" <<j << " NUMERIC" << endl;
                        pFile<< "@ATTRIBUTE " << " Mass_Center_Y_" <<j << " NUMERIC" << endl;
                        break;
                    case extractFeature::HOUGH_LINES:
                        pFile<< "@ATTRIBUTE " << " Hough_Lines_Vertical_" <<j << " NUMERIC" << endl;
                        pFile<< "@ATTRIBUTE " << " Hough_Lines_Horizontal_" <<j << " NUMERIC" << endl;
                        pFile<< "@ATTRIBUTE " << " Hough_Lines_DiagonalPos_" <<j << " NUMERIC" << endl;
                        pFile<< "@ATTRIBUTE " << " Hough_Lines_DiagonalNeg_" <<j << " NUMERIC" << endl;
                        break;

                   
                }
            }
    }
    pFile << "@ATTRIBUTE" << " Class " << " {accident, bomb, car, casualty, electricity, fire-brigade, fire, flood, gas, injury, paramedics, person, police, road-block}" << endl;

    pFile << endl;
    /*End Head File*/


    pFile << "@DATA" << endl;
    
    
    for(int i = 0 ; i < extrfeat.v_all_numeric_v_attributes_values.size() ; i++){
        for(int j = 0 ; j < extrfeat.v_all_numeric_v_attributes_values[i].size(); j++){
            pFile << extrfeat.v_all_numeric_v_attributes_values[i][j] << ",";
        }
        pFile << extrfeat.v_class[i] << endl;
    }

    pFile.close();
    

}

