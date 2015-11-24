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


Point2f feature::countMassCenter(){
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
    
    
    /// Get the moments
    double maxPos =0;
    double max = 0;
    
    vector<Moments> mu(contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    {
        mu[i] = moments( contours[i], false );
        if(mu[i].m00 > max){
            max = mu[i].m00;
            maxPos =i;
        }
    }
    
    
    
    return Point2f( mu[maxPos].m10/mu[maxPos].m00 , mu[maxPos].m01/mu[maxPos].m00 );

}


