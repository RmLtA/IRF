//
//  normalizeImages.cpp
//  ProjectIRF
//
//  Created by Cyrille Delabre on 08/12/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#include "normalizeImages.hpp"

void normalizeImages::process(){
    fileOp *  op = new fileOp(TEST);

    vector<string> result = op->getResultImages();
    cout << "Taille result : "  << result.size() << endl;
    string current;
    //read images
    for (int i = 0; i < result.size(); i++){
        Mat img = imread(result[i]);
        current = op->getFilename(result[i]);
        Mat res = this->boundingBox(img, current);

        cout << "Process... : " << current<< endl;
        cout << i << endl;
        op->writeNormalized(current,res, VERBOSE);
        
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
Mat normalizeImages::boundingBox(Mat src, string imgName)
{
    
    try{
        
        
        //removing borders...
    //Mat cropedImage (src , Rect(10,10,src.cols-10,src.rows-10));

    Mat src_gray;
    RNG rng(12345);
    int thresh = 200;
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Convert image to gray and blur it
    cvtColor(src, src_gray, CV_BGR2GRAY);
    blur(src_gray, src_gray, Size(3, 3));
        
//        Size kernalSize (2,2);
//        Mat element = getStructuringElement (MORPH_RECT, kernalSize, Point(1,1)  );
//        morphologyEx( src_gray, src_gray, MORPH_BLACKHAT, element );
//        imshow("morph" + imgName, src_gray);
    /// Detect edges using Threshold
    threshold(src_gray, threshold_output, thresh, 255, THRESH_BINARY);
        
        
      
    /// Find contours
    findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(1, 1));
    
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<Point2f>center(contours.size());
    vector<float>radius(contours.size());
    vector<Point> allcontours;
        cout << "for" <<endl;

    for(int i = 0 ;i< contours.size() ; i++){
        
        for(int j= 0 ; j< contours[i].size() ; j++){
            if(contours[i][j].x > 10 && contours[i][j].x < src.cols -10 &&
               contours[i][j].y > 10 && contours[i][j].y < src.rows -10 ){
                allcontours.push_back(contours[i][j]);
                //cout << "push" <<endl;
                

            }
        }
    }
        
        cout << "endfor" <<endl;
        cout << allcontours.size() <<endl;

        vector<Point> poly(allcontours.size());
        approxPolyDP(Mat(allcontours), poly, 15, true);
        Rect bounRect = boundingRect(Mat(poly));
        Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));


        rectangle(drawing, bounRect.tl(), bounRect.br(), color, 2, 8, 0);
        Mat final(src, bounRect);

        imshow("Contours " + imgName, drawing);
        imshow("final " + imgName, final);


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


Mat normalizeImages::Box( Mat src, string imgName){
    // ---- Preprocessing of depth map. (Optional.) ----
    /// Convert image to gray and blur it
    
    Mat matInput;
    cvtColor(src, matInput, CV_BGR2GRAY);
  //  blur(src_gray, src_gray, Size(3, 3));
    GaussianBlur(matInput, matInput, cv::Size(9, 9), 4.0);
    
    // ---- Here, we use cv::threshold instead of cv::Canny as explained above ----
    
    Mat matEdge;
    
    //Canny(matInput, matEdge, 0.1, 1.0);
    threshold(matInput, matEdge, 245, 255, THRESH_BINARY);

    
    // ---- Use findContours to find chains of consecutive edge pixels ----
    
    vector<vector<Point> > contours;
    findContours(matEdge, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    // ---- Code below is only used for visualizing the result. ----
    
    Mat matContour(matEdge.size(), CV_8UC1);
    
    for (size_t k = 0; k < contours.size(); ++k)
    {
        const vector<Point>& contour = contours[k];
        for (size_t k2 = 0; k2 < contour.size(); ++k2)
        {
            const Point& p = contour[k2];
            matContour.at<uint8_t>(p) = 255;
        }
    }
    imshow("ROI "+ imgName, matContour);

    cout << "Done!" << endl;
    return matContour;
}
