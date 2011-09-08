
#pragma once
#include "cv.h"

#define NUMBER_FEATURE	180	
#define NUMBER_SLOT		4
#define NUMBER_PAIRS	6
#define MAX_GROUPS		(NUMBER_FEATURE / 3) * 2
#define MIN_GROUPS		MAX_GROUPS / 2
#define Pi				3.141592653

#define COLOR_RED		cvScalar(0,0,255)

typedef struct StrSlotCounting
{
	int numSlot, widthSlot, heightSlot;		

	CvRect slotLtRect[NUMBER_SLOT];
	CvRect slotRtRect[NUMBER_SLOT];

	CvRect belowRect[NUMBER_SLOT];
	CvRect aboveRect[NUMBER_SLOT];

	bool flagEnableLtSlot[NUMBER_SLOT];
	bool flagEnableRtSlot[NUMBER_SLOT];

	int waitTimeOutLtSlot[NUMBER_SLOT];
	int waitTimeOutRtSlot[NUMBER_SLOT];

	StrSlotCounting()
	{
		numSlot = 0;
		widthSlot = 0;
	}
	~StrSlotCounting()
	{
		numSlot = 0;
		widthSlot = 0;
	}


	void updateLtSlotStructure(int widthWindow, CvPoint root, int hSlot)
	{
		numSlot = NUMBER_SLOT;
		widthSlot = widthWindow / numSlot;
		heightSlot = hSlot;

		for (int i=0; i < numSlot; i++)
		{
			slotLtRect[i].x = root.x;
			slotLtRect[i].y = root.y;
			slotLtRect[i].width = widthSlot;
			slotLtRect[i].height = heightSlot;

			belowRect[i].x = slotLtRect[i].x;
			belowRect[i].y = slotLtRect[i].y + heightSlot;
			belowRect[i].width = widthSlot;
			belowRect[i].height = heightSlot;

			flagEnableLtSlot[i] = true;
			waitTimeOutLtSlot[i] = 0;
			root.x = root.x + widthSlot;
		}
	}

	void updateRtSlotStructure(int widthWindow, CvPoint root, int hSlot)
	{
		numSlot = NUMBER_SLOT;
		widthSlot = widthWindow / numSlot;
		heightSlot = hSlot;

		for (int i=0; i < numSlot; i++)
		{
			slotRtRect[i].x = root.x;
			slotRtRect[i].y = root.y;
			slotRtRect[i].width = widthSlot;
			slotRtRect[i].height = heightSlot;

			aboveRect[i].x = slotRtRect[i].x;
			aboveRect[i].y = slotRtRect[i].y - heightSlot;
			if (aboveRect[i].y < 0)
			{
				aboveRect[i].y = 0;
			}
			aboveRect[i].width = widthSlot;
			aboveRect[i].height = heightSlot;

			flagEnableRtSlot[i] = true;
			waitTimeOutRtSlot[i] = 0;
			root.x = root.x + widthSlot;
		}
	}

	void showLtSlotToFrame(IplImage* img)
	{
		for (int i=0; i < numSlot; i++)
		{
			if (flagEnableLtSlot[i] == true)
			{
				cvRectangleR(img, slotLtRect[i], COLOR_RED, 1, CV_AA);
			}
		}
	}

	void showRtSlotToFrame(IplImage* img)
	{
		for (int i=0; i < numSlot; i++)
		{
			if (flagEnableRtSlot[i] == true)
			{
				cvRectangleR(img, slotRtRect[i], COLOR_RED, 1, CV_AA);
			}
		}
	}

} SLOTCOUNTING;