//
//	computeImages.cpp
//  projectIRF
//

#include "computeImages.h"


/**
 * \fn getTemplArea()
 * \brief retourne l'image (matrice) courante de la zone contenant les imagettes définie par le template
 * \return Mat
 */
Mat computeImages::getTemplArea(){
    return this->currentTemplArea;
}
/**
 * \fn getTemplArea()
 * \brief retourne la position Y courante de la zone contenant les imagettes définie par le template
 * \return int
 */
int computeImages::getPositionY(){
    return this->templPos.y;
}

/**
* \fn removeZone()
* \brief Suppression de la zone définie par un rectangle où des imagettes des icônes ont été détectés 
*
*/
void computeImages::removeZone(){
    rectangle(processingImg,templPos,Point(templPos.x+(200),templPos.y+(200)) ,Scalar(255,0,0) , -1);
}

/**
* \fn showFinalImage(string imageName)
* \brief affichage de l'image courante reduite
* \param imageName 
*/
void computeImages::showFinalImage(string imageName){
    Mat imreduite;
    int reduction = 8;
    Size tailleReduite(this->processingImg.cols / reduction, this->processingImg.rows / reduction);
    imreduite = Mat(tailleReduite, CV_8UC3); //cree une image à 3 canaux de profondeur 8 bits chacuns
    resize(this->processingImg, imreduite, tailleReduite);
    imshow(imageName, imreduite);
}

/**
 * \fn reduceImage(Mat image)
 * \brief reduit l'image avec un coefficient de reduction @REDUCTION
 * \param Mat image
 * \return Mat reduite
 */
Mat computeImages::reduceImage(Mat image, double reduction){
    Mat imreduite;
    Size tailleReduite(image.cols / reduction, image.rows / reduction);
    imreduite = Mat(tailleReduite, CV_8UC3); //cree une image à 3 canaux de profondeur 8 bits chacuns
    resize(image, imreduite, tailleReduite);
    return imreduite;
}


/**
* \fn findTemplArea(Mat templ, string currentName)
* \brief Reconnaissance des icones modèles 
*		 Définition d'une région de recherche des icones reproduites 
*
* \param imageName
* \return bool : TRUE si template trouvée 
*/
bool computeImages::findTemplArea(Mat templ, string currentName, bool secondPass)
{
    double reduction;
    double myThresholdVal;
    if(secondPass){
        myThresholdVal=NO_R_THRESHOLD_VAL;
        reduction = NO_REDUCE;

    }else{
        reduction = REDUCTION;
        myThresholdVal = THRESHOLD_VAL;
    }
    
    
    //valeur de seuil
    //params threshold
    double thresh = 0.1;
    double maxValue = 1.;
    double acceptance = 0.10;
    
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;
    Mat result;
    Mat gref, gsrc;
    
    Mat reducedMat = reduceImage(this->processingImg, reduction);
    Mat reducedTempl = reduceImage(templ, reduction);
	/// Create the result matrix
    int result_cols = reducedMat.cols - reducedTempl.cols ;//+ 1; /* why +1 ? */
    int result_rows = reducedMat.rows - reducedTempl.rows ;//+ 1; /* why +1 ? */
	result.create(result_rows, result_cols, CV_32FC1);

	
    
    //reducing the searching zone
    Mat zone(reducedMat, Rect(Point(0,0), Point(reducedMat.cols /4 ,reducedMat.rows )));
    //convert both images to GRAY
	cvtColor(zone, gref, CV_BGR2GRAY);
	cvtColor(reducedTempl, gsrc, CV_BGR2GRAY);
   // imshow("aa" , zone);
   // waitKey();
    //matching templates
    matchTemplate(gref, gsrc, result, match_method);
	//see threshold method...
    threshold(result, result, thresh, maxValue, THRESH_TOZERO);
    /// Localizing the best match with minMaxLoc
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    bool founded = false;

    if(maxVal >= myThresholdVal)
    {
        founded = true;
        //matchLoc = maxLoc;
        Point matchLoc = Point(maxLoc.x * reduction , maxLoc.y * reduction);
        //get the area with the "imagettes"
        Mat ligne(this->processingImg, Rect(Point(matchLoc.x+templ.cols,matchLoc.y-templ.rows/1.5), Point(this->processingImg.cols, matchLoc.y + templ.rows*2)));
       
        //save area and position
        this->currentTemplArea = ligne;
        this->templPos = matchLoc;
        
         if(verbose)  imshow(currentName, ligne);
      
    }else if(maxVal > (myThresholdVal - acceptance ) && secondPass != true){
        if(verbose)
        {   stringstream ss;
            
            cout << " \t\t ~~~~~~";
            ss<< "  values ::" <<" minLoc : " << minLoc << "  maxLoc : " << maxLoc << " \t minVal : " << minVal << "  maxVal : " << maxVal;
            string s = ss.str(); s.resize(100, ' ');
            cout << s << " trying again..." << endl;;
            
        }
        return findTemplArea(templ, currentName, true);
    }
    
    if(verbose)
    {   stringstream ss;

        cout << (secondPass ? " \t\t\t\t\t\t\t ===> " :" \t\t ")
        << (founded ? "//////" : "||||||");
        ss<< "  values ::" <<" minLoc : " << minLoc << "  maxLoc : " << maxLoc << " \t minVal : " << minVal << "  maxVal : " << maxVal;
        string s = ss.str(); s.resize(100, ' ');
        cout << s << "" << (founded ? "found " : " not found ");
      

    }
    return founded;
    
}



