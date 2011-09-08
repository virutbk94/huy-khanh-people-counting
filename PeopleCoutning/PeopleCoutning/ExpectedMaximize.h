
#pragma once
#include "cv.h"
#include "Param.h"
#include <vector>
#include "Vehicle.h"
template <typename T>

class ExpectedMaximize
{
	// variable & initialize
public:
	int iteration, numPoint, numCenter;
	double epsilon;
	double* arrMuy;
	double* arrSigma;
	double* arrHsPi;
	double** tableDistance;
	vector<CvPoint> vecPoint, vecCenter;
	vector<CvPoint> vecGroup[MIN_GROUPS]; 

public: 
	vector<VEHICLE> vecVehicle;

	ExpectedMaximize()
	{
		arrMuy = new double[MIN_GROUPS];
		arrSigma = new double[MIN_GROUPS];
		arrHsPi = new double[MIN_GROUPS];

		tableDistance= new double*[NUMBER_FEATURE];

		for (int i=0; i < NUMBER_FEATURE; i++)
		{
			tableDistance[i] = new double[MIN_GROUPS];
		}
	}
	~ExpectedMaximize()
	{

	}
	void initExpectedMaximization()
	{
		int numGroup = numCenter;

		double tempHsPi = 1.0 / numGroup;

		for (int i=0; i < numGroup; i++)
		{
			arrMuy[i] = 0; 
			arrSigma[i] = 1;  
			arrHsPi[i] = tempHsPi;
		}
	}

	void updateParam(int iTer, double iEps, vector<CvPoint> points, vector<CvPoint> centers)
	{
		iteration = iTer;
		epsilon = iEps;
		vecPoint = points;
		vecCenter = centers;
		numPoint = vecPoint.size();
		numCenter = vecCenter.size();
	}

	vector<CvPoint> getVecPoint()
	{
		return vecPoint;
	}

	// method to process data 	
private:
	double normalDistribution(double x, double muy, double sigma)
	{

		double coef = 1.0 / (sigma * sqrt(Pi * 2));

		double tu = (-1.0) * pow(x - muy, 2.0);

		double mau = 2 * pow(sigma, 2.0);

		double temp = tu / mau;

		double eponent = exp(temp);

		return coef * eponent;
	}

	double calProbScore(int n, int k, int side)
	{
		int numGroup = numCenter;
		double tu, mau, valDistance;

		valDistance = Param::calDistance2Point(vecPoint[n], vecCenter[k]) / side;
		tu = arrHsPi[k] * normalDistribution(valDistance, arrMuy[k], arrSigma[k]);

		mau = 0;
		for (int i=0; i < numGroup; i++)
		{
			valDistance = Param::calDistance2Point(vecPoint[n], vecCenter[i]) / side;
			mau += arrHsPi[i] * normalDistribution(valDistance, arrMuy[i], arrSigma[i]);
		}

		return (tu / mau);
	}

	double calExpLogNepe(int n, int side)
	{
		int numGroup = numCenter;

		double retVaLue = 0;

		for (int i=0; i < numGroup; i++)
		{
			double tempDistance = Param::calDistance2Point(vecPoint[n], vecCenter[i]) / side;
			retVaLue += arrHsPi[i] * normalDistribution(tempDistance, arrMuy[i], arrSigma[i]);
		}

		return retVaLue;
	}

	double logaritNepeLikelihood(int side)
	{ 
		double retVaLue = 0;

		for (int i=0; i < numPoint; i++)
		{
			double ExpLogNepe = calExpLogNepe(i, side);
			retVaLue += log(ExpLogNepe);
		}

		return retVaLue;
	}

	double calNk(int k)
	{
		double retVaLue = 0;

		for (int i=0; i < numPoint; i++)
		{
			retVaLue += tableDistance[i][k];
		}

		return retVaLue;
	}

	double calExpUk(int k, int side)
	{
		double retVaLue = 0;

		for (int i=0; i < numPoint; i++)
		{
			double tempDistance = Param::calDistance2Point(vecPoint[i], vecCenter[k]) / side;
			retVaLue += tableDistance[i][k] * tempDistance;
		}

		return retVaLue;
	}

	double calExpSigk(int k, int side)
	{
		double retVaLue = 0;

		for (int i=0; i < numPoint; i++)
		{
			double tempDistance = Param::calDistance2Point(vecPoint[i], vecCenter[k]) / side;
			retVaLue += tableDistance[i][k] * pow((tempDistance - arrMuy[k]), 2.0);
		}

		return retVaLue;
	}

