#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"

using namespace cv;

#define Pi 3.141592653

#define GREEN Scalar(255,255,0)


class OPTICAL
{
	int maxCorners;
	Mat prevImgGray, nextImgGray;
	TermCriteria criteria;

public:
	vector<Point2f> feature;

	OPTICAL()
	{

	}
	~OPTICAL()
	{

	}

	
	void drawArrow(Mat image, CvPoint p1, CvPoint p2, CvScalar color)
	{
		double angle = atan2( (double) p1.y - p2.y, (double) p1.x - p2.x );
		double hypotenuse = sqrt(pow(p1.y - p2.y, 2.0) + pow(p1.x - p2.x, 2.0));

		p2.x = (int)(p1.x - 2 * hypotenuse * cos(angle));
		p2.y = (int)(p1.y - 2 * hypotenuse * sin(angle));
		line(image, p1, p2, color, 1, CV_AA, 0 );	

		p1.x = (int)(p2.x + 4 * cos(angle + Pi / 4));
		p1.y = (int)(p2.y + 4 * sin(angle + Pi / 4));
		line(image, p1, p2, color, 1, CV_AA, 0 );

		p1.x = (int)(p2.x + 4 * cos(angle - Pi / 4));
		p1.y = (int)(p2.y + 4 * sin(angle - Pi / 4));
		line(image, p1, p2, color, 1, CV_AA, 0 );
	}

	double distance(Point2f a, Point2f b)
	{
		double i;
		i = (a.x - b.x)*(a.x - b.x) + (a.y-b.y)*(a.y-b.y);
		return sqrt(i);
	}

	void opticalFlow(Mat prevImg, Mat nextImg, vector<Point2f> prevPts, vector<Point2f> nextPts)
	{
		

		TermCriteria criteria = TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 0.01);
		vector<uchar> status;
		vector<float> err;
		Size winsize(15,15);
		int maxLevel = 3;
		double derivLambda=0.5;
		int flags=0;
		double minEigThreshold = 0.0001;

		Mat copy = nextImg.clone();

		calcOpticalFlowPyrLK(prevImg, nextImg, prevPts, nextPts,status,err, winsize, maxLevel,criteria, derivLambda, flags);

		///reduce matched point by double check for some more condition
		for (int i=0; i < status.size(); i++)
		{
			if (status[i] != 0 && err[i] <= 350)	
			{
				Point2f pointFirst(prevPts[i]);
				Point2f pointSecond(nextPts[i]);

				if ((pointFirst.x <= prevImg.cols && pointFirst.y <= prevImg.rows && pointFirst.x >= 0  && pointFirst.y >= 0) && 
					(pointSecond.x <= prevImg.cols && pointSecond.y <= prevImg.rows && pointSecond.x >= 0  && pointSecond.y >= 0))
				
					{
						double dis = distance(pointFirst,pointSecond );

				if ( 1.5 <= dis && dis <=10)
				{
					feature.push_back(pointSecond); ///
					drawArrow(copy, pointFirst, pointSecond, GREEN);
					imshow("feature2", copy );
				}
				}
			}
		}
	}

	
};