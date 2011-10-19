#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;


class DELAUNAY
{	
public:
	DELAUNAY()
	{

	}
	~DELAUNAY()
	{

	}
	void initialize(Mat src, vector<Point2f> fp)
	{
		Scalar delaunay_color(255,255,255);
		
		Rect rect(Point(0,0),src.size());
		Subdiv2D subdiv(rect);
		for (int i = 0; i <fp.size();i++)
		subdiv.insert(fp[i]);
		Mat copy = src.clone();
		draw_subdiv( copy, subdiv, delaunay_color );
		//copy.empty();
	}
	static void draw_subdiv( Mat img, Subdiv2D subdiv, Scalar delaunay_color )
	{
		vector<Vec4f> edgeList;
		subdiv.getEdgeList(edgeList);
		for( size_t i = 0; i < edgeList.size(); i++ )
		{
			Vec4f e = edgeList[i];
			Point pt0 = Point(cvRound(e[0]), cvRound(e[1]));
			Point pt1 = Point(cvRound(e[2]), cvRound(e[3]));
			line(img, pt0, pt1, delaunay_color, 1, CV_AA, 0);
		}
		imshow( "delaunay", img );

	}

	

	

};

