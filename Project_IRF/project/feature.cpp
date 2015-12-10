#include "feature.h"



// Count the number of black pixels
int feature::countBlackPixel(){
	if (!this->sourceImg.data)
		cerr << "Problem loading image from : countBlackPixel()" << endl;

    computeBlackPixels(); //TODO if already done
	return (int) black_pixels.size();
}




//PRIVATE
void feature::computeBlackPixels(){
   // Mat bw;
    // Transform it to binary and invert it. White on black is needed.
    threshold(this->graySourceImg, this->binaryImage, 40, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
    
    // output, locations of non-zero pixels
    cv::findNonZero(this->binaryImage, black_pixels);
  //  return bw;
}




int feature::countWhitePixel(){
    if (!this->sourceImg.data)
        cerr << "Problem loading image from : countWhitePixel()" << endl;
    
    
    computeBlackPixels();//TODO if already done
	// extract only the external blob
	findContours(this->binaryImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	Mat mask = Mat::zeros(this->binaryImage.size(), CV_8UC1);
	vector<Point> all_pixels;   // output, locations of non-zero pixels
	cv::findNonZero(mask, all_pixels);

	int white_pixels = (int) ( all_pixels.size() - black_pixels.size());
	return white_pixels;
}

// Count the number of Harris Corner
int feature::countHarrisCorners(){

	Mat dst, dst_norm, dst_norm_scaled;
	dst = Mat::zeros(this->sourceImg.size(), CV_32FC1);

	/// Detector parameters
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;

	/// Detecting corners
	cornerHarris(this->graySourceImg, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

	/// Normalizing
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);

	int cornerHarris = 0;
	/// Drawing a circle around corners
	for (int j = 0; j < dst_norm.rows; j++)
	{
		for (int i = 0; i < dst_norm.cols; i++)
		{
			if ((int)dst_norm.at<float>(j, i) > thresh)
			{
				cornerHarris++;
			}
		}
	}

	return cornerHarris;
}



double feature::countArea(){
    Mat src_gray;
    RNG rng(12345);
        
  //  cvtColor( src, src_gray, CV_BGR2GRAY );
    blur( this->graySourceImg, src_gray, Size(3,3) );
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using canny
    Canny( src_gray, canny_output, thresh, thresh*2, 3 );
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
 
    double max = 0;
    for(int i=0; i<contours.size() ; i++){
        double tmp = contourArea(contours[i]);
        if(tmp > max) max = tmp;
    }
    return max;
}


double feature::countLengthArea(){
    Mat src_gray;
    RNG rng(12345);
    
    blur( this->graySourceImg, src_gray, Size(3,3) );
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using canny
    Canny( src_gray, canny_output, thresh, thresh*2, 3 );
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    double max = 0;
    for(int i=0; i<contours.size() ; i++){
        double tmp = arcLength( contours[i], true );
        if(tmp > max) max = tmp;
    }
    return max;
}
int ww = 0;
//Attention les attributs de weka sont des valeurs simples numeric 
void feature::countMassCenter(){
    if(this->massCenter.x == -1 && this->massCenter.y == -1){
        Mat src_gray;
        RNG rng(12345);
        
        
        //    blur( this->graySourceImg, src_gray, Size(3,3) );
        //    Moments mu = moments(src_gray, true);
        //    Point2f center;
        //    if(mu.m00 == 0)
        //        this->massCenter = Point2f(-1,-1);
        //    else {
        //
        //
        //
        //        center.x = mu.m10 / mu.m00;
        //        center.y = mu.m01 / mu.m00;
        //
        //        this->massCenter =center;
        //
        //
        //
        //
        //    }
        //
        
        blur( this->graySourceImg, src_gray, Size(3,3) );
        Mat canny_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        
        /// Detect edges using canny
        Canny( src_gray, canny_output, thresh*1.5, thresh*2, 3 );
        /// Find contours
        findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        
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
        float distance;
        float totalX=0.0, totalY=0.0;
        for(int i=0; i<size; i++) {
            totalX+=mc[i].x;
            totalY+=mc[i].y;
        }
        
        Point2f massCenter(totalX/size, totalY/size); // condition: size != 0
        this->massCenter = massCenter;
        
        
//        Mat3b res;
//        cvtColor(src_gray, res, CV_GRAY2BGR);
//        
//        circle(res, massCenter, 2, Scalar(0,0,255));
//        
//        imshow("Result"+ to_string(ww++), res);
        //
        
        
        //    /// Get the moments
        //    long maxPos =-1;
        //    long max = -1;
        //
        //    vector<Moments> mu(contours.size() );
        //    for( int i = 0; i < contours.size(); i++ )
        //    {
        //
        //        if(mu[i].m00 > max){
        //            max = mu[i].m00;
        //            maxPos =i;
        //        }
        //    }
        //    if(max != -1)
        //        this->massCenter = Point2f( mu[maxPos].m10/mu[maxPos].m00 , mu[maxPos].m01/mu[maxPos].m00 );
        //    else
        //        this->massCenter = Point2f(-1,-1);
        //
        //    if(abs(this->massCenter.x -2147483648 )< 10|| abs(this->massCenter.y -2147483648)<10){
        //        imshow("srcGray" + to_string(ww++), src_gray);
        //        imshow("srcGray" + to_string(ww), canny_output);
        //
        //    }

    }
 }

double feature::massCenterX(){
    if(this->massCenter.x == -1 && this->massCenter.y == -1)
        feature::countMassCenter();
    
    if(!isnan(this->massCenter.x))
        return this->massCenter.x;
    else
        return 0;
}


double feature::massCenterY(){
    if(this->massCenter.x == -1 && this->massCenter.y == -1)
        feature::countMassCenter();
    
    if(!isnan(this->massCenter.y))
        return this->massCenter.y;
    else
        return 0;
}

