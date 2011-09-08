#include "stdafx.h"
LinkList::LinkList()
{
	head=NULL;
	current = head;
	tail = head;
	match = NULL;
}

void LinkList::append(CvPoint num)
{


	if( head == NULL )
	{
		head = new node;
		head->data = num;
		head->match = true;
		//head->pos = 0;
		head->link = NULL;
		current = head;
	}
	else
	{
		current = head;
		while( current->link != NULL )
			current = current->link;

		current->link = new node;
		current = current->link;
		current->data = num;
		current->match = true;
		//t->pos = current->pos+1;
		current->link = NULL;
		tail = current;
	}
}


void LinkList::addafter( int c, CvPoint num)
{
	node *t;
	int i;
	for(i=0,current=head;i<c;i++)
	{
		current = current->link;
		if( current == NULL )
		{
			cout<<"\nThere are less than "<<c<<" elements.";
			return;
		}
	}

	t = new node;
	t->data = num;
	t->match = true;
	t->link = current->link;
	current->link = t;
}
void LinkList::resetMatchValue()
{
	node *r;
	r = head;
	if (head!=NULL)
	{

	
	while (r->link!=NULL)
	{
		r->match = false;
		r= r->link;
	}
	}
}
void LinkList::delNotMatchNode( )
{
	node *r;
	current = head;
	//if( current->data.x == num.x && current->data.y == num.y)
		if (current!=NULL)
		{

		
while (current->match == false)
	{
		
		head = current->link;
		delete current;
		current = head;
	}

	//r = current;
	while( current->link!=NULL )
	{
		
		//if( current->data.x == num.x && current->data.y == num.y )
		if (current->link->match == false)
		{
			r = current->link;
			current->link= current->link->link;
			delete r;
			if (current == NULL)
				break;
			
		}
		else
			current=current->link;
	}
		}
}

int LinkList::count()
{

	int c=0;
	for( current=head ; current != NULL ; current = current->link )
		c++;

	return c;
}

LinkList::~LinkList()
{

	if( head == NULL )
		return;

	while( head != NULL )
	{
		current = head->link;
		delete head;
		head = current;
	}
}