/**
* \fn vector<Vec4i> computeImages::findLines(Mat imgSource)
* \brief Détection des lignes dans la feuille de test grâce au transformée de Hough (HoughLinesP). 
*		 L'algorithme regarde tous les points d’une image et vérifie tous les angles entre ceux ci pour déterminer les points qui forment des lignes. 
*
* \param imgSource
* \return vector de lignes 
*/
vector<Vec4i> computeImages::findLines(Mat imgSource){
    try{
        Mat img;
        //convert to grayscale
        
        //Apply blur to smooth edges and use adapative thresholding
        //Size size(3,3);
        //GaussianBlur(imgSource,img,size,0);
        cv::cvtColor(imgSource, img, CV_BGR2GRAY);
        //apply
        adaptiveThreshold(img, img,255,CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY,75,10);
        bitwise_not(img, img);
        //find lines
        vector<Vec4i> lines;
        HoughLinesP(img, lines, 1, CV_PI/180, 80, 100, 10);
        
        return lines;
        
    }catch(Exception e){
        
        std::cout << "computeImages::findLines error ::: " << e.msg <<endl ;
        throw e;
    }
}
  
/**
* \fn vector<Mat> computeImages::findImages(vector<Vec4i> lines, Mat imgSource)
* \brief Détection des lignes formant des polygones en particulier des rectangles.
*		 La vérification se base sur un calcul d’intersection de segments.
*		 En classant les segments qui s’intersectent dans le même groupe, on regroupe les segments qui appartiennent au même polygone.
*		 On trouve également la position des coins des polygones.
*		 Si on a plus de 3 coins le polygone, on classe le polygone comme un rectangle.
*
* \param lines : vector de lignes
* \param imgSource
* \return vector<Mat>
*/
int templ = 0;

