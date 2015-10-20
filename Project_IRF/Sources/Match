
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/// Global Variables
Mat img; Mat templ; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;

/// Function Headers
void MatchingMethod(int, void*);

/**
* @function main
*/
int main(int, char** argv)
{
	/// Load image and template
	img = imread("00000.png");
	templ = imread("bomb.jpg");

	/// Create windows
	namedWindow(image_window, WINDOW_AUTOSIZE);
	namedWindow(result_window, WINDOW_AUTOSIZE);

	/// Create Trackbar
	const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);

	MatchingMethod(0, 0);

	waitKey(0);
	return 0;
}

/**
* @function MatchingMethod
* @brief Trackbar callback
*/
void MatchingMethod(int, void*)
{
	
	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);

	//convert both images to GRAY
	Mat gref, gsrc;
	cvtColor(img, gref, CV_BGR2GRAY);
	cvtColor(templ, gsrc, CV_BGR2GRAY);

	/// Do the Matching and Normalize
	matchTemplate(gref, gsrc, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	
	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	/// Show me what you got
	//rectangle(img, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	Mat temp_founded(img, Rect(matchLoc, Point(matchLoc.x + templ.cols , matchLoc.y + templ.rows)));
	imshow(result_window, temp_founded);
	
	////// reseize and show
	Mat imreduite;
	int reduction =5;
	Size tailleReduite(img.cols / reduction, img.rows / reduction);
	imreduite = Mat(tailleReduite, CV_8UC3); //cree une image à 3 canaux de profondeur 8 bits chacuns
	resize(img, imreduite, tailleReduite);
	///////
	imshow(image_window, imreduite);

	/// affichage des lignes ROI
	Mat ligne(img, Rect(Point(matchLoc.x,matchLoc.y-templ.rows), Point(matchLoc.x +img.cols/1.2, matchLoc.y + 2*templ.rows)));
	Size tailleReduite_l(ligne.cols / reduction, ligne.rows / reduction);
	resize(ligne, ligne, tailleReduite_l);
	imshow("ligne", ligne);

	return;
}
