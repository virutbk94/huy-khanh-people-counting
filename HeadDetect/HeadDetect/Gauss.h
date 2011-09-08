#include "cv.h"
#include "highgui.h"
#include <fstream>
using namespace std;

//#define PI 3.14

#pragma once

class Gauss
{
public:	
	double InvCov[2][2];

	double sub[3];
	double rsl[3];
	double b;

	double phi;
	double logOfSqrt2PiVariance;
	double threshold;

	CvMat* mean;
	CvMat* corrvariant;

	void TrainData(char *prefix, char *suffix, int number_images, int start_index, int end_index, char* fileOutput);
	void LoadData(char* fileOutput);
	//IplImage* Classify(IplImage *imgColor, IplImage *mask);
	IplImage* Classify(IplImage *imgColor,  float thresh);
	IplImage* Classify(IplImage *imgColor);
	void SetThreshold(double threshold);

	CvScalar getPixalValue(IplImage* img, int x, int y, int channels)
	{
		CvScalar rtValue;
		uchar* ptrSrc = (uchar*) (img->imageData + y * img->widthStep + x * channels);
		for (int i=0; i < channels; i++)
		{
			rtValue.val[i] = ptrSrc[i];
		}
		return rtValue;
	}

	Gauss(void);
	~Gauss(void);
};