vector<Mat> computeImages::findImages(vector<Vec4i> lines, Mat imgSource){
    try{

        int* poly = new int[lines.size()];
        for(int i=0;i<lines.size();i++)poly[i] = - 1;
        
        
        int curPoly = 0;

        vector<vector<cv::Point2f> > corners;
        for (int i = 0; i < lines.size(); i++)
        {
            for (int j = i+1; j < lines.size(); j++)
            {
                cv::Point2f pt = computeIntersect(lines[i], lines[j]);
                //if inside the image
                if (pt.x > 1 && pt.y > 1 && pt.x<imgSource.size().width && pt.y<imgSource.size().height){
                    // if undefined -> new group
                    if(poly[i]==-1 && poly[j] == -1){
                        vector<Point2f> v;
                        v.push_back(pt);
                        corners.push_back(v);
                        poly[i] = curPoly;
                        poly[j] = curPoly;
                        curPoly++;
                        continue;
                    }
                    // if only one -> add the other line into group
                    if(poly[i]==-1&&poly[j]>=0){
                        corners[poly[j]].push_back(pt);
                        poly[i] = poly[j];
                        continue;
                    }
                    // same
                    if(poly[i]>=0&&poly[j]==-1){
                        corners[poly[i]].push_back(pt);
                        poly[j] = poly[i];
                        continue;
                    }
                    //if both are defined then add all the lines from one group into the other
                    if(poly[i]>=0&&poly[j]>=0){
                        if(poly[i]==poly[j]){
                            corners[poly[i]].push_back(pt);
                            continue;
                        }
                        
                        for(int k=0;k<corners[poly[j]].size();k++){
                            corners[poly[i]].push_back(corners[poly[j]][k]);
                        }
                        
                        corners[poly[j]].clear();
                        poly[j] = poly[i];
                        continue;
                    }
                }
            }
        }
        

        vector<vector<cv::Point2f> > saved ;

        for(int i=0;i<corners.size();i++){
            cv::Point2f center(0,0);
            if(corners[i].size()>4){

                for(int j=0;j<corners[i].size();j++){
                    center += corners[i][j];
                }
                center *= (1. / corners[i].size());
                if(sortCorners(corners[i], center)){
                    //saving matching rectangles
                    saved.push_back(std::vector<cv::Point2f>(corners[i]));
                }
            }
        }

        sort(saved.begin(),saved.end(),rectComparator);


        vector<Mat> returnImages ;
        //process retransformation

        for(int i=0;i<saved.size();i++){
            if(saved[i].size()<4)continue;
            Rect r = boundingRect(saved[i]);
            if(r.area()<50000){
                continue;
            }
            //removing borders...
            Rect rr(Point(r.tl().x +  10, r.tl().y +  10) ,Point(r.br().x-10,r.br().y-10));
            
            Mat res(imgSource, rr);
            
            
            if(verbose){
                stringstream ss;
                ss << "imagette :: " << " " << templ << " " << i ;
                
                imshow(ss.str(), res);
            }
            returnImages.push_back(res);
        
        }
        
        
        if(verbose){
            //affichage lignes trouvées
            Mat area = imgSource.clone();
            for(size_t i = 0; i < lines.size(); i++ ) {
                Vec4i l = lines[i];
                line(area, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
            }
            
            Mat imreduite;
            int reduction =2;
            Size tailleReduite(area.cols / reduction, area.rows / reduction);
            imreduite = Mat(tailleReduite, CV_8UC3); //cree une image à 3 canaux de profondeur 8 bits chacuns
            resize(area, imreduite, tailleReduite);
            stringstream ss;
            ss << "image :: " << " " << templ++ ;

            
            imshow(ss.str(), imreduite);
            
            
            
            
        }
        return returnImages;
    }catch(Exception e){
        std::cout << "computeImages::findImages" <<endl ;
        throw e;
    }
  
}




Point2f computeImages::computeIntersect(cv::Vec4i a, cv::Vec4i b){
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
    //variable d'intersection
    int avg = AVG_INTERSECTION;
    // if not parallel
    if (float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4)))
    {
        Point2f pt;
        pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
        pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;
        if(pt.x<min(x1,x2)-avg||pt.x>max(x1,x2)+avg||pt.y<min(y1,y2)-avg||pt.y>max(y1,y2)+avg){
            return Point2f(-1,-1);
        }
        if(pt.x<min(x3,x4)-avg||pt.x>max(x3,x4)+avg||pt.y<min(y3,y4)-avg||pt.y>max(y3,y4)+avg){
            return Point2f(-1,-1);
        }
        return pt;
    }
    else
        return Point2f(-1, -1);
}


bool computeImages::pointsComparator(Point2f a,Point2f b){
    return a.x<b.x;
}
bool computeImages::rectComparator(std::vector<cv::Point2f>& a, std::vector<cv::Point2f>& b){
    return  a.at(0).x < b.at(0).x;
}

/**
* \fn sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
* \brief Trie les coins de chaque rectangle dans le sens horaire si possible
*
* \param corners 
* \param center
* \return success
*/
bool computeImages::sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center){
    std::vector<cv::Point2f> top, bot;
    for (int i = 0; i < corners.size(); i++)
    {
        if (corners[i].y <= center.y)
            top.push_back(corners[i]);
        else
            bot.push_back(corners[i]);
    }
    // least 2 points top and bottom
    if(top.size()>1 && bot.size()>1)
    {
        sort(top.begin(),top.end(),pointsComparator);
        sort(bot.begin(),bot.end(),pointsComparator);
        cv::Point2f tl = top[0];
        cv::Point2f tr = top[top.size()-1];
        cv::Point2f bl = bot[0];
        cv::Point2f br = bot[bot.size()-1];
        corners.clear();
        corners.push_back(tl);
        corners.push_back(tr);
        corners.push_back(br);
        corners.push_back(bl);
        return true;
    }else{
        return false;
    }
        
}



