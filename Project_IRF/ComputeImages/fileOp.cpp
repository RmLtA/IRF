
#include "fileOp.h"

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

void fileOp::writeTxtFile(string templ, string sourceName, int row, int columm, Mat image, bool verbose){

    stringstream tmp,name, name_img;
    //00102.png
    //scripter = 001
    string scripter =sourceName.substr(3,sourceName.size());
    //page = 02
    string page =sourceName.substr(0,3);
    
    tmp << templ << "_" << scripter << "_" << page << "_" << row << "_" << columm;
    name <<dirResImagesName << tmp.str() << ".txt";
    name_img <<dirResImagesName<<tmp.str() << ".jpg";
    
    if(verbose)cout<< "  " << getFilename(name_img.str());
    //enregistrment de l'imagette
    imwrite(name_img.str(), image);

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
        closedir (dir);
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
    ofstream pFile("IRF.arff", ios::out);

    /*Begin Head File*/
    pFile << "%1. Title : " << endl;
    pFile << "%" << endl;
    pFile << "%2. Sources : " << endl;
    pFile << "%" << endl;
    pFile << "%" << endl;
    pFile << "%" << endl;
    pFile << "@RELATION Imagette" << endl;
    pFile << endl;

    
    for (int i = 0; i < (extrfeat).getSizeOfv_attribute_asked(); i++){
        int attribut = (extrfeat).getVectorAttributesAsked()[i];
            switch (attribut){
                case BLACK_PIXEL:
                        pFile << "@ATTRIBUTE " << " Black_Pixel " << " NUMERIC" << endl;
                    break;
                case WHITE_PIXEL:
                        pFile << "@ATTRIBUTE " << " White_Pixel " << " NUMERIC" << endl;
                    break;
                case AREA :
                    pFile << "@ATTRIBUTE " << " Airs " << " NUMERIC" << endl;
                    break;
                case CONTOURS_SIZE:
                    pFile << "@ATTRIBUTE " << " Contours_size " << " NUMERIC" << endl;
                    break;
                case HARRIS_CORNERS:
                    pFile << "@ATTRIBUTE " << " Harris_Corners " << " NUMERIC" << endl;
                    break;
                case LENGTHAREA:
                    pFile << "@ATTRIBUTE " << " Length_Area " << " NUMERIC" << endl;
                    break;
                /*case MASSCENTER:
                    pFile << "@ATTRIBUTE " << " Mass_Center " << " NUMERIC" << endl;
                    break;*/
            }
            
        }
    pFile << "@ATTRIBUTE" << " Class " << " {accident, bomb, car, casualty, electricity, fire-brigade, fire, flood, gas, injury, paramedics, person police, road-block}" << endl;

    pFile << endl;
    /*End Head File*/


    pFile << "@DATA" << endl;

    //First compute all numeric attribute
    int k = 0; //normalement tous les vecteurs ont la même taille
    int index_class = 0;
    while (k < (extrfeat).v_all_numeric_v_attributes_values[0].size()){
        vector<int> v;
        vector<string> vs;
        for (int i = 0; i < (extrfeat).v_all_numeric_v_attributes_values.size(); i++){
            v.push_back((extrfeat).v_all_numeric_v_attributes_values[i][k]);
        }
        vs.push_back(extrfeat.v_class[k]);

        //Ecriture dans le fichier

            for (int j = 0; j < v.size(); j++){
                if (j == v.size() - 1){
                    pFile << v[j] << ","<< vs[index_class]<<endl;

                }
                else{
                    pFile << v[j] << ",";
                }
            }
            v.clear();
            k++;



    }
    pFile.close();
    

}

