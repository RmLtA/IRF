//
//  main.cpp
//  projectIRF
//
//  Created by Cyrille Delabre on 21/10/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#include <iostream>
#include <string.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/core.hpp>
#include "fileOp.h"
#include "computeImages.h"

using namespace std;
using namespace cv;
static const int MAXIMAGETTE = 7 ;

string niceOutput(string output, int maxVal);


//debugging on console
static const bool VERBOSE = false;
static const bool RESULT = true;

//use folder test or default folder
static const bool TEST = false;;


int main(int argc, const char * argv[]) {
    cout << "Start..." <<endl;
    fileOp *  op = new fileOp(TEST);
    //recupere le nom des images sources
    vector<string> sourcesImages = op->getSourcesImages();
    //recupere le nom des imagettes sources	(templates)
    vector<string> templatesImages = op->getTemplImages();
	//pour chaque image source
    int totaltotal = 0;
    string pageCourante;
    for(int i=0 ; i < sourcesImages.size() ; i++){
        try{
            
		//on recupere l'image courante
		Mat img = imread(sourcesImages[i]);
        pageCourante = op->getFilename(sourcesImages[i]);
        if(VERBOSE)cout << "Traitement de la source : "<<sourcesImages[i]  << " ( " << pageCourante << " )" <<endl;
        if(RESULT && !VERBOSE)cout << pageCourante << "::";
        vector<templateArea> foundTemplate;
		computeImages * ll = new computeImages(img, VERBOSE);
        int imagesCount = 0;
        //On recupere en premier lieu les zones matchées par les templates afin de les ordonner
		 for(int j=0 ; j < templatesImages.size() ; j++)
         {
            if(imagesCount ==MAXIMAGETTE)break;
             
            string templateCourante = op->getFilename(templatesImages[j]);
            stringstream currentNameStream ; currentNameStream <<pageCourante << "-" << templateCourante;
             string s = currentNameStream.str();s.resize(17, ' ');
             if(VERBOSE)cout << "\t template ( " << s << " ) : ";
    
            Mat templ = imread(templatesImages[j]);
            if(ll->findTemplArea(templ, currentNameStream.str()))
            {
                //on enregistre les zones a traiter
                templateArea curr;
				curr.image = ll->getTemplArea().clone();;
				curr.position = ll->getPositionY();
                curr.name = templateCourante;
				

                foundTemplate.push_back(curr);
                imagesCount++;
                //on supprime la zone deja matchée...
                ll->removeZone();
                //on recommence avec le meme template afin de voir s'il existe le meme template à une autre position
                j--;
                
            }
            if(VERBOSE) cout << endl;

        }
        
        //if(VERBOSE) ll->showFinalImage(pageCourante);
        
        int totalImagettes =0;
        if(foundTemplate.size() >0){
            stringstream ss;
            //if(VERBOSE)cout << "ordonnancement des templates..." << endl; //sort templ avec position
            // on reordonne les zones matchées
            sort(foundTemplate.begin(), foundTemplate.end() , compareStruct);
            // pour chaque zone
            for(int ligne=0 ; ligne < foundTemplate.size() ; ligne++)
            {
                if(VERBOSE) cout << "recuperation de ligne potentielles... pour  " << foundTemplate[ligne].name<<endl;;

                //on récupère les lignes étant potentiellement nos imagettes à enregistrer
                vector<Vec4i> res =  ll->findLines(foundTemplate[ligne].image);
                
                if(VERBOSE) cout << "recuperation des imagettes... " <<endl;;

                //on récupère les imagettes
                vector<Mat> result =  ll->findImages(res, foundTemplate[ligne].image);
                
                if(VERBOSE) cout << "enregistrement... " <<endl;;

                //pour chaque image on enregistre le .jpg + txt avec les informations correspondantes
                for (int col =0 ; col < result.size(); col++) {
                    op->writeTxtFile(foundTemplate[ligne].name, pageCourante, ligne, col, result[col], false);
                   
                }
                if(RESULT)ss<< "\t "<< ligne <<" : " << result.size() << "/5" ;;
                totalImagettes += result.size();
            }
            if(VERBOSE) cout << endl;

            if(RESULT){
                stringstream sgs; sgs <<totalImagettes << "/35";
                cout << " found :  " << imagesCount << "/7 templates => imagettes : "
                << ss.str() << " \t total : " << niceOutput(sgs.str(), totalImagettes) << endl;
            }
        }else{

            if(VERBOSE)cout << "no templates founded " << endl; //sort templ avec position
        }
        totaltotal += totalImagettes;

        }catch(Exception e){
            cout << "error with image : " << pageCourante << endl;;
            cout << " " << e.what()<< endl;;
        }
     
	}
    
    cout << endl << "Total imagettes : " << totaltotal << " / " << 35*sourcesImages.size() << endl;;
       

    waitKey(0);
    return 0; 
}

string niceOutput(string output, int maxVal){
    stringstream ss;
    int  FG_RED      = 31;
    int FG_GREEN    = 32;
    ss << "\033[";
    if(maxVal == 35){
        ss << FG_GREEN << "m" << output;
    }else{
        ss << FG_RED << "m"  << output;
    }
    
    ss << " \033[0m";
    return ss.str();

}
