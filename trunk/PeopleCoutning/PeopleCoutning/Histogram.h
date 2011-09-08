#pragma once

#include <cv.h>
#include <highgui.h>

class CHist
{
	IplImage* hsv;
	IplImage* h_plane;
	IplImage* s_plane;
	IplImage* v_plane;
public:
	CHist(CvSize size);
public:
	~CHist(void);
public:
	CvHistogram* computeHist(IplImage* image, CvRect rect);
public:
	double distance(CvHistogram* histA, CvHistogram* histB);
public:
	void initHSVImage(IplImage* image);
};


CHist::CHist(CvSize size)
{
	
}

CHist::~CHist(void)
{
	if (h_plane != NULL)
	{
		cvReleaseImage(&h_plane);
	}
	if (s_plane != NULL)
	{
		cvReleaseImage(&s_plane);
	}
	if (v_plane != NULL)
	{
		cvReleaseImage(&v_plane);
	}
	if (hsv != NULL)
	{
		cvReleaseImage(&hsv);
	}
}

CvHistogram* CHist::computeHist(IplImage* image, CvRect rect)
{
	cvSetImageROI(h_plane, rect);
	cvSetImageROI(s_plane, rect);
	cvSetImageROI(v_plane, rect);
	cvSetImageROI(hsv, rect);
	cvSetImageROI(image, rect);
	IplImage* planes[] = {h_plane, s_plane, v_plane};
	int h_bins, s_bins, v_bins;
	h_bins = s_bins = v_bins = 8;
	CvHistogram* hist = NULL;
	{
		int hist_size[] = {h_bins, s_bins, v_bins};
		float h_ranges[] = {0, 180};
		float s_ranges[] = {0, 255};
		float v_ranges[] = {0, 255};
		float* ranges[] = {h_ranges, s_ranges, v_ranges};
		hist = cvCreateHist(3, hist_size, CV_HIST_ARRAY, ranges, 1);
	}
	cvCalcHist(planes, hist, 0, 0);
	cvNormalizeHist(hist, 1.0);
	cvResetImageROI(h_plane);
	cvResetImageROI(s_plane);
	cvResetImageROI(v_plane);
	cvResetImageROI(hsv);
	cvResetImageROI(image);
	return hist;
}


double CHist::distance(CvHistogram* histA, CvHistogram* histB)
{
	return cvCompareHist(histA, histB, CV_COMP_BHATTACHARYYA);
}

void CHist::initHSVImage(IplImage* image)
{
	CvSize size = cvGetSize(image); 
	hsv = cvCreateImage(size, 8, 3);
	h_plane = cvCreateImage(size, 8, 1);
	s_plane = cvCreateImage(size, 8, 1);
	v_plane = cvCreateImage(size, 8, 1);
	cvCopyImage(image, hsv);
	//cvCvtColor(image, hsv, CV_BGR2HSV);
	cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, 0);
}
