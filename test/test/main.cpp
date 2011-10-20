#define CV_NO_BACKWARD_COMPATIBILITY

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "KLT.h"
#include "delaunayTriangle.h"
#include "optical.h"

using namespace cv;
#define FILE "Video 6.wmv"

Mat src, srcPreserve;
vector<Point2f> pointPreserve;

int main()
{
	
	VideoCapture cap(FILE);
	if( !cap.isOpened() )
		return -1;

	KLT klt;
	DELAUNAY delaunay;
	OPTICAL optical;
	cap >> src;

	for (;;)
	{
		srcPreserve = src.clone();
		cap >> src;
		klt.initial(src);
		pointPreserve = klt.corners;
		optical.opticalFlow(srcPreserve,src,pointPreserve,klt.corners);
		delaunay.initialize(src, optical.feature);
		optical.feature.clear();
		if (waitKey(30)>=0)
			break;
			
	}
	
	return 0;
}
