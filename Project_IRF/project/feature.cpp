#include "feature.h"

//TODO : SI feature est de l'image globale ou si c'est d'une image splited
// voir extractfeatures

// Count the number of black pixels
double feature::countBlackPixel(){
	
    vector<Point> black_pixels;
    //invert image
    threshold(this->graySourceImg, this->binaryImage, 1, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
    
    // output, locations of non-zero pixels
    cv::findNonZero(this->binaryImage, black_pixels);
    
    double res = (double) black_pixels.size()/ (double)(sourceImg.cols*sourceImg.rows) * 100;
    if(u.VERBOSE){
        
        imshow("Black pixels " + imgName, this->binaryImage);
        cout << "Black pixels  :"<< imgName << " : "<< res << endl;
        waitKey(1);
    }
    
    return res;
}


// Count the number of Harris Corner and get the mass center of all the points
//PRIVATE
void feature::countHarrisCorners(){
    //TODO check the params
    //try to get values that are the same for the same type...
    //check for splited images and global...
    int thresh;
    int blockSize;
    int apertureSize;
    double k = 0.03;
    
    if(isGlobal){
        thresh = 180;
        blockSize = 25;
        apertureSize = 3;
    }else{
        thresh = 230;
        blockSize = 4;
        apertureSize = 3;
        //params
    }
    
	Mat dst, dst_norm;
	dst = Mat::zeros(this->sourceImg.size(), CV_32FC1);
	

	/// Detecting corners
	cornerHarris(this->graySourceImg, dst, blockSize, apertureSize, k, BORDER_DEFAULT);
	/// Normalizing from 0 to 255
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    Mat display = this->sourceImg.clone();
	int cornerHarris = 0;
	/// Drawing a circle around corners
    double moyX=0;
    double moyY=0;
	for (int j = 0; j < dst_norm.rows; j++)
	{
		for (int i = 0; i < dst_norm.cols; i++)
		{
			if ((int)dst_norm.at<float>(j, i) > thresh)
			{
				cornerHarris++;
                moyX += i;
                moyY +=j;
                if(u.VERBOSE)circle( display, Point( i, j ), 5,  Scalar(0), 2, 8, 0 );

			}
		}
	}
    if(cornerHarris){
        moyX = moyX/cornerHarris;
        moyY = moyY/cornerHarris;
        moyX = moyX /  dst_norm.cols * 100;
        moyY = moyY / dst_norm.rows * 100;

    }else{
        moyX =0;
        moyY =0;
    }
   // if(u.VERBOSE)circle( display, Point( moyX, moyY ), 5,  Scalar(3), 2, 8, 0 );
   
    cornerHarrisPoint = Point2f(moyX, moyX);
    if(u.VERBOSE){
        imshow("Harris corners ", display);
        cout << "Harris :"<< imgName << " : "<< cornerHarris << endl;
        cout << " X " << moyX << " Y " << moyY << endl;
        waitKey(1);
    }

}


double feature::harrisCornerX(){
    if(cornerHarrisPoint.x ==-1 && cornerHarrisPoint.y ==-1)
        countHarrisCorners();
    
    return cornerHarrisPoint.x;
}
double feature::harrisCornerY(){
    if(cornerHarrisPoint.x ==-1 && cornerHarrisPoint.y ==-1)
        countHarrisCorners();

    return cornerHarrisPoint.y;
}


//global feature
//if rows are bigger then 1
//else 0
int feature::isLongerRowsOrCols(){
    int rows = this->sourceImg.rows;
    int cols = this->sourceImg.cols;
    if(  rows == cols ){
        //this means image has beene normalized in equals squares
        //so we need to check the one that has more white pixels
        int countRows = 0 , countCols = 0;
        //TODO :: dont really works...
        //chekc better implementation
        for( int i = 0; i < rows; i++ ) {
                if ( this->graySourceImg.at<uchar>(i,0) != 255  &&
                    this->graySourceImg.at<uchar>(i,1) == 255)
                    countRows++;
            
        }
        for( int i = 0; i < cols; i++ ) {
            if ( this->graySourceImg.at<uchar>(0,i) == 255 &&
                this->graySourceImg.at<uchar>(1,i) == 255)
                    countCols++;
            
        }
        if(countRows == countCols)
            return -1;
        if(countRows > countCols)
            return 1;
        else
            return 0;
    
    }else if(rows>cols){
        return 1;
    }else{
        return 0;
    }
}

void feature::HoughLines(){
    int resolution;
    int threshold ;
    int minLinLength;
    int maxLineGap;
    int AVG;

    //    //TODO check better params
    if(isGlobal){
        resolution = 1;
        threshold = 30;
        minLinLength =3;
        maxLineGap = 3;
        AVG= 10;
    }else{
        //faire en fonction du splitted params
        resolution = 1;
        threshold = 10;
        minLinLength =3;
        maxLineGap = 5;
        AVG = 5;
    }
    
    vector<Vec4i> lines;
    
    Mat dst, cdst;
    //check canny params also...
    Canny(this->sourceImg, dst, 50, 200, 3);
    cdst = dst.clone();
    HoughLinesP(dst, lines, resolution, CV_PI/180, threshold,minLinLength , maxLineGap );
    
    double horizontal=0, vertical=0, diagonalPos=0, diagonalNeg=0;
        for( size_t i = 0; i < lines.size(); i++ )
        {
            Vec4i l = lines[i];
            if(u.VERBOSE)line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(180,8,255), 3, CV_AA);
          
            if(abs(l[0] - l[2]) < AVG)             /*check if line is | */
                vertical++;
            else if(abs(l[1] - l[3]) < AVG)             /*check if line is -*/
                horizontal++;
            else if(l[1] < l[3])                 /*check if line is \ */
                diagonalNeg++;
            else if(l[1] > l[3])                /*check if line is / */
                diagonalPos++;
          
            
        
            
        }
    
    if(lines.size() > 0){
        houghLines[0] = (vertical / (double) lines.size()) * 100;
        houghLines[1] = (horizontal / (double) lines.size()) * 100;
        houghLines[2] = (diagonalNeg / (double) lines.size()) * 100;
        houghLines[3] = (diagonalPos / (double) lines.size()) * 100;
    }else{
        houghLines[0] = 0;
        houghLines[1] = 0;
        houghLines[2] = 0;
        houghLines[3] = 0;
        
    }
    if(u.VERBOSE){
        imshow("Hough lines", cdst);
        waitKey(1);
       // imshow("Canny ", dst);

        cout << "Hough :" << imgName << " : " <<  lines.size()<< endl;
    }
 }

