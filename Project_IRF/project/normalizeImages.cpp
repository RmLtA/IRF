//
//  normalizeImages.cpp
//  ProjectIRF
//
//  Created by Cyrille Delabre on 08/12/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#include "normalizeImages.hpp"
#include <future>
#include <dirent.h>


std::mutex NormalizeMtx;           // mutex for critical section
std::mutex OutputMtx;           // mutex for critical section



void normalizeImages::process(){
    auto resultImagesThread = std::async(&normalizeImages::getImages);
    if(askSquared){

        cout << "Crop images in squares ?  (Y :1 , N:0)" << endl;
        int in;
        cin >>in;
        squareImg = ((in == 1) ? 1 : 0);
        if(squareImg){
            int size;
            cout << "Size to resize images : (press 0 for " << sizeImg << ")"<<endl;
            cin >> size;
            if(size) sizeImg = size;
            cout << "OK, ";
        }
        
        if(squareImg) {
            cout << "Images 'll be resized in : " << sizeImg <<"x"<<sizeImg << " px" <<endl;
        }
        
 
    }else{
        squareImg = 0;
    }
    
    time_t xt = time(NULL);
    
    
    //retrieve async images
    vector<string> resultImages =resultImagesThread.get();
    
    
    if(u.BW){
        cout << "How much ?  ("<<resultImages.size()<<")" << endl;
        int in;
        cin >>in;
        
    
        random_shuffle(resultImages.begin(), resultImages.end());
        resultImages.resize(in);
    }
    
    
    unsigned long int nbImages =resultImages.size();
    
    cout << "Images to process : "  << nbImages * u.SPLIT_FACTOR << endl;
  
    
    
    u.SIZE_IMAGE = sizeImg;
   
    
    vector<thread> vThreads;
    int NB_THREADS = thread::hardware_concurrency();
    if(NB_THREADS ==0)NB_THREADS = 1;
    
    if(u.RESULT)cout <<"Using " << NB_THREADS << " threads  "<<endl;
    
    //init vars
    leftToProcess=resultImages.size();
    currentToProcess = 0 ;
    
    //init x processes
    for(int i=0 ; i < NB_THREADS ; i++){
        vThreads.push_back(std::thread(&normalizeImages::processTask, std::ref(*this), resultImages));
    }
    
    this_thread::yield();

    while(leftToProcess)
    {
        OutputMtx.lock();
        double pr=((double)currentToProcess/(double)nbImages)*100;
        cout<<"\t"<<setprecision(2)<<std::fixed<< pr<<"%   \r"<< flush ;
        OutputMtx.unlock();
        sleep(1); //pour windows a faire
        // http://stackoverflow.com/questions/10918206/cross-platform-sleep-function-for-c
        //ou yield si marche pas
    }
    //join all processes to be sure everyone has finished
    for(auto& th : vThreads){
        th.join();
    }
    double pr=((double)currentToProcess/(double)nbImages)*100;
    cout<<"\t"<<setprecision(2)<<std::fixed<< pr<<"%   \r"<< flush ;

    
    if(u.VERBOSE){
        if(nbImages !=0){ //prevent div 0 
            moy_rows /=(nbImages-nErroImg);
            moy_cols /=(nbImages-nErroImg);
            cout << endl << "Avg size cols : " << moy_cols << " \t Avg size rows : " << moy_rows <<endl;;
            cout << "Min size cols : " << min_cols << " \t Min size rows : " << min_rows <<endl;;
            cout << "Max size cols : " << max_cols << " \t Max size rows : " << max_rows <<endl;;


        }
    }
    
    //output time results
    xt = time(NULL) - xt;
    cout << endl << setprecision(3)
    << "Total : " << (currentToProcess-nErroImg)*u.SPLIT_FACTOR << " / " << nbImages*u.SPLIT_FACTOR << "\t"
    << "| Temps d'exec.: " << setprecision(2)<< (double)xt/60 <<" min" <<endl;;


}


//remove result images and get images async
vector<string> normalizeImages::getImages(){
    fileOp *  op = new fileOp();
    op->removeAllResNormalizedFiles();
    op->removeAllResSplittedFiles();
//    cout << "Result dir cleaned" << endl;
    vector<string> resultImages = op->getResultImages();
//    cout << "Get images done" << endl;
    delete op;
    return resultImages;
}



