#pragma once
#include <cv.h>
#include <vector>
#include <stack>
#include "Param.h"
#define NUMBER_FEATURE	500
#define NUMBER_SLOT		4
#define NUMBER_PAIRS	6
#define MAX_GROUPS		(NUMBER_FEATURE / 3) * 2
#define MIN_GROUPS		MAX_GROUPS / 2
#define Pi				3.141592653

#define COLOR_RED		cvScalar(0,0,255)

typedef struct StrEdge
{
	CvSubdiv2DEdge subEdge;
	CvPoint beginPoint, endPoint;
	int width, height;
	double length;

	StrEdge(CvSubdiv2DEdge tempEdge, CvRect rect)
	{
		subEdge = tempEdge;
		width = rect.width;
		height = rect.height;
		beginPoint = beginEdgePoint();
		endPoint = endEdgePoint();
		length = getEdgeLength();
	}

	~StrEdge()
	{

	}

	CvPoint beginEdgePoint()
	{
		CvSubdiv2DPoint* org_pt = cvSubdiv2DEdgeOrg(subEdge);
		return cvPointFrom32f(org_pt->pt);
	}

	CvPoint endEdgePoint()
	{
		CvSubdiv2DPoint* dst_pt = cvSubdiv2DEdgeDst(subEdge);
		return cvPointFrom32f(dst_pt->pt);
	}

	double getEdgeLength()
	{
		return sqrt(pow(beginPoint.x - endPoint.x, 2.0) + pow(beginPoint.y - endPoint.y, 2.0));
	}

	bool isValidEdge()
	{
		if ( isPointInRegion(beginPoint) && isPointInRegion(endPoint) )
		{
			return true;
		}
		return false;
	}

	bool isPointInRegion(CvPoint pt)
	{
		if ( (pt.x >= 0 && pt.x <= width) && (pt.y >=0 && pt.y <= height) )
		{
			return true;
		}
		return false;
	}

} EDGE;

typedef struct StrVertex
{
	// tag ~ index group and verify wherther to checked
	CvSubdiv2DPoint subdivPoint;
	CvPoint2D32f point32f;
	CvPoint point;
	int tag; 

	StrVertex()
	{
		tag = -1;
	}

	StrVertex(CvSubdiv2DPoint sdPoint)
	{
		subdivPoint = sdPoint;
		tag = -1;
		update();
	}

	~StrVertex()
	{

	}

	void update()
	{
		point32f = getPoint32f();
		point = getPoint();
	}

	CvPoint2D32f getPoint32f()
	{
		return subdivPoint.pt;
	}

	CvPoint getPoint()
	{
		return cvPointFrom32f(subdivPoint.pt);
	}

} VERTEX;

class DelaunayTriangle
{	
	CvSize size;
	CvRect rect;
	CvMemStorage *storage;
	CvSubdiv2D *subdiv;
	VERTEX arrVertex[NUMBER_FEATURE];
	vector<VERTEX> groupVertex[MAX_GROUPS];
	vector<EDGE> setEdge;

	
	
	CvScalar colorRect;

	
	double threshHoldEdge;
	int numPoint, countGroup, minimumFeatureBecomeGroup;

public: 
	vector<CvPoint> vecFilterFeature, vecFilterCenter;
public:
	
