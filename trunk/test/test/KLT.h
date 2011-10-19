#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
using namespace std;	

#define NUMBER_FEATURE 50
#define Pi 3.141592653
using namespace std;
class KLT
{
	int maxCorners;
	Mat src, src_gray;
	RNG rng;
	
public:
	vector<Point2f> corners;
	KLT ()
	{

	}
	~KLT()
	{

	}
	void initial( Mat srcInput )
	{
		src = srcInput;
		cvtColor( src, src_gray, CV_BGR2GRAY );
		maxCorners = NUMBER_FEATURE;
		rng(12345);
		goodFeatureToTrack();
	}
	void goodFeatureToTrack()
	{
		
		double qualityLevel = 0.01;
		double minDistance = 10;
		int blockSize = 3;
		bool useHarrisDetector = false;
		double k = 0.04;

		Mat copy;
		copy = src.clone();

		goodFeaturesToTrack( src_gray, 
			corners,
			maxCorners,
			qualityLevel,
			minDistance,
			Mat(),
			blockSize,
			useHarrisDetector,
			k );

		int r = 4;
		for( int i = 0; i < corners.size(); i++ )
		{ circle( copy, corners[i], r, Scalar(255,0,255), -1, 8, 0 ); }
		
		/// Show what you got
		namedWindow( "image", CV_WINDOW_AUTOSIZE );
		imshow( "feature", copy ); 
	}

};