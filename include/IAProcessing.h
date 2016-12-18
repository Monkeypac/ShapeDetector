////////////////////////////////////////////////////////////
// < Fichier : "IAProcessing.h"> 
// < Author : Yohan "HadesFufu" Masson >
////////////////////////////////////////////////////////////

#ifndef IAPROCESSING_H
#define IAPROCESSING_H
#include "Optimizer.h"
#include <algorithm>
#include <stack>
#include <functional>

extern point mapsize;

struct Node{
	static int nextId;

	Node(){
		id = nextId;
		nextId++;
	}

	inline bool operator==(const Node& a){
		if (id == a.id)
			return true;
		else
			return false;
	}

	inline bool operator!=(const Node& a){
		if (id != a.id)
			return true;
		else
			return false;
	}
	int id;
	std::vector<point> area;
};

struct Link{
	static int nextId;
	Link(){
		id = nextId;
		nextId++;
	}

	void copyFrom(Link& l)
	{
		id = l.id;
		beginPosition = l.beginPosition;
		endPosition = l.endPosition;
		left = l.left;
		startingNode = l.startingNode;
		endingNode = l.endingNode;
		type = l.type;
	}

	int			 id;
	point		 beginPosition = point(-1,-1);
	point		 endPosition = point(-1, -1);
	bool		 left;
	Node*		 startingNode;
	Node*		 endingNode;
	std::string  type;
};

/* Curves Function */
float test(float x);
float linear(float x, float p);
float jumpFunc(float x, float a, float maxJump);

class IAProcessing
{
public:

	IAProcessing(sf::Image* img);
	~IAProcessing();
	void process(std::vector<std::vector<point>>& AllShapes);
	std::vector<Node>& getNodes(){ return m_nodes; }
	
	void writeAll(std::string path);
	void writeAll(std::ofstream& s);
	void print();

private:

	//bool isPointInNode(Node& node, sf::Vector2f& pt);
	
	void determinateNodes(std::vector<std::vector<point>>& AllShapes);
		void clearNode1();
	void determinateLinks();
		void addLink(Node* begin, Node* end, point& beginPos, point& endPos, std::string type, bool left);
		void testDrop(Node& n);
		void testJump(Node& n);


	bool checkAddJump(Link& l);

	Node* detectOnLine(Node& nodeOfOrigin, point origin, std::function<float(float)> f, point& result, bool increasing = true);

	/* Writing */

	void write(Node&, std::ofstream& stream);
	void write(Link&, std::ofstream& stream);

	/* Copy Paste */
	point getPointOfCollision(point& a1, point& a2, point& b1, point& b2);
	float distance(point& a, point& b);
	bool is_between(point& first, point& second, point& checked);
	bool onSegment(point p, point q, point r);
	int  orientation(point p, point q, point r);
	bool doIntersect(point p1, point q1, point p2, point q2);

	sf::Image*		  m_outputImage;
	std::vector<Node> m_nodes;
	std::vector<Link> m_links;

	//Printing
	std::vector<point> m_BufferPrinter;
};


#endif 