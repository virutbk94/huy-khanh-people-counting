#define CV_NO_BACKWARD_COMPATIBILITY

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "KLT.h"
#include "delaunayTriangle.h"

using namespace cv;
#define FILE "Video 6.wmv"

Mat src;

int main()
{
	
	VideoCapture cap(FILE);
	if( !cap.isOpened() )
		return -1;

	KLT klt;
	DelaunayTriangle delaunay;
	
	for (;;)
	{
		cap >> src;
		klt.initial(src);
		delaunay.initialize(src, klt.corners);
		if (waitKey(30)>=0)
			break;
	}

	return 0;
}
