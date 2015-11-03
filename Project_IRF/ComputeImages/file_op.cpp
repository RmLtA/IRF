#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

#include <windows.h>


/**
* \fn void write_txtfile(string icone, int scripter_number, int page_number, int row, int columm)
* \brief Fonction de cr�ation et de remplissage du fichier ".txt" associ� aux imagettes
*
* \param string icone Le nom du symbole de l'imagette
* \param int scripter_number Le num�ro du scripter
* \param int page_number Le num�ro de la page
* \param int row Le num�ro de la ligne de l'imagette
* \param int columm Le num�ro de la colonne de l'imagette
* \return rien
*/
void write_txtfile(string icone, int scripter_number, int page_number, int row, int columm){

	stringstream name;
	name << icone << "_" << scripter_number << "_" << page_number << "_" << row << "_" << columm << ".txt";
	string file_name;
	file_name = name.str();

	//cr�ation du fichier .txt
	ofstream pFile(file_name, ios::out);

	//remplissage du fichier
	pFile << "Label " << icone << endl;
	pFile << "Scripteur " << scripter_number << endl;
	pFile << "Page " << page_number << endl;
	pFile << "Row " << row << endl;
	pFile << "Columm " << columm << endl;

	//fermeture du fichier
	pFile.close();
}


/**
* \fn vector<string> find_image_name(void)
* \brief Fonction r�cup�rant le nom des images du r�pertoire courant
*
* \param rien
* \return vector<string> Un vecteur contenant le nom de tous les fichiers ".png" trouv�s dans le dossier courant. (l'extension ".png" est enlev�e : 0000.png -> 0000)
*/
vector<string> find_image_name(void){

	vector<string> vector_image_name;
	WIN32_FIND_DATA File;
	HANDLE hSearch;

	//recherche de tous les fichiers .png
	hSearch = FindFirstFile(L"*.png", &File);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		//pour chaque fichier trouv� on extrait le nom
		do{
			//traitement pour ne r�cup�rer que le nom
			wstring ws(File.cFileName);
			string str(ws.begin(), ws.end());
			size_t found = str.find(".");
			str.resize(found);
			//enregistrement du nom dans le vecteur
			vector_image_name.push_back(str);
		}while (FindNextFile(hSearch, &File));

		FindClose(hSearch);
	}

	return vector_image_name;
}