	void initialize(IplImage *image)
	{
		size = cvGetSize(image);
		rect = cvRect(0, 0, size.width, size.height);

		storage = cvCreateMemStorage(0);


		subdiv = cvCreateSubdiv2D( CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),
			sizeof(CvSubdiv2DPoint),
			sizeof(CvQuadEdge2D),
			storage );
		cvInitSubdivDelaunay2D( subdiv, rect );

	}

	void initualizeDelaunayGraph(CvSize size, CvScalar color, double thrEdge, int minFeature)
	{
		countGroup = 0;
		threshHoldEdge = thrEdge;
		colorRect = color;
		minimumFeatureBecomeGroup = minFeature;
		rect =  cvRect(0, 0, size.width, size.height);  
		storage = cvCreateMemStorage(0);
		subdiv = cvCreateSubdivDelaunay2D(rect, storage);
	}

	void insertPoint(CvPoint2D32f kltPoint[])
	{
		CvPoint2D32f fp;
		for (int i = 0; i < NUMBER_FEATURE; i++)
		{
			if (kltPoint[i].x>=0||kltPoint[i].y>=0)
			{
				fp = kltPoint[i];
				cvSubdivDelaunay2DInsert(subdiv, fp);

			}
		}

	}

	void draw_subdiv( IplImage* img, CvScalar delaunay_color, CvScalar voronoi_color )
	{
		CvSeqReader  reader;
		int i, total = subdiv->edges->total;
		int elem_size = subdiv->edges->elem_size;

		cvStartReadSeq( (CvSeq*)(subdiv->edges), &reader, 0 );

		for( i = 0; i < total; i++ )
		{
			CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

			if( CV_IS_SET_ELEM( edge ))
			{
				draw_subdiv_edge( img, (CvSubdiv2DEdge)edge + 1, voronoi_color );
				draw_subdiv_edge( img, (CvSubdiv2DEdge)edge, delaunay_color );
			}

			CV_NEXT_SEQ_ELEM( elem_size, reader );
		}
		//Global_Dev = (1/N)*sum_Local_Dev;
	}

	void draw_subdiv_edge( IplImage* img, CvSubdiv2DEdge edge, CvScalar color )
	{
		CvSubdiv2DPoint* org_pt;
		CvSubdiv2DPoint* dst_pt;
		CvPoint2D32f org;
		CvPoint2D32f dst;
		CvPoint iorg, idst;

		org_pt = cvSubdiv2DEdgeOrg(edge);
		dst_pt = cvSubdiv2DEdgeDst(edge);

		if( org_pt && dst_pt )
		{
			org = org_pt->pt;
			dst = dst_pt->pt;

			iorg = cvPoint( cvRound( org.x ), cvRound( org.y ));
			idst = cvPoint( cvRound( dst.x ), cvRound( dst.y ));

			cvLine( img, iorg, idst, color, 1, CV_AA, 0 );
		}
	}

	double Local_Dev(CvSubdiv2DEdge edge)
	{
		double mLocal_Mean_Length;
		int d;
		double e[100];
		double mLocal_Dev;
		double sum_in_Local_Dev; 
		CvSubdiv2DEdge temp;

		Local_Mean_Length(edge,mLocal_Mean_Length,d,e);

		for (int i = 0; i < d; ++i)
		{
			sum_in_Local_Dev =  mLocal_Mean_Length - fabs(e[i]);
		}

		mLocal_Dev = sqrt((1/d)*(sum_in_Local_Dev*sum_in_Local_Dev));

		return mLocal_Dev;
	}

	void Local_Mean_Length(CvSubdiv2DEdge edge, double &mLocal_Mean_Length, int &d, double e[])
	{
		CvSubdiv2DPoint* org_pt;
		CvSubdiv2DPoint* dst_pt;
		CvPoint2D32f org;
		CvPoint2D32f dst;
		CvPoint iorg, idst;
		double sum_e;
		CvSubdiv2DEdge temp;

		org_pt = cvSubdiv2DEdgeOrg(edge);
		//dst_pt = cvSubdiv2DEdgeDst(edge);
		d = 0;
		if( org_pt  )
		{
			org = org_pt->pt;
			iorg = cvPoint(  org.x ,  org.y );
		}

		temp = cvSubdiv2DNextEdge(edge);
		while (temp)
		{
			dst_pt = cvSubdiv2DEdgeDst(edge);
			if( dst_pt )
			{
				dst = dst_pt->pt;
				idst = cvPoint( dst.x , dst.y);
			}
			e[d] = Param::calDistance2Point(iorg, idst);
			d++;
			temp = cvSubdiv2DNextEdge(edge);
		}

		for (int i = 0; i < d; ++i)
		{
			sum_e +=fabs(e[i]);
		}

		mLocal_Mean_Length = (1/d)*sum_e;
	}

	stack<StrEdge> getSetRelativeEdgeOrigin(CvPoint pt)
	{
		stack<StrEdge> stackEdge;
		for (int i=0; i < setEdge.size(); i++)
		{
			EDGE tempEdge = setEdge[i];
			if (tempEdge.beginPoint.x == pt.x && tempEdge.beginPoint.y == pt.y)
			{
				stackEdge.push(tempEdge);
			}
		}
		return stackEdge;
	}
	stack<StrEdge> getSetRelativeEdgeDestinate(CvPoint pt)
	{
		//clock_t a = clock();

		stack<StrEdge> stackEdge;
		for (int i=0; i < setEdge.size(); i++)
		{
			EDGE tempEdge = setEdge[i];
			if (tempEdge.endPoint.x == pt.x && tempEdge.endPoint.y == pt.y)
			{
				stackEdge.push(tempEdge);
			}
		}
		return stackEdge;
	}
	
	CvRect getBoundingRect(vector<VERTEX> vecVertice)
	{
		CvRect rect;

		int xmax, ymax, xmin, ymin;
		xmin = xmax = vecVertice[0].point.x;
		ymin = ymax = vecVertice[0].point.y;

		int numPoint = vecVertice.size();
		for(int i=1; i < numPoint; i++)
		{
			if(xmin > vecVertice[i].point.x)
				xmin = vecVertice[i].point.x;

			if(xmax < vecVertice[i].point.x)
				xmax = vecVertice[i].point.x;

			if(ymin > vecVertice[i].point.y)
				ymin = vecVertice[i].point.y;

			if(ymax < vecVertice[i].point.y)
				ymax = vecVertice[i].point.y;		
		}

		rect.x = xmin;
		rect.y = ymin;
		rect.width  = xmax - xmin;
		rect.height = ymax - ymin;

		return rect;
	}
	void clearDelaunayData()
	{
		if (countGroup > 0)
		{
			for (int i=0; i <= countGroup; i++)
			{
				groupVertex[i].clear();
			}

			countGroup = 0;
			setEdge.clear();
			cvClearMemStorage(storage);
		}
	}

	void buildDelaunayGraph()
	{
		CvSeqReader reader;
		int total = this->subdiv->edges->total;
		int elem_size = this->subdiv->edges->elem_size;

		cvStartReadSeq( (CvSeq*)(subdiv->edges), &reader, 0 );

		// total edge include delaunay edge and voronoi edge
		for (int i = 0; i < total; i++)
		{
			// Quad-edge is a basic element of subdivision containing four edges 
			// (e, eRot, reversed e and reversed eRot):
			CvQuadEdge2D* ptEdge = (CvQuadEdge2D*)(reader.ptr);

			if (CV_IS_SET_ELEM(ptEdge))
			{
				CvSubdiv2DEdge edge2D = (CvSubdiv2DEdge) ptEdge;

				EDGE edge(edge2D, this->rect);

				if (edge.isValidEdge())
				{
					setEdge.push_back(edge);
				}
				//draw_subdiv_edge( image, edge2D, COLOR_BLUE );
			}

			CV_NEXT_SEQ_ELEM( elem_size, reader );

		}
	}
	int getIndexVertexFromPoint(CvPoint pt)
	{
		int index = -1;
		for (int i=0; i < numPoint; i++)
		{
			CvPoint tmpPoint= arrVertex[i].point;
			if (tmpPoint.x == pt.x && tmpPoint.y == pt.y)
			{
				index = i;
				break;
			}
		}
		return index;
	}
	void checkVertex(VERTEX &vertex)
	{
		if (vertex.tag == -1)
		{
			groupVertex[countGroup].push_back(vertex);
			vertex.tag = countGroup;
			stack<EDGE> stackEdgeOrigin = getSetRelativeEdgeOrigin(vertex.point);
			stack<EDGE> stackEdgeDestinate = getSetRelativeEdgeDestinate(vertex.point);	

			while (!stackEdgeOrigin.empty())
			{
				EDGE checkEdge = (EDGE) stackEdgeOrigin.top();
				stackEdgeOrigin.pop();
				if (checkEdge.length <= threshHoldEdge)
				{
					// lay diem cuoi do diem dau la diem dang check
					int index = getIndexVertexFromPoint(checkEdge.endPoint);
					checkVertex(arrVertex[index]);
				}
			}

			while (!stackEdgeDestinate.empty())
			{
				EDGE checkEdge = (EDGE) stackEdgeDestinate.top();
				stackEdgeDestinate.pop();
				if (checkEdge.length <= threshHoldEdge)
				{
					// lay diem dau do diem cuoi la diem dang check
					int index = getIndexVertexFromPoint(checkEdge.beginPoint);
					checkVertex(arrVertex[index]);
				}
			}
		}
	}

	void clusterVerticeToGroups()
	{
		for (int i=0; i < numPoint; i++)
		{
			if (arrVertex[i].tag == -1)
			{
				checkVertex(arrVertex[i]);
				countGroup++;
			}
		}
	}

	void updateDelaunayGraph(CvPoint2D32f pt32f[])
	{
		clearDelaunayData();
		subdiv = cvCreateSubdivDelaunay2D(rect, storage);
		CvPoint2D32f fp;
		
		// insert features to graph, each feature equal each vertex
		for (int i = 0; i < NUMBER_FEATURE ; i++)
		{
			fp = pt32f[i];
			CvSubdiv2DPoint* insertPoint;
			if (pt32f[i].x>=0||pt32f[i].y>=0)
			{
				fp = pt32f[i];
				 insertPoint = cvSubdivDelaunay2DInsert(subdiv, fp);

			}
			if (insertPoint != NULL)
			{
				VERTEX tempVertex(*insertPoint);
				arrVertex[i] = tempVertex;
			}
		}

		buildDelaunayGraph();
		clusterVerticeToGroups();
	}
	CvPoint getCenterPointInGroup(vector<VERTEX> setPoint)
	{
		double xAvg = 0;
		double yAvg = 0;
		int numPointInGroup = setPoint.size();
		for (int i=0; i < numPointInGroup; i++)
		{
			VERTEX vertex = setPoint[i];
			xAvg += vertex.point.x;
			yAvg += vertex.point.y;
		}

		CvPoint center = cvPoint(ceil(xAvg/numPointInGroup), ceil(yAvg/numPointInGroup));
		return center;
	}
	void findFilterCenterEachGroup()
	{
		vecFilterCenter.clear();
		vecFilterFeature.clear();
		for (int i=0; i < countGroup; i++)
		{
			if (groupVertex[i].size() >= minimumFeatureBecomeGroup)
			{
				CvPoint center = getCenterPointInGroup(groupVertex[i]);
				vecFilterCenter.push_back(center);
				int numFeature = groupVertex[i].size();
				for (int j=0; j < numFeature; j++)
				{
					CvPoint ptFeat = groupVertex[i].at(j).point;
					vecFilterFeature.push_back(ptFeat);
				}
			}
		}
	}
	DelaunayTriangle()
	{

	}
	~DelaunayTriangle()
	{
		cvReleaseMemStorage(&storage);
	}
};