double feature::houghLinesVerticals(){
    if(houghLines[0] == -1)
        HoughLines();
    
    return houghLines[0];
}
double feature::houghLinesHorizontals(){
    if(houghLines[0] == -1)
        HoughLines();
    
    return houghLines[1];
}
double feature::houghLinesDiagonalNegs(){
    if(houghLines[0] == -1)
        HoughLines();
    
    return houghLines[2];
}
double feature::houghLinesDiagonalPos(){
    if(houghLines[0] == -1)
        HoughLines();
    
    return houghLines[3];
}

double feature::countArea(){
    
    //    //TODO
    //    //
    //    if(isGlobal){
    //        //params
    //    }else{
    //        //params
    //    }
    Mat src_gray;
    //RNG rng(12345);
//    int thresh = 100;
    //int IMG_GAP = 1;
  //  cvtColor( src, src_gray, CV_BGR2GRAY );
    blur( this->graySourceImg, src_gray, Size(1,1) );
    Mat img;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using canny
    Canny( src_gray, img, 0, 300, 3 );
    /// Find contours
    findContours( img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
 
    vector<Point> allcontours;
    
    for(int i = 0 ;i< contours.size() ; i++){
        
        for(int j= 0 ; j< contours[i].size() ; j++){
//            if(contours[i][j].x > IMG_GAP && contours[i][j].x < img.cols -IMG_GAP &&
//               contours[i][j].y > IMG_GAP && contours[i][j].y < img.rows -IMG_GAP ){
                allcontours.push_back(contours[i][j]);
                
                
//            }
        }
    }
    vector<Point> approx;

    if(allcontours.size() >0){
        approxPolyDP(allcontours, approx, 1, true);
        return (double) contourArea(approx) / (double)(this->graySourceImg.rows * this->graySourceImg.cols);

    }
    else
        return 0;
}


double feature::countLengthArea(){
    
    //    //TODO
    //    //
    //    if(isGlobal){
    //        //params
    //    }else{
    //        //params
    //    }
    Mat src_gray;
    RNG rng(12345);
    
    blur( this->graySourceImg, src_gray, Size(3,3) );
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using canny
    Canny( src_gray, canny_output, 0, 300, 3 );
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );
    
    
    vector<Point> allcontours;
    
    for(int i = 0 ;i< contours.size() ; i++){
        
        for(int j= 0 ; j< contours[i].size() ; j++){
            //            if(contours[i][j].x > IMG_GAP && contours[i][j].x < img.cols -IMG_GAP &&
            //               contours[i][j].y > IMG_GAP && contours[i][j].y < img.rows -IMG_GAP ){
            allcontours.push_back(contours[i][j]);
            
            
            //            }
        }
    }
    if(allcontours.size() >0){
        return (double) arcLength(allcontours, false) / (double)(this->graySourceImg.rows * this->graySourceImg.cols);
        
    }
    else
        return 0;
}

double feature::massCenterX(){
    
    if(this->massCenter.x == -1 && this->massCenter.y == -1)
        feature::countMassCenter();
    
    if(!isnan(this->massCenter.x))
        return this->massCenter.x / this->sourceImg.cols;
    else
        return 0;
}


double feature::massCenterY(){
    if(this->massCenter.x == -1 && this->massCenter.y == -1)
        feature::countMassCenter();
    
    if(!isnan(this->massCenter.y))
        return this->massCenter.y / this->sourceImg.rows;
    else
        return 0;
}


//PRIVATE
void feature::countMassCenter(){
    //    //TODO
    //    //
    //    if(isGlobal){
    //        //params
    //    }else{
    //        //params
    //    }
    if(this->massCenter.x == -1 && this->massCenter.y == -1){
        int thresh = 140;
        Mat src_gray;
        RNG rng(12345);
        
        blur( this->graySourceImg, src_gray, Size(3,3) );
        Mat canny_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        
        /// Detect edges using canny
        Canny( src_gray, canny_output, thresh, 200, 3 );
        /// Find contours
        findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        
        vector<Moments> mu(contours.size() );
        vector<Point2f> mc( contours.size() );
        for( int i = 0; i < contours.size(); i++ )
        {
            mu[i] = moments( contours[i], false );
            if(mu[i].m00 == 0)
                mc[i] = Point2f(0,0);
            else
                mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
        }
        
        double size = contours.size();
        float totalX=0.0, totalY=0.0;
        for(int i=0; i<size; i++) {
            totalX+=mc[i].x;
            totalY+=mc[i].y;
        }
        
        Point2f massCenter(totalX/size, totalY/size); // condition: size != 0
        this->massCenter = massCenter;
    }
}


