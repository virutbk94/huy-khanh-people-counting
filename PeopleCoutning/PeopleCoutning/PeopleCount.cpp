// PeopleCount.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


#define FILE "Video 6.wmv"

int _tmain(int argc, _TCHAR* argv[])
{
	

	CvCapture *capture = cvCaptureFromFile(FILE);
	if (capture == NULL)
	{
		return 0;
	}

	IplImage *image;
	image = cvQueryFrame(capture);
	CvSize size;
	size = cvGetSize(image);

	Container container(image);
	
	double fps = cvGetCaptureProperty(
		capture,
		CV_CAP_PROP_FPS
		);
	/*CvVideoWriter *writer = cvCreateVideoWriter(
		"out.avi",
		-1,
		fps,
		cvGetSize(image)
		);
		*/
	int depth = 8;
	int channels = 1;
	CvFont font;
	cvInitFont(&font,CV_FONT_ITALIC, 0.5,0.5,0,1,8);

	IplImage *background = cvCreateImage(size, depth, channels);
	cvCvtColor(image, background, CV_RGB2GRAY);
	
	IplImage *foreground = cvCreateImage(size, depth, channels);
	
	int isInit = 0;
	//for (int i = 0; i < 150; ++i)
		//cvQueryFrame(capture);
	int i = 0;
	char count[10];
	while ((image = cvQueryFrame(capture))!=NULL)
	{
		++i;
		itoa(i,count,10);
		//cvPutText(image, count, cvPoint(100, 100),&font , cvScalar(255,255,255));
		cvShowImage("image",image);
		//cvCvtColor(image, foreground, CV_RGB2GRAY);
		//BackgroundSubtract(foreground, background, foreground);
		
		//cvShowImage("image2",foreground);
		
		//cvWriteFrame(writer, foreground);
		
		/*if (isInit == 0)
		{
			container.initHist(image);
			isInit = 1;
		}
		container.getHist(image);
		container.matchPeople(image);
		*/
		//container.initPoint();
		//container.Process(foreground, image);
		
		cvShowImage("Contour", container.destinyImage);
		//cvWriteFrame(writer, container.destinyImage);
		//cvZero(container.destinyImage);
		
		if (cvWaitKey(0) == 27 )
		{
			//system("PAUSE");
			//getchar();
			//system("CONTINUE");
		}
	}
//	 cvReleaseVideoWriter(&writer);
	cvReleaseImage( &image);
	cvReleaseCapture(&capture);
	cvDestroyAllWindows();
	
	return 0;
}

