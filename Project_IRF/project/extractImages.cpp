//
//  extractImages.cpp
//  ProjectIRF
//
//  Created by Cyrille Delabre on 08/12/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#include "extractImages.hpp"
#include <time.h>
#include <iomanip>
#include <mutex>          // std::mutex
std::mutex mtx;           // mutex for critical section


void extractImages::process(){
    fileOp *  op = new fileOp();
    //removing files befores procesing new files
    op->removeAllResImagesFiles();
    clock_t prog_b, prog_e;
    double cpuTime;
    prog_b = clock();
    time_t xt = time(NULL);

    //recupere le nom des images sources (feuilles de test)
    vector<string> sourcesImages = op->getSourcesImages();
    
    //recupere le nom des templates
    vector<string> templatesImages = op->getTemplImages();
   
#ifdef __APPLE__
    vector<thread> vThreads;
    int NB_THREADS = thread::hardware_concurrency();
    if(NB_THREADS ==0)
        NB_THREADS = 4;
    
    cout <<"Using up to threads :  "<< NB_THREADS <<endl;
    
    //init vars
    leftToProcess=sourcesImages.size();
    toProcess =sourcesImages.size();
    unsigned int finishedTask = NB_THREADS;
    long int temp = leftToProcess;

    //init 4 processes
    for(int i=0 ; i < NB_THREADS ; i++){
        vThreads.push_back(std::thread(&extractImages::processTask, std::ref(*this), sourcesImages[i], templatesImages));
    }

    this_thread::yield();
    
    while(leftToProcess)
    {
        if(temp != leftToProcess)
        {
            mtx.lock();
            int proceded = (int)temp - (int) leftToProcess;
            int finished = finishedTask;
            for(int i = 0 ; i <proceded && finished <toProcess; i++){
                cout << "." << flush;
                vThreads.push_back(std::thread(&extractImages::processTask, std::ref(*this), sourcesImages[i+finishedTask], templatesImages));
                finished++;
            }
            finishedTask= finished;
            temp = leftToProcess;
            mtx.unlock();
        }else{
            //this_thread::yield();
            //sleep(1);
        }
    }
    
    for(auto& th : vThreads){
        th.join();
    }
    

#else
    
      for(int i=0 ; i < sourcesImages.size() ; i++){
          processTask(*this,sourcesImages[i],templatesImages);
      }
    
#endif
    
    xt = time(NULL) - xt;
    prog_e = clock();

    cpuTime = (double) ((prog_e - prog_b) / (double)CLOCKS_PER_SEC);
    cout << endl << setprecision(3) << "Total imagettes : " << nTotalImg << " / " << 35*(sourcesImages.size()-nErroImg) << "\t" << "| Temps d'exec.: " << (double)xt/60. <<" min | Cpu time : "<< (double)cpuTime/60. << endl;;

    delete op;
}



void extractImages::processTask(extractImages& self,string sourceImage,const vector<string>& templatesImages)
{
    utils & u = utils::i();
    Mat img = imread(sourceImage);
    fileOp *  op = new fileOp();
    computeImages * ll = new computeImages(img, u.VERBOSE);
    string pageCourante;
    int imagesCount = 0;
    stringstream output;

    
    try{
        pageCourante = op->getFilename(sourceImage);
        if(u.VERBOSE)output << "Traitement de la source : "<<sourceImage  << " ( " << pageCourante << " )" <<endl;
        if(u.RESULT && !u.VERBOSE)output << pageCourante << " ::";
        
        
        vector<templateArea> foundTemplate;
        
        //On recupere en premier lieu les zones matchées par les templates afin de les ordonner
        for(int j=0 ; j < templatesImages.size() ; j++)
        {
            if(imagesCount ==MAXIMAGETTE)break;
            
            string templateCourante = op->getFilename(templatesImages[j]);
            stringstream currentNameStream ; currentNameStream <<pageCourante << "-" << templateCourante << "-" << imagesCount;
            string s = currentNameStream.str();s.resize(17, ' ');
            if(u.VERBOSE)output << "\t template ( " << s << " ) : ";
            
            Mat templ = imread(templatesImages[j]);
            if(ll->findTemplArea(templ, currentNameStream.str(), false))
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
            if(u.VERBOSE) cout << endl ;
            
        }
        
      
        
        int totalImagettes =0;
        if(foundTemplate.size() >0){
            if(u.VERBOSE) output << "saving... " <<endl;;
            stringstream ss;
            // on reordonne les zones matchées
            sort(foundTemplate.begin(), foundTemplate.end() , compareStruct);
            // pour chaque zone
            for(int ligne=0 ; ligne < foundTemplate.size() ; ligne++)
            {
                //if(VERBOSE) cout << "recuperation de ligne potentielles... pour  " << foundTemplate[ligne].name<<endl;;
                
                //on récupère les lignes étant potentiellement nos imagettes à enregistrer
                vector<Vec4i> res =  ll->findLines(foundTemplate[ligne].image);
                
                //if(VERBOSE) cout << "recuperation des imagettes... " <<endl;;
                
                //on récupère les imagettes
                vector<Mat> result =  ll->findImages(res, foundTemplate[ligne].image);
                
                //pour chaque image on enregistre le .jpg + txt avec les informations correspondantes
                for (int col =0 ; col < result.size(); col++) {
                    op->writeTxtFile(foundTemplate[ligne].name, pageCourante, ligne, col, result[col]);
                    
                }
                if(u.RESULT){
                    stringstream sgs; sgs <<result.size() << "/5";
                    ss<< "\t "<< ligne+1 <<" : " << niceOutput(sgs.str(), result.size() == 5) ;;
                }
                totalImagettes += result.size();
            }
            if(u.VERBOSE) output << endl;
            
            if(u.RESULT){
                stringstream sgs; sgs <<totalImagettes << "/35";
                output << " found :  " << imagesCount << "/7 templates => imagettes : "
                << ss.str() << " \t total : " << niceOutput(sgs.str(), totalImagettes == 35)  << endl;
            }
        }else{
            
            if(u.VERBOSE || u.RESULT)cout << niceOutput(" Error, image rejected", false) << endl; //sort templ avec position
            self.nErroImg++;
            
        }
        self.nTotalImg += totalImagettes;
        
    }catch(Exception e){
        output << "error with image : " << pageCourante << endl;;
        output << " " << e.what()<< endl;;
    }
    delete ll;
    delete op;
#ifdef __APPLE__
    mtx.lock();
    if(u.RESULT)cout << "End : " << sourceImage <<endl;
    if(u.VERBOSE || u.RESULT )cout << output.str()<<endl;
    cout<< flush;
    self.leftToProcess--;
    mtx.unlock();
#endif

}