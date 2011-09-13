#pragma  once
#include <cv.h>

class BackgroundSubtract
{
private:
	IplConvKernel *morph;
	

public:
	BackgroundSubtract(IplImage *image, IplImage *background, IplImage *foreground);
	~BackgroundSubtract(void);
};


BackgroundSubtract::BackgroundSubtract(IplImage *image, IplImage *background, IplImage *foreground)
{
	morph = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT);
	cvAbsDiff(image, background, foreground);
	cvThreshold(foreground, foreground, 40 ,255, CV_THRESH_BINARY);
	
	cvErode(foreground, foreground, morph, 3);
	cvDilate(foreground, foreground, morph, 10);
}
BackgroundSubtract::~BackgroundSubtract(void)
{
	cvReleaseStructuringElement(&morph);

}