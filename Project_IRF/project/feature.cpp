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
    int GAP;
    if(isGlobal){
        resolution = 3;
        threshold = 30;
        minLinLength =10;
        maxLineGap = 3;
        GAP= 10;
    }else{
        //faire en fonction du splitted params
        resolution = 5;
        threshold = 30;
        minLinLength =10;
        maxLineGap = 10;
        GAP = 10;
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
          
            if(abs(l[0] - l[2]) < GAP)           /*check if line is | */
                vertical++;
            else if(abs(l[1] - l[3]) < GAP)      /*check if line is - */
                horizontal++;
            else if(l[1] < l[3])                 /*check if line is \ */
                diagonalNeg++;
            else if(l[1] > l[3])                 /*check if line is / */
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
    
    nbHoughLines = (int)lines.size();
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

int feature::nbHoughLinesResult(){
    if(houghLines[0] == -1)
        HoughLines();
    
    return nbHoughLines;
}



void feature::HoughCircles(){
    int dp;
    int minDist ;
      if(isGlobal){
        dp = 5;
        minDist = 3;
      
    }else{
        //faire en fonction du splitted params
        dp = 8;
        minDist = 1;
    }
    
    vector<Vec3f> circles;
    Mat dst, cdst;
    cdst = this->sourceImg.clone();
    GaussianBlur( this->graySourceImg, dst, Size(3, 3), 2, 2 );

    double posX =0 , posY = 0, radius = 0 ;
    cv::HoughCircles(dst, circles, HOUGH_GRADIENT,dp, minDist, 200, 50, 0, 0 );
    
    for( size_t i = 0; i < circles.size(); i++ )
    {
        posX += cvRound(circles[i][0]);
        posY += cvRound(circles[i][1]);
        radius += cvRound(circles[i][2]);
        // draw the circle center
        if(u.VERBOSE){
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            circle( cdst, center, 3, Scalar(0,255,0), -1, 8, 0 );
        }
    }
    if(circles.size())
    {
        houghCircles[0] =  ( posX /   (double) circles.size() );
        houghCircles[1] =  ( posY /   (double) circles.size() );
        houghCircles[2] =  ( radius / (double) circles.size() );
       
        houghCircles[0] = houghCircles[0]   / dst.cols *100;
        houghCircles[1] = houghCircles[1]   / dst.rows *100;
        houghCircles[2] = houghCircles[2]   / dst.rows *100;
    }else{
        houghCircles[0] = 0;houghCircles[1] = 0;houghCircles[2] = 0;
    }
    
    nbHoughCircles =(int) circles.size();

    if(u.VERBOSE){
        imshow("Hough Cicles", cdst);
        waitKey(1);
        cout << "Cicles :" << imgName << " : " <<  circles.size()<< endl;
    }
}

double feature::houghCirclesX(){
    if(houghCircles[0] == -1)HoughCircles();
    
    return houghCircles[0];
}
double feature::houghCirclesY(){
    if(houghCircles[0] == -1)HoughCircles();
    
    return houghCircles[1];
}

double feature::houghCirclesRadius(){
    if(houghCircles[0] == -1)HoughCircles();
    
    return houghCircles[2];
}
int feature::nbHoughCirclesResult(){
    if(houghCircles[0] == -1)HoughCircles();
    
    return nbHoughCircles;
}


void feature::Area(){
    
    //    //TODO
    //    //
    //    if(isGlobal){
    //        //params
    //    }else{
    //        //params
    //    }
    Mat src_gray;

    blur( this->graySourceImg, src_gray, Size(1,1) );
    Mat img, cdst;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    cdst = this->sourceImg.clone();
    /// Detect edges using canny
    Canny( src_gray, img, 0, 300, 3 );
    /// Find contours
    findContours( img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
 
    vector<Point> allcontours;
    
    for(int i = 0 ;i< contours.size() ; i++){
        
        for(int j= 0 ; j< contours[i].size() ; j++)
        {
            allcontours.push_back(contours[i][j]);
        }
    }
    vector<Point> approx;
    
  
    if(allcontours.size() >0){
       
        approxPolyDP(allcontours, approx, 1, true);
        boundRectangleArea =  boundingRect( approx );

        minEnclosingCircle(approx, centerArea, radiusArea);

        minEnclosingTriangle(approx, triangleArea);
        
        contourArea = cv::contourArea(approx);
        lengthContourArea = arcLength(allcontours, false);
        
        if(u.VERBOSE){
            double x =0, y =0;
            for(int i = 0 ; i< approx.size(); i++){
                circle( cdst, approx[i], 3, Scalar(0,255,0), -1, 4, 0 );
                
                x += approx[i].x;
                y += approx[i].y;
            }
            
            x /= approx.size();
            y /= approx.size();
            
            circle( cdst, Point(x,y) , 3, Scalar(255,0,0), -1, 8, 0 );
            
            rectangle(cdst,boundRectangleArea,Scalar(0,0,255));
          
            circle(cdst, centerArea,radiusArea,  Scalar(200,200,14) );
            circle(cdst, centerArea, 3,  Scalar(200,200,200), -1, 8, 0 );

            
            line(cdst, triangleArea[0],triangleArea[1], Scalar(100,100,100));
            line(cdst, triangleArea[1],triangleArea[2], Scalar(100,100,100));
            line(cdst, triangleArea[2],triangleArea[0], Scalar(100,100,100));

            imshow("Area", cdst);
            waitKey(1);
        }
    }
    else{
        contourArea = 0;
        lengthContourArea = 0;
        radiusArea = 0 ;
     

        
        centerArea = Point(0,0);
        
    }
}

//TODO contourArea
//TODO lenghtContourArea

Point2f feature::centerAreaRes(){
    if(radiusArea == -1)Area();
    centerArea.x = centerArea.x / this->sourceImg.cols * 100;
    centerArea.y = centerArea.y / this->sourceImg.rows * 100;
    return centerArea;
}
double feature::radiusAreaRes(){
    if(radiusArea == -1)Area();
    return radiusArea;
}
double feature::contoursArea(){
    if(radiusArea == -1)Area();
    return contourArea;
}
double feature::lengthContoursArea(){
    if(radiusArea == -1)Area();
    return lengthContourArea;
}
Point2f feature::triangleArea1(){
    if(radiusArea == -1)Area();
    triangleArea[0].x = triangleArea[0].x / this->sourceImg.cols * 100;
    triangleArea[0].y = triangleArea[0].y / this->sourceImg.rows * 100;
    return triangleArea[0];
}
Point2f feature::triangleArea2(){
    if(radiusArea == -1)Area();
    triangleArea[1].x = triangleArea[1].x / this->sourceImg.cols * 100;
    triangleArea[1].y = triangleArea[1].y / this->sourceImg.rows * 100;
    return triangleArea[0];
}
Point2f feature::triangleArea3(){
    if(radiusArea == -1)Area();
    triangleArea[1].x = triangleArea[1].x / this->sourceImg.cols * 100;
    triangleArea[1].y = triangleArea[1].y / this->sourceImg.rows * 100;
    return triangleArea[0];
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


vector<int> feature::getPixels(){
    vector<int> res;
    for(int i=0 ; i< this->graySourceImg.rows ; i++)
        for(int j=0 ; j<this->graySourceImg.cols ; j++)
            res.push_back(graySourceImg.at<uchar>(i,j));
    return res;
}
//
//
//Mat thresh_bernsen(Mat& gray,int ksize,int contrast_limit)
//{
//    
//    Mat ret = Mat::zeros(gray.size(),gray.type());
//    for(int i=0;i<gray.cols;i++ )
//    {
//        for(int j=0;j<gray.rows;j++ )
//        {
//            double mn=999,mx=0;
//            int ti=0,tj=0;
//            int tlx=i-ksize/2;
//            int tly=j-ksize/2;
//            int brx=i+ksize/2;
//            int bry=j+ksize/2;
//            if(tlx<0) tlx=0;
//            if(tly<0) tly=0;
//            if(brx>=gray.cols) brx=gray.cols-1;
//            if(bry>=gray.rows) bry=gray.rows-1;
//            
//            minMaxIdx(gray(Rect(Point(tlx,tly),Point(brx,bry))),&mn,&mx,0,0);
//            /* this does the above
//             for(int ik=-ksize/2;ik<=ksize/2;ik++)
//             {
//             for(int jk=-ksize/2;jk<=ksize/2;jk++)
//             {
//             ti=i+ik;
//             tj=j+jk;
//             if(ti>0 && ti<gray.cols && tj>0 && tj<gray.rows)
//             {
//             uchar pix = gray.at<uchar>(tj,ti);
//             if(pix<mn) mn=pix;
//             if(pix>mx) mx=pix;
//             }
//             }
//             }*/
//            int median = 0.5 * (mn+mx);
//            if(median<contrast_limit)
//            {
//                ret.at<uchar>(j,i)=0;
//            }else
//            {
//                uchar pix = gray.at<uchar>(j,i);
//                ret.at<uchar>(j,i) = pix>median?255:0;
//            }
//        }
//    }
//    return ret;
//}
//int main()
//{
//    Mat gray = imread("c:/data/number.jpg",0);
//    gray=255-gray;
//    Mat adaptthresh,bernsen;
//    bernsen=thresh_bernsen(gray,25,40);
//    imshow("gray",gray);
//    imshow("adaptthresh",adaptthresh);
//    imshow("bernsen",bernsen);
//    waitKey(0);
//}







//PRIVATE
void feature::countMassCenter(){
        //TODO
        //
    int thresh;
    if(isGlobal){
       thresh  = 140;

    }else{
        thresh = 100;
    }
    if(this->massCenter.x == -1 && this->massCenter.y == -1){
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