//process task normalize image for threads async
void normalizeImages::processTask(normalizeImages& self,vector<string> resultImages)
{
    utils & u = utils::i();
    fileOp * op = new fileOp();
    
    
    while(self.leftToProcess)
    {
        NormalizeMtx.lock();
        if(self.leftToProcess==0) break;
        self.leftToProcess--;
        string sourceImage = resultImages[self.currentToProcess++];
        NormalizeMtx.unlock();

        
        Mat img = imread(sourceImage);
        string current = op->getFilename(sourceImage);
        try{
            Mat res;
            Mat box = normalizeImages::boundingBox(img, current);
            if(box.rows == 0 || box.cols == 0 )
            {
                if(u.VERBOSE)cout << "Error with image : "<< current <<endl;
                self.nErroImg++;
                continue;
            }
            if(!self.squareImg){
                if(box.rows < MIN_SIZE || box.cols < MIN_SIZE ){
                    if(u.VERBOSE)cout << "Rejected image : "<< current << " => image res is to low" << endl;
                    self.nErroImg++;
                    continue;
                }
            }
            
            if(u.VERBOSE){
                NormalizeMtx.lock();
                self.moy_cols +=box.cols;
                self.moy_rows +=box.rows;
                if(self.min_cols > box.cols)
                    self.min_cols =  box.cols;
                if(self.max_cols < box.cols)
                    self.max_cols = box.cols;
                    
                if(self.min_rows > box.rows)
                    self.min_rows =  box.rows;
                if(self.max_rows< box.rows)
                    self.max_rows = box.rows;

                NormalizeMtx.unlock();

            }

            
            if(self.squareImg)
                res = normalizeImages::getSquareImage(box,current, self.sizeImg);
            else
                res = box;

            if(u.BW)
                op->writeByName(current,res);
            else
                op->writeNormalized(current,res);
            
            if(u.SPLIT_FACTOR >1){
                vector<Mat> splited = normalizeImages::splitImage(u.SPLIT_FACTOR, res);
                for(int j = 0 ; j < splited.size(); j++)
                {
                    stringstream   ss;
                    ss << current << "_" << j;
                    op->writeSplited(ss.str(),splited[j]);
                }
            }
      
                        
        }catch(Exception e){
            cout << "Error with image : "<< current <<endl;
        }
    }
    
    delete op;
}



//Divide the image into x parts/squares;
vector<Mat>  normalizeImages::splitImage(int x, Mat const & src){
    vector<Mat> smallImages;
    int height = src.rows;
    int width =src.cols;
    double factor = sqrt(x);
    try{
        Size smallSize ((1/factor)*width,(1/factor)*height); // Size of the squares
        
        double smallHeight =(1/factor)*height;
        double smallWidth =(1/factor)*width ;
        double offsetHeight =smallHeight/2 ;
        double offsetWidth =smallWidth/2;
        for  ( double y =  0 ; y < height-offsetHeight; y += smallHeight )
        {
            for  ( double x =  0 ; x < width-offsetWidth; x += smallWidth)
            {
                Rect rect =  Rect ( x, y, smallWidth , smallHeight );
                Mat mat =Mat(src , rect);
                smallImages.push_back (mat); // constructor Mat (Mat, roi)
            }
        }
    }catch(Exception e){
        cout << e.msg;
        throw e;
    }
    return smallImages;
}


cv::Mat normalizeImages::getSquareImage( const cv::Mat& img, string imgName, int sizeImg)
{
    int width = img.cols,
    height = img.rows;
//    utils & u = utils::i();
    cv::Mat square( sizeImg, sizeImg, img.type() );
   // if(!u.BW)
        square.setTo(cv::Scalar(255,255,255));
   // else
   //     square.setTo(cv::Scalar(0,0,0));

    int max_dim = ( width >= height ) ? width : height;
    float scale = ( ( float ) sizeImg ) / max_dim;
    cv::Rect roi;
    if ( width >= height )
    {
        roi.width = sizeImg;
        roi.x = 0;
        roi.height = height * scale;
        roi.y = ( sizeImg - roi.height ) / 2;
    }
    else
    {
        roi.y = 0;
        roi.height = sizeImg;
        roi.width = width * scale;
        roi.x = ( sizeImg - roi.width ) / 2;
    }
   
    cv::resize( img, square( roi ), roi.size() , 0, 0 ,CV_INTER_AREA  );
    
    return square;
}






/**
 * \fn Mat boundingBox
 * \brief extrait l'image
 *
 * \param src : image source
 * \return Mat
 */
Mat normalizeImages::boundingBox(const cv::Mat& img, string imgName)
{
    
    try{
 

        Mat src_gray;
        RNG rng(12345);
        Mat threshold_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        /// Convert image to gray and blur it
        cvtColor(img, src_gray, CV_BGR2GRAY);
        blur(src_gray, src_gray, Size(5, 5));
            
        threshold(src_gray, threshold_output, THRESH_VALUE, 255, THRESH_BINARY);
        
        /// Find contours
        findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(1,1));

        /// Approximate contours to polygons + get bounding rects and circles
        vector<vector<Point> > contours_poly(contours.size());
        vector<Rect> boundRect(contours.size());
        vector<Point2f>center(contours.size());
        vector<float>radius(contours.size());
        vector<Point> allcontours;

        for(int i = 0 ;i< contours.size() ; i++){
            
            for(int j= 0 ; j< contours[i].size() ; j++){
                if(contours[i][j].x > IMG_GAP && contours[i][j].x < img.cols -IMG_GAP &&
                   contours[i][j].y > IMG_GAP && contours[i][j].y < img.rows -IMG_GAP ){
                    allcontours.push_back(contours[i][j]);
                    

                }
            }
        }
        
        if(allcontours.size() == 0)return Mat();
     

        vector<Point> poly(allcontours.size());
        approxPolyDP(Mat(allcontours), poly, 5, true);
        Rect bounRect = boundingRect(Mat(poly));

        Mat final(img, bounRect);
   
    return final;
        
    }catch(Exception e ){
        cout << e.msg <<endl;
        throw e;
    }
}
