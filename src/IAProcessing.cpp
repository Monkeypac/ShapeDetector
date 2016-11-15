#include "IAProcessing.h"

int			Node::nextId = 0;
int			Link::nextId = 0;

IAProcessing::IAProcessing(sf::Image* img) : m_outputImage(img){
}

IAProcessing::~IAProcessing(){

}

void IAProcessing::process(std::vector<std::vector<point>>& AllShapes){
	int i2, n;
	bool okPole, okSegment, creating, differentShapes;
	std::vector<point>* shape = nullptr;
	point limit, limitbefore, delta;
	limit.solid = false;

	for (int i = 0; i < AllShapes.size(); ++i){
		shape = &AllShapes[i];
		n = shape->size();
		creating = false;

		for (int j = 0; j < n; ++j){
			point& current = shape->at(j);
			limit.x = current.x;
			limit.y = current.y - 50;
			okPole = true; okSegment = true;
			if (creating){
				delta.x = limit.x - limitbefore.x;
				delta.y = limit.y - limitbefore.y;

				delta.x = fabs(delta.x);
				delta.y = fabs(delta.y);

				if (delta.y != 0){
					float angle = delta.x / delta.y;
					if (angle < 0.1){
						okSegment = false;
						Log::debug("Shape Cut") << angle << delta.x << delta.y << "||" << limit.x << limit.y << limitbefore.x << limitbefore.y;
					}
				}
			}
			for (int l = 0; l < AllShapes.size() && (okSegment || okPole); ++l){
				for (int k = 0; k < AllShapes[l].size() && (okSegment || okPole); ++k){
					i2 = (k < AllShapes[l].size() - 1) ? k + 1 : k + 1 - AllShapes[l].size();
					point pointBegin = AllShapes[l][k];
					point pointEnd = AllShapes[l][i2];

					differentShapes = !(pointBegin == current || pointEnd == current);
					
					if (doIntersect(current, limit, pointBegin, pointEnd) && differentShapes){
						okPole = false;
						Log::debug("Shape Cut") << "Hit poteau" << current.x << current.y << "||" << pointBegin.x << pointBegin.y << pointEnd.x << pointEnd.y;
					}
					
					else if (doIntersect(limitbefore, limit, pointBegin, pointEnd)){
						okSegment = false;
						Log::debug("Shape Cut") << "Hit corde" << current.x << current.y << "||" << pointBegin.x << pointBegin.y << pointEnd.x << pointEnd.y;
					}
				}
			}
			if (okSegment && okPole && creating){
				m_nodes.back().area.push_back(current);
			}

			if (!okSegment || !okPole){
				creating = false;
			}

			if (okPole && !creating){
				m_nodes.emplace_back();
				Log::debug("Shape Create") << m_nodes.back().id << j << n;
				m_nodes.back().area.push_back(current);
				creating = true;
			}
			limitbefore = limit;
		}
	}
	clearNode1();
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
		Log::debug("Display") << "id:" << it->id;
	Log::debug() << "Done";
	detectOnLine(m_nodes[2], sf::Vector2f(0, 500), &test);
	Log::debug() << "re Done";
	//link();
}

