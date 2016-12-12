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
	//Log::debug() << "Done";
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		//Log::debug() << it->id;
		for (int i = 0; i < it->area.size(); i++)
		{
			//Log::debug() << it->area[i].x << it->area[i].y;
		}
	}
	//Log::debug() << "re Done";
	link();
}
/*
bool IAProcessing::is_between(point& first, point& second, point& checked){
	return (distance(first, checked) + distance(checked, second) == distance(first, second));
}
*/
bool IAProcessing::onSegment(point p, point q, point r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;

	return false;
}

int IAProcessing::orientation(point p, point q, point r)
{
	// See http://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear

	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

bool IAProcessing::doIntersect(point p1, point q1, point p2, point q2)
{
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if (o1 != o2 && o3 != o4)
		return true;
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
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
			Log::debug("Printing") << it->id << it->left << it->beginPosition.x << it->beginPosition.y << it->endPosition.x << it->endPosition.y << it->type << it->startingNode->id << it->endingNode->id;
		if (!it->beginPosition || !it->endPosition){
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
		//Log::debug() << "done !";
	}
}

void IAProcessing::link(){
	Log::debug() << "Link Begin";
	for (Node& n : m_nodes){
		testDrop(n);
	}
}

void IAProcessing::addLink(Node* begin, Node* end, point& beginPos, point& endPos, std::string type, bool left)
{
	Link newLink;
	newLink.beginPosition = beginPos;
	newLink.endPosition = endPos;
	newLink.startingNode = begin;
	newLink.endingNode = end;
	newLink.left = left;
	newLink.type = type;
	float newDist = distance(beginPos, endPos);
	for (auto it = m_links.begin(); it != m_links.end(); ++it)
	{
		Log::debug() << begin->id << it->id;
		if (newLink.startingNode == it->startingNode && newLink.endingNode == it->endingNode && newLink.beginPosition == it->beginPosition && distance(it->beginPosition, it->endPosition) < newDist){
			return;
		}
		else if (newLink.startingNode == it->startingNode && newLink.endingNode == it->endingNode && newLink.beginPosition == it->beginPosition && distance(it->beginPosition, it->endPosition) > newDist){
			it = m_links.erase(it);
			Log::debug() << "erased";
		}
	}
	Log::debug() << begin->id << "done add link";
}


void IAProcessing::testDrop(Node& n)
{
	Node* node;
	point endposition;
	std::function<float(float, float)> flinear = linear;
	for (float i = 0.f; i < 1.f; i += 0.1){
		auto fcurrentlinear = std::bind(flinear, std::placeholders::_1, i);
		node = detectOnLine(n, n.area[0], fcurrentlinear, endposition, true);
		if (node)
		{
			addLink(&n, node, n.area[0], endposition, "fall", false);
		}
		node = detectOnLine(n, n.area[n.area.size()-1], fcurrentlinear, endposition, false);
		if (node)
		{
			addLink(&n, node, n.area[0], endposition, "fall", true);
		}
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
/*
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
*/
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
/*
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
*/
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

Node* IAProcessing::detectOnLine(Node& nodeOfOrigin, point origin, std::function<float(float)> function, point &result, bool increasing){
	point delta(0,0), newpoint, previous;
	result.x = 0; result.y = 0;
	//bool stop = false;
	while (true)
	{
		if (increasing){
			delta.x++;
			delta.y = function(delta.x);
		}
		else{
			delta.x--;
			delta.y = function(-delta.x);
		}
		newpoint = origin + delta;
		//Log::debug() << "newPoint = " << newpoint.x << newpoint.y;
		if (newpoint.x < 0 || newpoint.y < 0 || newpoint.x > mapsize.x || newpoint.y > mapsize.y){
			//Log::debug() << "Over The map" << newpoint.x << newpoint.y << mapsize.x << mapsize.y;
			return nullptr;
		}
		for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
		{
			if (*it == nodeOfOrigin)
				continue;
			for (int i = 0; i < it->area.size()-1; i++)
			{
				if (doIntersect(it->area[i], it->area[i + 1], previous, newpoint)){
					Log::debug() << "is ok ! with " << it->id;
					result = newpoint;
					return &(*it);
				}
			}
		}
		previous = newpoint;
	}
}

float IAProcessing::distance(point& a, point& b)
{
	return sqrtf((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y)*(a.y - b.y)*(a.y - b.y));
}


/*bool IAProcessing::isPointInNode(Node& node, sf::Vector2f& pt)
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
}*/

float test(float x)
{
	return x;
}

float linear(float x, float p)
{
	return x*p;
}
