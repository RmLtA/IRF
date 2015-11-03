//////////////////////////////////////////////////////////////////////////
// Module IRF, 5-iNFO
// Class computeImages
//////////////////////////////////////////////////////////////////////////

#include "computeImages.h"

bool computeImages::findTemplArea(Mat templ, string currentName)
{
	/// Create the result matrix
	int result_cols = this->sourceImg.cols - templ.cols + 1;
	int result_rows = this->sourceImg.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);

	//convert both images to GRAY
	Mat gref, gsrc;
    
	cvtColor(this->sourceImg, gref, CV_BGR2GRAY);
	cvtColor(templ, gsrc, CV_BGR2GRAY);

	/// Do the Matching and Normalize
	matchTemplate(gref, gsrc, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	
	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	
    
    // try to find best match
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    
    //checking if the found image is well located
    //TODO find better way...
    if(minLoc.x <350 && minLoc.x > 290 && maxLoc.x < 2050 && maxLoc.x >1500  ){
        try{
            
            //TODO see what this do...
            /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
            if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
            {
                matchLoc = minLoc;
            }
            else
            {
                matchLoc = maxLoc;
            }
            //drawing image from matchlocation point
            Mat temp_founded(this->sourceImg, Rect(matchLoc, Point(matchLoc.x + templ.cols , matchLoc.y + templ.rows)));
        
            Mat ligne(this->sourceImg, Rect(Point(matchLoc.x,matchLoc.y-templ.rows), Point(matchLoc.x +this->sourceImg.cols/1.2, matchLoc.y + 2*templ.rows)));
            this->templArea = ligne;
            return true;
            
        }catch(Exception e){
            std::cout << "" << e.msg <<endl ;
            return false;
        }
  
    }else{
        cout << "not in detected values...  "
        <<" minval : " << minLoc << " max val :: " << maxLoc << endl;
        return false;
    }
}

Mat computeImages::getTemplArea(){
    return this->templArea;
}



vector<Vec4i> computeImages::findLines(Mat imgSource){
    try{
        Mat img;
        //convert to grayscale
        
        //Apply blur to smooth edges and use adapative thresholding
        Size size(3,3);
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
        
        std::cout << "" << e.msg <<endl ;
        throw e;
        // return 0;
    }
    
}
    

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

      //  if we have more than 3 corners => it's a rectangle :)
        for(int i=0;i<corners.size();i++){
            cv::Point2f center(0,0);
            if(corners[i].size()>=3){
               
                for(int j=0;j<corners[i].size();j++){
                    center += corners[i][j];
                }
                center *= (1. / corners[i].size());
                sortCorners(corners[i], center);
                saved.push_back(std::vector<cv::Point2f>(corners[i]));
            }
        }
        sort(saved.begin(),saved.end(),rectComparator);

        
        vector<Mat> returnImages ;
        //process retransformation
        
        for(int i=0;i<saved.size();i++){
            if(saved[i].size()<4)continue;
            Rect r = boundingRect(saved[i]);
            if(r.area()<50000)continue;
            
            //removing borders...
            Rect rr(Point(r.tl().x +  10, r.tl().y +  10) ,Point(r.br().x-10,r.br().y-10));
            
            Mat res(imgSource, rr);
            returnImages.push_back(res);
        
        }
        return returnImages;
    }catch(Exception e){
        
        std::cout << "" << e.msg <<endl ; 
        throw e;
    }
  
}


//wrap perspective... no useful in our case
// Define the destination image
//            cv::Mat quad = cv::Mat::zeros(r.height, r.width, CV_8UC3);
//            // Corners of the destination image
//            std::vector<cv::Point2f> quad_pts;
//            quad_pts.push_back(cv::Point2f(0, 0));
//            quad_pts.push_back(cv::Point2f(quad.cols, 0));
//            quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));
//            quad_pts.push_back(cv::Point2f(0, quad.rows));
//            // Get transformation matrix
//            cv::Mat transmtx = cv::getPerspectiveTransform(saved[i], quad_pts);
//            // Apply perspective transformation
//            cv::warpPerspective(imgSource, quad, transmtx, quad.size());



Point2f computeImages::computeIntersect(cv::Vec4i a, cv::Vec4i b){
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
    // if not parallel
    if (float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4)))
    {
        Point2f pt;
        pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
        pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;
        if(pt.x<min(x1,x2)-10||pt.x>max(x1,x2)+10||pt.y<min(y1,y2)-10||pt.y>max(y1,y2)+10){
            return Point2f(-1,-1);
        }
        if(pt.x<min(x3,x4)-10||pt.x>max(x3,x4)+10||pt.y<min(y3,y4)-10||pt.y>max(y3,y4)+10){
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

void computeImages::sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center){
    std::vector<cv::Point2f> top, bot;
    for (int i = 0; i < corners.size(); i++)
    {
        if (corners[i].y < center.y)
            top.push_back(corners[i]);
        else
            bot.push_back(corners[i]);
    }
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
}
        