float IAProcessing::distance(point& a, point& b){
	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y) * (a.x - b.x));
}
bool IAProcessing::is_between(point& first, point& second, point& checked){
	return (distance(first, checked) + distance(checked, second) == distance(first, second));
}

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool IAProcessing::onSegment(point p, point q, point r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int IAProcessing::orientation(point p, point q, point r)
{
	// See http://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear

	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool IAProcessing::doIntersect(point p1, point q1, point p2, point q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}

void IAProcessing::writeAll(std::string path){
	std::ofstream s;
	s.open(path, std::fstream::app);
	s << "<IA>" << std::endl;
	for (int i = 0; i < m_nodes.size(); ++i)
		write(m_nodes[i], s);
	for (int i = 0; i < m_links.size(); ++i)
		write(m_links[i], s);
	s << "</IA>" << std::endl;
	s.close();
}

void IAProcessing::writeAll(std::ofstream& s){
	s << "<IA>" << std::endl;
	for (int i = 0; i < m_nodes.size(); ++i)
		write(m_nodes[i], s);
	for (int i = 0; i < m_links.size(); ++i)
		write(m_links[i], s);
	s << "</IA>" << std::endl;
}

void IAProcessing::write(Node& n, std::ofstream& stream){
	stream << "<node id=\"" << n.id << "\">" << std::endl;
	for (point c : n.area) {
		stream << "\t<point x=\"" << c.x << "\" y=\"" << c.y << "\" />" << std::endl;
	}
	stream << "</node>" << std::endl;
}

void IAProcessing::write(Link& l, std::ofstream& stream){
	if (l.left)
		stream << "<link id=\"" << l.id << "\" Firstid=\"" << l.startingNode->id << "\" Secondid = \"" << l.endingNode->id << "\" type=\"" << l.type << "\" direction = \"left\" >" << std::endl;
	else
		stream << "<link id=\"" << l.id << "\" Firstid=\"" << l.startingNode->id << "\" Secondid = \"" << l.endingNode->id << "\" type=\"" << l.type << "\" direction = \"right\" >" << std::endl;
	stream << "\t<BeginPosition x=\"" << l.beginPosition.x << "\" y=\"" << l.beginPosition.y << "\"/>" << std::endl;
	stream << "\t<EndPosition x=\"" << l.endPosition.x << "\" y=\"" << l.endPosition.y << "\"/>" << std::endl;
	stream << "</link>" << std::endl;

}

void IAProcessing::print(){
	int i = 0;
	for (Node n : m_nodes) {
		Correct(n.area);
		for (auto it = n.area.begin(); it != n.area.end(); ++it){
			m_outputImage->setPixel(it->x, it->y, sf::Color::Red);
			i++;
		}
	}
	std::vector<point> vec;

	for (auto it = m_links.begin(); it != m_links.end(); ++it){
		//if (it->id == 54)
			Log::debug() << it->id << it->left << it->beginPosition.x << it->beginPosition.y << it->endPosition.x << it->endPosition.y << it->type << it->startingNode->id << it->endingNode->id;
		if (!it->beginPosition || !it->endPosition){
			Log::debug() << "jumped !";
			continue;
		}
		vec.push_back(it->beginPosition);
		vec.push_back(it->endPosition);
		Correct(vec);
		for (auto it2 = vec.begin(); it2 != vec.end(); ++it2){
			if (it->type == "fall")
				m_outputImage->setPixel(it2->x, it2->y, sf::Color::Cyan);
			else
				m_outputImage->setPixel(it2->x, it2->y, sf::Color(255, 168, 0));
		}
		vec.clear();
		Log::debug() << "done !";
	}
}

void IAProcessing::link(){
	Log::debug() << "Link Begin";
	int i2;
	point falling_origin, falling_limit, falling_corner,
		jumping_goal, jumping_limit, jumping_corner1, jumping_corner2;

	bool left = true;
	for (int o = 0; o < 2; o++){
		for (Node& n : m_nodes){
			//Needed data for falling calculation
			determineFallingData(n, falling_origin, falling_limit, falling_corner, left);

			//Needed data for Jumping calculation
			jumping_goal = falling_origin;
			determineJumpingData(jumping_goal, jumping_limit, jumping_corner1, jumping_corner2, left);
			//Log::debug("IA-Link") << falling_origin.x << falling_origin.y << falling_limit.x << falling_limit.y << falling_corner.x << falling_corner.y;
			for (Node& m : m_nodes){
				if (n == m){
				//	Log::debug() << "pass" << n.id << m.id;
					continue;
				}
				else
				//	Log::debug() << "No Pass" << n.id << m.id;
				for (int i = 1; i < m.area.size(); ++i){
					i2 = (i < m.area.size() - 1) ? i + 1 : i + 1 - m.area.size();
					if (!m.area[i].solid || !m.area[i2].solid)
						continue;
					//Log::debug() << "Checking" << m.area[i].x << m.area[i].y;
					//Log::debug("IA-Link Intersect") << m.area[i].x << m.area[i].y << m.area[i2].x << m.area[i2].y;
					//Falling
					if (PointInTriangle(m.area[i], falling_origin, falling_corner, falling_limit)){
						Link newLink;
						newLink.beginPosition = falling_origin;
						newLink.endPosition = m.area[i];
						newLink.type = "fall";
						newLink.startingNode = &n;
						newLink.endingNode = &m;
						newLink.left = left;
						m_links.push_back(newLink);
					}
					else if (doIntersect(falling_origin, falling_limit, m.area[i], m.area[i2]) || doIntersect(falling_origin, falling_corner, m.area[i], m.area[i2])){
						Link newLink;
						newLink.beginPosition = falling_origin;
						newLink.type = "fall";
						newLink.startingNode = &n;
						newLink.endingNode = &m;
						m_links.push_back(newLink);
						newLink.left = left;
					}

					//Jumping
					if (PointInCarre(m.area[i], jumping_goal, jumping_limit)){
						Log::debug() << "In Carre";
						Link newLink;
						newLink.beginPosition = m.area[i];
						newLink.endPosition = jumping_goal;
						newLink.type = "jump";
						newLink.startingNode = &m;
						newLink.endingNode = &n;
						newLink.left = left;
						if (checkAddJump(newLink))
							m_links.push_back(newLink);
					}
					else if (doIntersect(jumping_goal, jumping_corner2, m.area[i], m.area[i2]) || doIntersect(jumping_corner1, jumping_limit, m.area[i], m.area[i2])){
						Log::debug() << "Cross Border";
						Link newLink;
						newLink.endPosition = jumping_goal;
						newLink.beginPosition = getPointOfCollision(jumping_corner1, jumping_limit, m.area[i], m.area[i2]);
						if (!newLink.beginPosition)
							newLink.beginPosition = getPointOfCollision(jumping_goal, jumping_corner2, m.area[i], m.area[i2]);
						newLink.type = "jump";
						newLink.startingNode = &m;
						newLink.endingNode = &n;
						newLink.left = left;
						if (checkAddJump(newLink))
							m_links.push_back(newLink);
					}
				}
			}
		}
		left = false;
	}
}


void IAProcessing::clearNode1(){
	for (auto it = m_nodes.begin(); it != m_nodes.end();){
		if (it->area.size() < 2){
			it = m_nodes.erase(it);
		}
		else
			++it;
	}
}

void IAProcessing::determineFallingPoint(point& current, point& result, bool left){
	if (result.x == -42 || current.x == result.x && current.y > result.y){
		result = current;
		return;
	}
	if (left){
		if (current.x < result.x)
			result = current;
	}
	else{
		if (current.x > result.x)
			result = current;
	}
}

void IAProcessing::determineFallingLimit(point& origin, point& result, bool left){
	int y = 1;
	int x;
	if (left)
		for (x = origin.x; x > 0 && y < mapsize.y && x < mapsize.x && y > 0; x--)
			y = origin.y + (origin.x - x);
	else
		for (x = origin.x; x > 0 && y < mapsize.y && x < mapsize.x && y > 0; x++)
			y = origin.y + (x - origin.x);
	result.x = x;
	result.y = y;
}

void IAProcessing::determineFallingData(Node& n, point& origin, point& limit, point& corner, bool left){
	origin.x = -42;
	for (int i = 0; i < n.area.size(); ++i){
		determineFallingPoint(n.area[i], origin, left);
	}
	determineFallingLimit(origin, limit, left);
	corner.x = origin.x;
	corner.y = limit.y;
}


void IAProcessing::determineJumpingData(point& origin, point& limit, point& corner1, point& corner2, bool left){
	determineJumpingLimit(origin, limit, left);
	corner1.x = limit.x;
	corner1.y = origin.y;

	corner2.x = origin.x;
	corner2.y = limit.y;
}

void IAProcessing::determineJumpingLimit(point& current, point& result, bool left){
	if (left)
		result.x = current.x - 300;
	else
		result.x = current.x + 300;
	result.y = current.y + 300;
	if (result.x > mapsize.x)
		result.x = mapsize.x;
	else if (result.x < 0)
		result.x = 0;

	if (result.y > mapsize.y)
		result.y = mapsize.y;
	else if (result.y < 0)
		result.y = 0;
}

point IAProcessing::getPointOfCollision(point& a1, point& a2, point& b1, point& b2){
	point result(-1, -1);
	float x12 = a1.x - a2.x;
	float x34 = b1.x - b2.x;
	float y12 = a1.y - a2.y;
	float y34 = b1.y - b2.y;

	float c = x12 * y34 - y12 * x34;

	if (fabs(c) > 0.01)
	{
		// Intersection
		float a = a1.x * a2.y - a1.y  * a2.x;
		float b = b1.x * b2.y - b1.y * b2.x;

		float x = (a * x34 - b * x12) / c;
		float y = (a * y34 - b * y12) / c;

		result.x = x;
		result.y = y;
	}
	return result;
}

float IAProcessing::sign(point& p1, point& p2, point& p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool IAProcessing::PointInTriangle(point& pt, point& v1, point& v2, point& v3)
{
	bool b1, b2, b3;

	b1 = sign(pt, v1, v2) < 0.0f;
	b2 = sign(pt, v2, v3) < 0.0f;
	b3 = sign(pt, v3, v1) < 0.0f;

	//Draw
	if (false){
		std::vector<point> vec;
		vec.push_back(v1);
		vec.push_back(v3);
		vec.push_back(v2);
		//vec.push_back(pt);
		Correct(vec);
		for (auto it = vec.begin(); it != vec.end(); ++it){
			m_outputImage->setPixel(it->x, it->y, sf::Color::Yellow);
		}
	}
	return ((b1 == b2) && (b2 == b3));
}
bool IAProcessing::PointInCarre(point& pt, point& v1, point& v2){

	if (false){
		std::vector<point> vec;
		vec.push_back(v1);
		vec.push_back(point(v1.x, v2.y));
		vec.push_back(v2);
		vec.push_back(point(v2.x, v1.y));
		//vec.push_back(pt);
		Correct(vec);
		for (auto it = vec.begin(); it != vec.end(); ++it){
			m_outputImage->setPixel(it->x, it->y, sf::Color::Yellow);
		}
	}
	return ((pt.x >= v1.x && pt.x <= v2.x && pt.y >= v1.y && pt.y <= v2.y) || (pt.x >= v2.x && pt.x <= v1.x && pt.y >= v2.y && pt.y <= v1.y));
}

bool IAProcessing::checkAddJump(Link& l){
	for (auto it = m_links.begin(); it != m_links.end(); ++it){
		if (l.id != it->id && l.type == it->type && l.startingNode == it->startingNode && l.endingNode == it->endingNode){
			float d1, d2;
			d1 = distance(l.beginPosition, l.endPosition);
			d2 = distance(it->beginPosition, it->endPosition);
			if (d2 <= d1)
				return false;
			else{
				it = m_links.erase(it);
				Log::debug() << "erasing";
			}
		}
	}
	return true;
}

void IAProcessing::detectOnLine(Node& nodeOfOrigin, sf::Vector2f origin, float (*function)(float), bool increasing){
	sf::Vector2f delta(0,0), newpoint;
	//bool stop = false;
	for (auto it = nodeOfOrigin.area.begin(); it != nodeOfOrigin.area.end(); ++it)
		Log::debug() << it
	return;
	while (true)
	{
		if (increasing)
			delta.x++;
		else
			delta.x--;
		delta.y = function(delta.x);
		newpoint = origin + delta;
		Log::debug() << "newPoint = " << newpoint;
		if (newpoint.x < 0 || newpoint.y < 0 || newpoint.x > mapsize.x || newpoint.y > mapsize.y)
			return;
		for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
		{
			if (*it == nodeOfOrigin)
				continue;

			if (isPointInNode(*it, newpoint))
			{
				Log::debug() << "is ok ! with " << it->id;
				return;
			}
		}
	}
}

bool IAProcessing::isPointInNode(Node& node, sf::Vector2f& pt)
{
	std::vector<point> points = node.area;
	int i, j, nvert = points.size();
	bool c = false;

	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((points[i].y >= pt.y) != (points[j].y >= pt.y)) &&
			(pt.x <= (points[j].x - points[i].x) * (pt.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
			c = !c;
	}
	return c;
}

float test(float x)
{
	return x;
}