	void updateParametterModel(int side)
	{
		int numGroup = numCenter;

		for (int i=0; i < numGroup; i++)
		{
			double Nk = calNk(i);
			double coef = 1 / Nk;

			double ExpUk = calExpUk(i, side);
			arrMuy[i] = coef * ExpUk;

			double ExpSigk = calExpSigk(i, side);
			arrSigma[i] = coef * ExpSigk;

			// model 1
			/*arrHsPi[i] = Nk / numPoint;*/

			// model 2
			double numPointInGroup = vecGroup[i].size();
			arrHsPi[i] =  numPointInGroup / numPoint;
		}
	}

	bool updateCenterPoint()
	{
		bool flag = false;

		for (int i=0; i < numCenter; i++)
		{
			double xAvg = 0;
			double yAvg = 0;
			int numPointInGroup = vecGroup[i].size();
			for (int j=0; j < numPointInGroup; j++)
			{
				xAvg += vecGroup[i].at(j).x;
				yAvg += vecGroup[i].at(j).y;
			}
			CvPoint oldPoint = vecCenter[i]; 
			CvPoint newPoint = cvPoint(ceil(xAvg/numPointInGroup), ceil(yAvg/numPointInGroup));
			if (oldPoint.x != newPoint.x || oldPoint.y != newPoint.y)
			{
				vecCenter[i] = newPoint;
				flag = true;
			}
		}
		return flag;
	}

	int findMaxIndex(double* arrVaue, int nValue)
	{
		int indMax = 0;
		double maxValue = arrVaue[0];
		for (int i=0; i < nValue; i++)
		{
			if (maxValue < arrVaue[i])
			{
				maxValue = arrVaue[i];
				indMax = i;
			}
		}
		return indMax;
	}

	void separateToGroup()
	{

		for (int i=0; i < numCenter; i++)
		{
			vecGroup[i].clear();
		}

		for (int i=0; i < numPoint; i++)
		{
			int indMax = findMaxIndex(tableDistance[i], numCenter);
			vecGroup[indMax].push_back(vecPoint[i]);
		}
	}

public: 
	void expectedMaximizationAlgorithm()
	{
		initExpectedMaximization();

		int numGroup = numCenter;
		int side = epsilon;

		for (int i=0; i < numPoint; i++)
		{
			for (int j=0; j < numGroup; j++)
			{
				tableDistance[i][j] = calProbScore(i, j, side);
			}
		}


		int nIter = 0;
		double pFirst, pLater;
		pFirst = logaritNepeLikelihood(side);
		pLater = -1;
		bool blUpdateCenter = true;
		while ((nIter < iteration) && (pFirst != pLater) && blUpdateCenter)
		{
			separateToGroup();
			blUpdateCenter = updateCenterPoint();
			updateParametterModel(side);
			pFirst = pLater;
			pLater = logaritNepeLikelihood(side);
			if (cvWaitKey(1) == 27) break;
			nIter++;
		}
	}

	void showAllGroup(IplImage* img)
	{
		CvRect rect;
		for (int i=0; i < numCenter; i++)
		{
			int numPointInGroup = vecGroup[i].size();
			if (numPointInGroup > 3)
			{
				rect = Param::getBoundingRect(vecGroup[i]);
				if (rect.width >= 25 && rect.height >= 40)
				{
					cvRectangle(img, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), COLOR_GREEN, 1);
					cvCircle( img, vecCenter[i], 4, COLOR_BLUE, CV_FILLED );
				}
			}
		}
	}

	void filterVehicleLtFlow()
	{
		CvRect rect;
		vecVehicle.clear();
		for (int i=0; i < numCenter; i++)
		{
			int numPointInGroup = vecGroup[i].size();
			if (numPointInGroup > 3)
			{
				rect = Param::getBoundingRect(vecGroup[i]);
				if (rect.width >= 25 && rect.height >= 40)
				{
					// khac voi RtFlow
					VEHICLE vehicle(rect, vecCenter[i]);
					vecVehicle.push_back(vehicle);
				}
			}
		}
	}

	void filterVehicleRtFlow()
	{
		CvRect rect;
		vecVehicle.clear();
		for (int i=0; i < numCenter; i++)
		{
			int numPointInGroup = vecGroup[i].size();
			if (numPointInGroup > 3)
			{
				rect = Param::getBoundingRect(vecGroup[i]);
				if (rect.width >= 25 && rect.height >= 40)
				{
					// khac voi LtFlow
					CvPoint tmpPoint; // vecCenter[i]
					tmpPoint.x = rect.x + rect.width / 2;
					tmpPoint.y = rect.y + rect.height / 2;
					// vecCenter[i]
					VEHICLE vehicle(rect, tmpPoint);
					vecVehicle.push_back(vehicle);
				}
			}
		}
	}
};