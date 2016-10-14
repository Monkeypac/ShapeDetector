#include "Optimizer.h"

point mapsize = point();

int flags = 0;

int Sign(double x) {
	if (x < 0)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

void drawLine(point a, point b, std::vector<point>& vec, std::vector<point>::iterator& beginindex)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	int x;
	int y;
	//Log::debug() << "From " << a.x << a.y << " To " << b.x << b.y;
	//Log::debug("Before:") << vec.size();
	if (fabs(dy) > fabs(dx))
	{
		for (y = a.y; y != b.y; y += Sign(dy))
		{
			//Log::debug("|a|");
			x = a.x + (y - a.y) * dx / dy;
			beginindex = vec.emplace(beginindex, x, y);
			++beginindex;
		}
	}

	else
	{
		for (x = a.x; x != b.x; x += Sign(dx))
		{
			//Log::debug("|b|");
			y = a.y + (x - a.x) * dy / dx;
			beginindex = vec.emplace(beginindex, x, y);
			++beginindex;
		}
	}
	Log::debug("After:") << vec.size();
}

void Correct(std::vector<point>& vec){
	point prev = vec[0];
	for (auto it = vec.begin()+1; it != vec.end(); ++it){
		if (it->x > prev.x + 1 || it->x < prev.x - 1)
			drawLine(prev, *it, vec, it);
		else if (it->y > prev.y + 1 || it->y < prev.y - 1)
			drawLine(prev, *it, vec, it);
		prev = *it;
	}
	if (vec.size() > 2){
		prev = vec[vec.size() - 1];
		auto it = vec.begin();
		if (it->x > prev.x + 1 || it->x < prev.x - 1)
			drawLine(prev, *it, vec, it);
		else if (it->y > prev.y + 1 || it->y < prev.y - 1)
			drawLine(prev, *it, vec, it);
	}
}

void print(std::vector<point>& vec){
	point prev = vec[0];
	for (auto it = vec.begin(); it != vec.end(); ++it){
		if (it->x > prev.x + 1 || it->x < prev.x - 1)
			Log::debug() << "Here ! X";
		if (it->y > prev.y + 1 || it->y < prev.y - 1)
			Log::debug() << "Here ! Y";
		Log::debug() << it->x << " " << it->y;
		prev = *it;
	}
}

float distance(point x, point y){
	point delta;
	delta.x = y.x - x.x;
	delta.y = y.y - x.y;
	return sqrt(delta.x*delta.x + delta.y*delta.y);
}

void loadfromXML(std::vector<std::vector<point>>& vec, std::string& path)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());

	vec.clear();

	if (!result){
		std::cout << "bug loading file : " << path << " " << result.description() << "  " << result.offset << std::endl;
		return;
	}

	pugi::xml_node element = doc.child("shape");
	pugi::xml_node nodepoint;


	std::vector<point> v;
	point *buffer = nullptr;
	while (element){
		v.clear();
		nodepoint = element.child("point");
		while (nodepoint){
			buffer = new point();
			buffer->x = nodepoint.attribute("x").as_float();
			buffer->y = nodepoint.attribute("y").as_float();

			if (buffer->x > mapsize.x)
				mapsize.x = buffer->x;

			if (buffer->y > mapsize.y)
				mapsize.y = buffer->y;

			v.push_back(*buffer);
			nodepoint = nodepoint.next_sibling();
		}
		vec.push_back(v);
		element = element.next_sibling();
	}
	std::cout << "loaded file " << path << " successfully" << std::endl;
}

std::vector<point> Optimize(std::vector<point> points, float toleranceMult){
	std::vector<point> result;
	point begin = points.at(0);
	point next;

	point delta;

	float prevangle = -8000;
	float angle = 0.f;
	float tolerance = 0.f;
	float beginindex = 0.f;
	float index = 1.f;

	result.push_back(points[0]);
	while (index < points.size()){
		next = points.at(index);

		delta.x = next.x - begin.x;
		delta.y = next.y - begin.y;


		delta.x = fabs(delta.x);
		delta.y = fabs(delta.y);

		angle = std::fminf(delta.y, delta.x) / std::fmaxf(delta.x, delta.y);

		if (prevangle == -8000.f)
			prevangle = angle;

		if (index - beginindex - 1 == 0)
			tolerance = 0;
		else
			tolerance = 1 / (index - beginindex - 1);

		tolerance *= toleranceMult;
		if (angle < prevangle - tolerance || angle > prevangle + tolerance){
			result.push_back(points.at(index - 1));
			begin = points.at(index);
			beginindex = index;
			prevangle = -8000.f;
			index++;
		}
		else{
			prevangle = angle;
			index++;
		}
	}
	return result;
}

void write(std::vector<std::vector<point>>& vec, std::string path)
{
	std::ofstream s;
	s.open(path);
	for (std::vector<point> v : vec)
	{
		s << "<shape>" << std::endl;;
		for (point p : v)
		{
			s << "\t<point x=\"" << p.x << "\" y=\"" << p.y << "\" />" << std::endl;
		}
		s << "</shape>" << std::endl;;
	}

	s.close();
}

bool shapeLargeEnough(std::vector<point> vec, int pas)
{
	bool X, Y;
	X = false;
	Y = false;

	for (int i = 0; i < vec.size() - 1; i++)
	{
		if (abs(vec[i].x - vec[i + 1].x) > pas)
			X = true;
		if (abs(vec[i].y - vec[i + 1].y) > pas)
			Y = true;
	}

	return X && Y;
}

void write(std::vector<std::vector<std::vector<point>>>& vec, std::string path, std::string inputpath)
{
	std::ofstream s;
	s.open(path);
	s << "<Map>" << std::endl;
	s << "\t<Background texture='"<< inputpath <<"'/>" << std::endl;
	s << "\t<Ground texture=''/>" << std::endl;
	s << "\t<Foreground texture=''/>" << std::endl;
	s << "\t<MapElements>" << std::endl;
	for (std::vector<std::vector<point>> v : vec) {
		s << "\t\t<StaticElement>" << std::endl;
		for (std::vector<point> v2 : v) {
			if (v2.size() >= 3 && shapeLargeEnough(v2, 5)) {
				s << "\t\t\t<shape>" << std::endl;
				for (point p : v2) {
					s << "\t\t\t\t<point x=\"" << p.x << "\" y=\"" << p.y << "\" />" << std::endl;
				}
				s << "\t\t\t</shape>" << std::endl;
			}
		}
		s << "\t\t</StaticElement>" << std::endl;
	}
	s << "\t<MapElements>" << std::endl;
	s << "</Map>" << std::endl;

	s.close();
}