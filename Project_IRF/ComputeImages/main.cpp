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
#include "feature.h"


/* Utility functions*/
void get_args(int argc, const char * argv[]);
string niceOutput(string output, bool ok);
Mat boundingBox2(Mat src);
/* Process functions*/

void process_part_one();
void process_part_two();
void process_part_three();


using namespace std;
using namespace cv;
static const int MAXIMAGETTE = 7 ;


//debugging on console
static bool VERBOSE = false;
static bool RESULT = false;

/*unused*/
//use folder test or default folder
static  bool TEST = false;



static bool PART_1 = false;
static bool PART_2 = false;
static bool PART_3 = true;
/*
 // USAGE ::
 ./projetIRF -v [mode verbose]
 ./projetIRF -r [mode resultats]
 ./projetIRF -a [mode resultats + verbose]
 */
/* si en mode test (sources des images sources/test)
 ./projetIRF -a -test [mode resultats + verbose]
 */




int main(int argc, const char * argv[]) {

    get_args(argc, argv);
    
    
    if(PART_1){
        if(VERBOSE)cout << "Part one..." <<endl;
        process_part_one();
    }
    if(PART_2){
        if(VERBOSE)cout << "Part two..." <<endl;
        process_part_two();
    }
    if (PART_3){
		if (VERBOSE)cout << "Part three..." << endl;
		process_part_three();
	}
	cout << "Finished..." << endl;


    
    waitKey(0);
    return 0; 
}

/// ./projetIRF -none -part2 -test

void get_args(int argc, const char * argv[]){
   // cout << argc << endl;;
        for(int i=1 ; i<argc ; i++){
            string a = argv[i];
           // cout <<a << endl;
            if      (a== "-verbose"  || a=="-v") VERBOSE =true;
            else if (a=="-result"    || a=="-r") RESULT =true;
            else if (a=="-none"      || a=="-n") RESULT = VERBOSE =false;
            else if(a == "-1" || a =="-part1") PART_1 =true;
            else if(a == "-2" || a =="-part2") PART_2 =true;
            else if (a == "-3" || a == "-part3") PART_3 = true;
            else if(a == "-a" || a =="-all") PART_2 = PART_1 = PART_3= true;
            else if(a == "-test" || a =="-t") TEST =true;
            
        }
    cout << "VERBOSE : " << VERBOSE;
    cout << " RESULT : " << RESULT;
    cout << " PART1 : " << PART_1;
    cout << " PART2 : " << PART_2;
    cout << " TEST : " << TEST;
    cout <<endl;

}



