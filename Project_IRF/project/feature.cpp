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
int ww=0;
// Count the number of Harris Corner
int feature::countHarrisCorners(){
    int thresh = 150;
	Mat dst, dst_norm, dst_norm_scaled;
	dst = Mat::zeros(this->sourceImg.size(), CV_32FC1);

	/// Detector parameters
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.03;

	/// Detecting corners
	cornerHarris(this->graySourceImg, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

	/// Normalizing
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	//convertScaleAbs(dst_norm, dst_norm_scaled);
    Mat display = this->sourceImg.clone();
	int cornerHarris = 0;
	/// Drawing a circle around corners
	for (int j = 0; j < dst_norm.rows; j++)
	{
		for (int i = 0; i < dst_norm.cols; i++)
		{
			if ((int)dst_norm.at<float>(j, i) > thresh)
			{
				cornerHarris++;
                circle( display, Point( i, j ), 5,  Scalar(0), 2, 8, 0 );

			}
		}
	}
    if(u.VERBOSE){
        imshow("Harris corners " + to_string(ww++), display);
        cout << "Harris :"<< to_string(ww) << " : "<< cornerHarris << endl;
        waitKey(1);
    }

	return cornerHarris;
}



double feature::HoughLines(){
    int resolution = 1;
    int threshold = 30;
    int minLinLength =10;
    int maxLineGap = 10;
    vector<Vec4i> lines;
    
    Mat dst, cdst;
    Canny(this->sourceImg, dst, 50, 200, 3);
    cdst = dst.clone();
    HoughLinesP(dst, lines, resolution, CV_PI/180, threshold,minLinLength , maxLineGap );
    
    
    if(u.VERBOSE){
        for( size_t i = 0; i < lines.size(); i++ )
        {
            Vec4i l = lines[i];
            line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(180,8,255), 3, CV_AA);
            
        }
        
        imshow("Hough lines" + to_string(ww), cdst);
        cout << "Hough :" << to_string(ww++) << " : " <<  lines.size()<< endl;
        waitKey(1);
    }
    return lines.size();
}


double feature::countArea(){
    Mat src_gray;
    RNG rng(12345);
    int thresh = 100;
        
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


//PRIVATE
void feature::countMassCenter(){
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
        float totalX=0.0, totalY=0.0;
        for(int i=0; i<size; i++) {
            totalX+=mc[i].x;
            totalY+=mc[i].y;
        }
        
        Point2f massCenter(totalX/size, totalY/size); // condition: size != 0
        this->massCenter = massCenter;
    }
}


