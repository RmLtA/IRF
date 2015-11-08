
#include "fileOp.h"

/**
* \fn void write_txtfile(string icone, int scripter_number, int page_number, int row, int columm)
* \brief Fonction de cr�ation et de remplissage du fichier ".txt" associ� aux imagettes
*
* \param string templ Le nom du symbole de l'imagette
* \param int scripter_number Le num�ro du scripter
* \param int page_number Le num�ro de la page
* \param int row Le num�ro de la ligne de l'imagette commence a 0
* \param int columm Le num�ro de la colonne de l'imagette commence a 0
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

	//cr�ation du fichier .txt
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

