
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

void fileOp::writeARFFFile(){
    ofstream pFile("IRF.arff", ios::out);

    pFile << "%1. Title : " << endl;
    pFile << "%" << endl;
    pFile << "%2. Sources : " << endl;
    pFile << "%" << endl;
    pFile << "%" << endl;
    pFile << "%" << endl;
    pFile << "@RELATION Imagette" << endl;
    pFile << endl;

    for (int i = 0; i < v_attributes.size()-1; i++){
        pFile << "@ATTRIBUTE " << v_attributes[i]<< " NUMERIC" <<endl;
    }
    pFile << "@ATTRIBUTE " << v_attributes[v_attributes.size()-1] << " {accident, bomb, car, casualty, electricity, fire-brigade, flood, gas, injury, paramedics, person police, road-block}" << endl;

    pFile << endl;
    pFile << "@DATA" << endl;
    //normalement tous les vector de values of attributes on la même taille

        for (int i = 0; i < v_nb_black_pixels.size(); i++){
            pFile <<v_nb_black_pixels[i];
            pFile << ",";
            pFile << v_nb_harris_corners[i];
            pFile << ",";
            pFile << v_nb_area[i];
            pFile << ",";
            pFile << v_nb_lenght[i];
            pFile << ",";
            pFile << v_class_icon[i];
            pFile << endl;

        }
    
    pFile.close();
    

}

void fileOp::addclasstov_class_icon(string current){
    bool flag = false;
    if (current.find("accident") != string::npos) v_class_icon.push_back("accident");
    if (current.find("bomb") != string::npos) v_class_icon.push_back("bomb");
    if (current.find("car") != string::npos) v_class_icon.push_back("car");
    if (current.find("casualty") != string::npos) v_class_icon.push_back("casualty");
    if (current.find("electricity") != string::npos) v_class_icon.push_back("electricity");
    if (current.find("brigade") != string::npos){
        flag = true;
        v_class_icon.push_back("fire-brigade");
    }
    if (current.find("fire") != string::npos && flag != true){
        v_class_icon.push_back("fire");
    }
    if (current.find("flood") != string::npos) v_class_icon.push_back("flood");
    if (current.find("gas") != string::npos) v_class_icon.push_back("gas");
    if (current.find("injury") != string::npos) v_class_icon.push_back("injury");
    if (current.find("paramedics") != string::npos) v_class_icon.push_back("paramedics");
    if (current.find("person") != string::npos) v_class_icon.push_back("person");
    if (current.find("police") != string::npos) v_class_icon.push_back("police");
    if (current.find("road-block") != string::npos) v_class_icon.push_back("road-block");
}

