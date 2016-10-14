////////////////////////////////////////////////////////////
// < Fichier : "IAProcessing.h"> 
// < Author : Yohan "HadesFufu" Masson >
////////////////////////////////////////////////////////////

#ifndef IAPROCESSING_H
#define IAPROCESSING_H
#include "Optimizer.h"
#include <algorithm>
#include <stack>

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
	int id;
	std::vector<point> area;
};

struct Link{
	static int nextId;
	Link(){
		id = nextId;
		nextId++;
	}
	int			 id;
	point		 beginPosition = point(-1,-1);
	point		 endPosition = point(-1, -1);
	bool		 left;
	Node*		 startingNode;
	Node*		 endingNode;
	std::string  type;
};

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

	float distance(point& a, point& b);
	bool is_between(point& first, point& second, point& checked);

	bool onSegment(point p, point q, point r);
	int  orientation(point p, point q, point r);
	bool doIntersect(point p1, point q1, point p2, point q2);

	
	void write(Node&, std::ofstream& stream);
	void write(Link&, std::ofstream& stream);
	void link();
	void clearNode1();

	void determineFallingPoint(point& current, point& result, bool left);
	void determineFallingLimit(point& origin, point& result, bool left);
	void determineFallingData(Node& n, point& origin, point& limit, point& corner, bool left);

	void determineJumpingLimit(point& current, point& result, bool left);
	void determineJumpingData(point& origin, point& limit, point& corner1, point& corner2, bool left);

	float sign(point& p1, point& p2, point& p3);
	bool  PointInTriangle(point& pt, point& v1, point& v2, point& v3);
	bool  PointInCarre(point& pt, point& v1, point& v2);
	point getPointOfCollision(point& a1, point& a2, point& b1, point& b2);

	bool checkAddJump(Link& l);

	sf::Image*		  m_outputImage;
	std::vector<Node> m_nodes;
	std::vector<Link> m_links;
};


#endif 