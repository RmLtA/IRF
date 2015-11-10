//
//  main.cpp
//  projectIRF
//

#include <iostream>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <ctime>
#include "fileOp.h"
#include "computeImages.h"

using namespace std;
using namespace cv;
static const int MAXIMAGETTE = 7 ;
string niceOutput(string output, bool ok);


//debugging on console
static bool VERBOSE = false;
static bool RESULT = true;

/*unused*/
//use folder test or default folder
static  bool TEST = false;

/*
 USAGE ::
 ./projetIRF -v [mode verbose]
 ./projetIRF -r [mode resultats]
 ./projetIRF -a [mode resultats + verbose]
 */
 /* si en mode test (sources des images sources/test)
    ./projetIRF -a [mode resultats + verbose]
 */

int main(int argc, const char * argv[]) {
    if(argc > 1){
        string a = argv[1];
        if(a== "verbose" || a=="-v") {
            VERBOSE =true;
        }else if(a == "result" || a=="-r"){
            RESULT =true;
        }else if(a=="-a"){
            RESULT =true;
            VERBOSE = true;
        }
        if(argc == 3){
            string a = argv[2];
            if(a == "-test" || a =="-t") TEST =true;
        }
    }
   
        
    cout << "Start..." <<endl;
    fileOp *  op = new fileOp(TEST);
    clock_t start, end, prog_b, prog_e;
    double cpuTime;
    
    prog_b = clock();

    //recupere le nom des images sources (feuilles de test)
    vector<string> sourcesImages = op->getSourcesImages();

    //recupere le nom des templates
    vector<string> templatesImages = op->getTemplImages();

	//boucle de traitement des images sources
    int totaltotal = 0;
    string pageCourante;
    for(int i=0 ; i < sourcesImages.size() ; i++){
    	Mat img = imread(sourcesImages[i]);
    	computeImages * ll = new computeImages(img, VERBOSE);
        try{
			
				pageCourante = op->getFilename(sourcesImages[i]);
                start = clock();
				if(VERBOSE)cout << "Traitement de la source : "<<sourcesImages[i]  << " ( " << pageCourante << " )" <<endl;
                if(RESULT && !VERBOSE)cout << pageCourante << " ::";


				vector<templateArea> foundTemplate;
				

				int imagesCount = 0;

				//On recupere en premier lieu les zones matchées par les templates afin de les ordonner
				 for(int j=0 ; j < templatesImages.size() ; j++)
				 {
					if(imagesCount ==MAXIMAGETTE)break;
             
					string templateCourante = op->getFilename(templatesImages[j]);
					stringstream currentNameStream ; currentNameStream <<pageCourante << "-" << templateCourante << "-" << imagesCount;
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
                        //if(VERBOSE)ll->showFinalImage(currentNameStream.str());
						//on recommence avec le meme template afin de voir s'il existe le meme template à une autre position
						j--;
                
					}
                     if(VERBOSE) cout << endl ;

				}
        
            if(VERBOSE){
                ll->showFinalImage(pageCourante);
                clock_t middle = clock();
                cpuTime = (double) ((middle - start) / (double)CLOCKS_PER_SEC);
                cout << endl << "time for template matching" << cpuTime ;
            }
			int totalImagettes =0;
			if(foundTemplate.size() >0){
				stringstream ss;
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
                    if(RESULT){
                        stringstream sgs; sgs <<result.size() << "/5";
                        ss<< "\t "<< ligne+1 <<" : " << niceOutput(sgs.str(), result.size() == 5) ;;
                    }
                    totalImagettes += result.size();
				}
				if(VERBOSE) cout << endl;

				if(RESULT){
                    end = clock();
                    cpuTime = (double) ((end - start) / (double)CLOCKS_PER_SEC);
					stringstream sgs; sgs <<totalImagettes << "/35";
					cout << " found :  " << imagesCount << "/7 templates => imagettes : "
					<< ss.str() << " \t total : " << niceOutput(sgs.str(), totalImagettes == 35) << "\t time : " << cpuTime << " s"  << endl;
				}
			}else{

				if(VERBOSE)cout << "no templates founded " << endl; //sort templ avec position
			}
			totaltotal += totalImagettes;
			
			}catch(Exception e){
				cout << "error with image : " << pageCourante << endl;;
				cout << " " << e.what()<< endl;;
			}
			delete ll;
	
     
	}
    
    
    prog_e = clock();
    cpuTime = (double) ((prog_e - prog_b) / (double)CLOCKS_PER_SEC);

    cout << endl << "Total imagettes : " << totaltotal << " / " << 35*sourcesImages.size() << "\t" << "| Temps d'exec.: " << cpuTime/60  <<" min" << endl;;
       
    delete op;
    waitKey(0);
    return 0; 
}


 string niceOutput(string output, bool ok){
    stringstream ss;
    int  FG_RED      = 31;
    int FG_GREEN    = 32;
    ss << "\033[";
    if(ok){
        ss << FG_GREEN << "m" << output;
    }else{
        ss << FG_RED << "m"  << output;
    }
    
    ss << " \033[0m";
    return ss.str();
}


