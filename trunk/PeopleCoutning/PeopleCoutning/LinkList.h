
class LinkList
{
public:
	struct node
	{
		CvPoint data;
		bool match;
		//int pos;
		node *link;
	};
	node *current;
	node *head;
	node *tail;
	node *match;


	LinkList();
	void append( CvPoint num );
	//void add_as_first( CvPoint num );
	void addafter( int c, CvPoint num );
	void delNotMatchNode(  );
	void LinkList::resetMatchValue();
	//void display();
	int count();
	~LinkList();
};