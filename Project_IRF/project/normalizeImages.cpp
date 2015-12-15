//
//  normalizeImages.cpp
//  ProjectIRF
//
//  Created by Cyrille Delabre on 08/12/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#include "normalizeImages.hpp"

void normalizeImages::process(bool saveNormalized, bool squareImg){
    fileOp *  op = new fileOp();
    if(saveNormalized) op->removeAllResNormalizedFiles();
    op->removeAllResSplittedFiles();

    
    double moy_cols = 0, moy_rows = 0;

    vector<string> result = op->getResultImages();
    //cout << "Taille result : "  << result.size() << endl;
    string current;
    //read images
    for (int i = 0; i < result.size(); i++){
        cout << "." << flush;
        Mat img = imread(result[i]);
        current = op->getFilename(result[i]);
        try{
            Mat res;
            Mat box = this->boundingBox(img, current);
            if(box.rows == 0 || box.cols == 0 )
            {
                if(u.RESULT)cout << "Error with image : "<< current <<endl;
                continue;
            }
            
            if(u.RESULT){
                moy_cols +=box.cols;
                moy_rows +=box.rows;
            }
            if(squareImg)
                res = getSquareImage(box,current);
            else
                res = box;
                

            if(u.VERBOSE) cout << "\nProcess... : \n" << current;
            //cout << i << endl;
            if(saveNormalized)op->writeNormalized(current,res);
            
          
            vector<Mat> splited = splitImage(u.SPLIT_FACTOR, res);
            for(int j = 0 ; j < splited.size(); j++)
            {
                stringstream   ss;
                ss << current << "_" << j;
                op->writeSplited(ss.str(),splited[j]);
            }
            
       
        }catch(Exception e){
            cout << "Error with image : "<< current <<endl;
        }
        
    }
    if(u.RESULT){
        double nb_img = result.size();
        if(nb_img !=0){
            moy_rows /=nb_img;
            moy_cols /=nb_img;
            cout << "Avg size cols : " << moy_cols << " \t Avg size rows : " << moy_rows <<endl;;
            cout << "Resized image size : " << MAX_SIZE <<"x" << MAX_SIZE<<endl;
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


cv::Mat normalizeImages::getSquareImage( const cv::Mat& img, string imgName )
{
    int width = img.cols,
    height = img.rows;
    
    cv::Mat square( MAX_SIZE, MAX_SIZE, img.type() );
    square.setTo(cv::Scalar(255,255,255));
    int max_dim = ( width >= height ) ? width : height;
    float scale = ( ( float ) MAX_SIZE ) / max_dim;
    cv::Rect roi;
    if ( width >= height )
    {
        roi.width = MAX_SIZE;
        roi.x = 0;
        roi.height = height * scale;
        roi.y = ( MAX_SIZE - roi.height ) / 2;
    }
    else
    {
        roi.y = 0;
        roi.height = MAX_SIZE;
        roi.width = width * scale;
        roi.x = ( MAX_SIZE - roi.width ) / 2;
    }
    
    cv::resize( img, square( roi ), roi.size() );
    
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
        blur(src_gray, src_gray, Size(3, 3));
            
        threshold(src_gray, threshold_output, THRESH_VALUE, 255, THRESH_BINARY);
        
        /// Find contours
        findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(1, 1));

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
        approxPolyDP(Mat(allcontours), poly, 15, true);
        Rect bounRect = boundingRect(Mat(poly));
//               Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));


//        Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);

//        rectangle(drawing, bounRect.tl(), bounRect.br(), color, 2, 8, 0);
        Mat final(img, bounRect);

//        imshow("Contours " + imgName, drawing);
//        imshow("final " + imgName, final);
//        waitKey(1);


//
//        
//    for (int i = 0; i < contours.size(); i++)
//    {
//        approxPolyDP(Mat(contours[i]), contours_poly[i], 15, true);
//        boundRect[i] = boundingRect(Mat(contours_poly[i]));
//      //  minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
//    }
//    
//    double max = 0; int k = 0;
//    /// Draw polygonal contour + bonding rects + circles
//    Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
//    Mat dst = Mat::zeros(threshold_output.size(), CV_8UC3);
//    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
//    for (int i = 0; i< contours.size(); i++)
//    {
//        
//        //drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
//        if (boundRect[i].area() > max && boundRect[i].area()<src.size().area() - 1000){
//            
//            max = boundRect[i].area();
//            k = i;
//        }
//        
//        //affiche le rectangle
//        rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
//        
//        circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
//    }
//    //rectangle(dst, boundRect[k].tl(), boundRect[k].br(), color, 2, 8, 0);
//    cout << boundRect[k].area() << endl;
//    // decouper les images
//    
//    Mat final(src, boundRect[k]);
//    if(VERBOSE){
//        
//        imshow("ROI "+ imgName, final);
//        /// Show in a window
//        namedWindow("Contours" + imgName, CV_WINDOW_AUTOSIZE);
//        imshow("Contours" + imgName, drawing);
//        //imshow("bigger" + imgName, dst);
//    }
   
    return final;
        
    }catch(Exception e ){
        cout << e.msg <<endl;
        throw e;
    }
}