void process_part_one()
{
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
    int nErroImg = 0;
    for(int i=0 ; i < sourcesImages.size() ; i++){
        Mat img = imread(sourcesImages[i]);
        computeImages * ll = new computeImages(img, VERBOSE);
        
        int imagesCount = 0;
        
        
        try{
            pageCourante = op->getFilename(sourcesImages[i]);
            start = clock();
            if(VERBOSE)cout << "Traitement de la source : "<<sourcesImages[i]  << " ( " << pageCourante << " )" <<endl;
            if(RESULT && !VERBOSE)cout << pageCourante << " ::";
            
            
            vector<templateArea> foundTemplate;
            
            //On recupere en premier lieu les zones matchées par les templates afin de les ordonner
            for(int j=0 ; j < templatesImages.size() ; j++)
            {
                if(imagesCount ==MAXIMAGETTE)break;
                
                string templateCourante = op->getFilename(templatesImages[j]);
                stringstream currentNameStream ; currentNameStream <<pageCourante << "-" << templateCourante << "-" << imagesCount;
                string s = currentNameStream.str();s.resize(17, ' ');
                if(VERBOSE)cout << "\t template ( " << s << " ) : ";
                
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
                if(VERBOSE) cout << endl ;
                
            }
            
            if(VERBOSE){
                ll->showFinalImage(pageCourante);
                clock_t middle = clock();
                cpuTime = (double) ((middle - start) / (double)CLOCKS_PER_SEC);
                cout << endl << "Time for template matching : " << cpuTime << "s"<<endl;;
            }
            
            int totalImagettes =0;
            if(foundTemplate.size() >0){
                if(VERBOSE) cout << "saving... " <<endl;;
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
                
                if(VERBOSE || RESULT)cout << niceOutput(" Error, image rejected", false) << endl; //sort templ avec position
                nErroImg++;
                
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
    cout << endl << "Total imagettes : " << totaltotal << " / " << 35*(sourcesImages.size()-nErroImg) << "\t" << "| Temps d'exec.: " << cpuTime/60  <<" min" << endl;;
}

void process_part_two()
{    
    fileOp *  op = new fileOp(TEST);
    try{
        
        //Name of Images used to extract featires
        vector<string> v_result_images_toextract_features = op->getResultImages();

        //Print features available, ! attention l'ordre ici est important, le même que pour l'enum Features_available si modification
        //car c'est ce que l'utilisateur va prendre en compte car instructions imprimées à l'écran
        vector<string> v_features_available = {"Black_Pixel","White_Pixel","Airs","Contours_Size","Harris_Corners", "Length Area"};

        //Features to extract
        vector<int> v_features_to_extract;
        cout << "All features available :" << endl;
        for (int i = 0; i<v_features_available.size();i++){
            cout << "   -"<<v_features_available[i] << " : Tape "<< i+1<< endl;
        }
        cout << "To finish : Tape 0" << endl;

        
        int features_toextract;
        cin >> features_toextract;
        while (features_toextract != 0){
            v_features_to_extract.push_back(features_toextract);
            cin >> features_toextract;
        }
        //Pour mettre l'attribut class à la fin
        v_features_to_extract.push_back(INT_MAX);


        extractFeature extract_feature;
        extract_feature.compute_features(v_features_to_extract,v_result_images_toextract_features);

        cout << "computing ... " << endl;
        op->writeARFFFile(extract_feature);
        
    }catch(Exception e){
        cout << "Error in part two..." << endl;
        cout << e.what() << endl;
    }
    
    
    delete op;
    
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

void process_part_three()
{
	
    fileOp *  op = new fileOp(TEST);

        vector<string> result = op->getSourcesImages();
        cout << "Taille result : "  << result.size() << endl;
		string current;
        //read images
        for (int i = 0; i < result.size(); i++){
			Mat img = imread(result[i]);
			Mat res = boundingBox2(img);
			current = op->getFilename(result[i]);
			cout << "Process... : " << current<< endl;
			cout << i << endl;
			op->writeFile(current,res, VERBOSE);
		
		  }       
  
	
    delete op;
    
}
/**
 * \fn Mat boundingBox
 * \brief extrait l'image
 *
 * \param src : image source
 * \return Mat
 */
 Mat boundingBox2(Mat src)
 {
	 Mat src_gray;
	 RNG rng(12345);
	 int thresh = 245;
	 Mat threshold_output;
	 vector<vector<Point> > contours;
	 vector<Vec4i> hierarchy;

	 /// Convert image to gray and blur it
	 cvtColor(src, src_gray, CV_BGR2GRAY);
	 blur(src_gray, src_gray, Size(3, 3));
	 /// Detect edges using Threshold
	 threshold(src_gray, threshold_output, thresh, 255, THRESH_BINARY);
	 /// Find contours
	 findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	 /// Approximate contours to polygons + get bounding rects and circles
	 vector<vector<Point> > contours_poly(contours.size());
	 vector<Rect> boundRect(contours.size());
	 vector<Point2f>center(contours.size());
	 vector<float>radius(contours.size());

	 for (int i = 0; i < contours.size(); i++)
	 {
		 approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		 boundRect[i] = boundingRect(Mat(contours_poly[i]));
		 //minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
	 }

	 double max = 0; int k = 0;
	 /// Draw polygonal contour + bonding rects + circles
	 Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	 Mat dst = Mat::zeros(threshold_output.size(), CV_8UC3);
	 Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	 for (int i = 0; i< contours.size(); i++)
	 {

		 drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		 if (boundRect[i].area() > max && boundRect[i].area()<src.size().area() - 1000){
			 max = boundRect[i].area();
			 k = i;
		 }
		 rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);

		 //circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
	 }
	 rectangle(dst, boundRect[k].tl(), boundRect[k].br(), color, 2, 8, 0);
	 cout << boundRect[k].area() << endl;
	 // decouper les images

	 Mat final(src, boundRect[k]);

	 imshow("ROI", final);
	 /// Show in a window
	 namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	 imshow("Contours", drawing);
	 imshow("bigger", dst);
	 return final;
 }

