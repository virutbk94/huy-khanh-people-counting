#pragma once
#include <cv.h>
#include <vector>
#include <stack>
#include <time.h>
#include "Param.h"


class DelaunayTriangle
{	
	CvSize size;
	CvRect rect;
	CvMemStorage *storage;
	CvSubdiv2D *subdiv;
	
public:
	~DelaunayTriangle()
	{
		 cvReleaseMemStorage( &storage );
		 
	}
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
		int N= 0;
		double sum_Local_Dev=0, Global_Dev;

		cvStartReadSeq( (CvSeq*)(subdiv->edges), &reader, 0 );

		for( i = 0; i < total; i++ )
		{
			CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

			if( CV_IS_SET_ELEM( edge ))
			{
				//draw_subdiv_edge( img, (CvSubdiv2DEdge)edge + 1, voronoi_color );
				sum_Local_Dev+= Local_Dev((CvSubdiv2DEdge)(edge));
				draw_subdiv_edge( img, (CvSubdiv2DEdge)edge, delaunay_color );
				++N;
			}

			CV_NEXT_SEQ_ELEM( elem_size, reader );
			
		}
		N;
		Global_Dev = (1/N)*sum_Local_Dev;
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
	
	
};