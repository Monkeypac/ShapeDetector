#include "Application.h"

int DEBUG = 1;
int PRINT = 1 << 1;
int VERBOSE = 1 << 2;
int TIME = 1 << 3;

Application::Application(std::string path) : m_minimumOpacity(0.1)//, pool()
{
	m_inputImage.loadFromFile(path);
	mapsize.x = m_inputImage.getSize().x;
	mapsize.y = m_inputImage.getSize().y;
	m_outputImage.create(mapsize.x + 1, mapsize.y + 1);
	m_IAProcessor = new IAProcessing(&m_outputImage);
	m_inputpath = path;
}

Application::~Application()
{
	m_outputImage.saveToFile("debugOutput.png");
	delete m_IAProcessor;
}

void Application::process(float tolerance, const std::string& output)
{
	std::vector<std::vector<point>> shapes;
	std::vector<std::vector<std::vector<point>>> ConvexShapes;
	Log::info() << "Begining Detection";
	if (flags & TIME) Timer::chronos<Application>();
	findShapes();
	if (flags & TIME) Timer::chronos<Application>("Detecting shapes");

	Log::info() << "Begining Optimization";
	if (flags & TIME) Timer::chronos<Application>();
	shapes = optimize(tolerance);
	if (flags & TIME) Timer::chronos<Application>("Optimization");

	Log::info() << "Begining IAProcessing";
	if (flags & TIME) Timer::chronos<Application>();
	m_IAProcessor->process(shapes);
	if (flags & TIME) Timer::chronos<Application>("IAProcessing");
	
	Log::info() << "Begining Convexification";
	if (flags & TIME) Timer::chronos<Application>();
	ConvexShapes = convexify(shapes);
	if (flags & TIME) Timer::chronos<Application>("Convexification");

	if (flags & TIME) Timer::chronos<Application>();
	write(ConvexShapes, output, m_inputpath);
	m_IAProcessor->writeAll(output);
	if (flags & TIME) Timer::chronos<Application>("Writing");
	
	if (flags & PRINT & TIME) Timer::chronos<Application>();
	if (flags & PRINT) {
		print(ConvexShapes);
		//m_IAProcessor->print();
	}
	if (flags & PRINT & TIME) Timer::chronos<Application>("Printing");

}

void Application::findShapes()
{
	sf::Vector2i shapeStartingPoint;
	Shape* currentshape;

    for (int x = 0; x < (int)m_inputImage.getSize().x; x++) {
        for (int y = 0; y < (int)m_inputImage.getSize().y; y++) {
			shapeStartingPoint.x = x;
			shapeStartingPoint.y = y;

            if (hasMinimumOpacity(shapeStartingPoint) && isOnBorder(shapeStartingPoint) && !isInList(shapeStartingPoint)) {
				m_shapes.emplace_back();
				currentshape = &m_shapes[m_shapes.size() - 1];

				currentshape->push_back(shapeStartingPoint);

                exploreShape(currentshape);
			}
		}
	}

	if (flags & DEBUG)
		writeShape(m_shapes, "debug.txt", false);

	if (flags & VERBOSE)
		writeShape(m_shapes, "map_explored.xml", true);
}

std::vector<std::vector<point>> Application::optimize(float tolerance)
{
	std::vector<std::vector<point>> points = getPointsList();
	std::vector<std::vector<point>> result;

	// optimize then separate
    for (std::vector<point> v : points) {
		Correct(v);
		result.push_back(Optimize(v, tolerance));
	}

	return result;
}



std::vector<std::vector<std::vector<point>>> Application::convexify(std::vector<std::vector<point>>& input){

	std::vector<point> tmp_vect;
	std::vector<std::vector<point>> tmp_sep;
	std::vector<std::vector<std::vector<point>>> result;
	for (std::vector<point> v : input) {
		tmp_vect.clear();
		tmp_vect = v;
		tmp_sep = b2Separator::Separate(&tmp_vect);
		for (auto it = tmp_sep.begin(); it != tmp_sep.end(); ++it)
			std::reverse(it->begin(), it->end());
		result.push_back(tmp_sep);
	}
// take sure all shapes are well formed
	std::vector<std::vector<std::vector<point>>> tmp_res;
	for (std::vector<std::vector<point>> v : result) {
		std::vector<std::vector<point>> tmp_v;
		for (std::vector<point> v2 : v){
			if (toCut(v2)) {
				for (std::vector<point> av : cut(v2))
					tmp_v.push_back(av);
				}
			else
				tmp_v.push_back(v2);
		}
		tmp_res.push_back(tmp_v);
	}
	result = tmp_res;
	return result;

}

void Application::print(std::vector<std::vector<std::vector<point>>> shapes)
{
	int i = 0;
    for (std::vector<std::vector<point>> v : shapes) {
		for (std::vector<point> v2 : v){
            if (v2.size() >= 3 && shapeLargeEnough(v2, 5)) {
				Correct(v2);
				for (auto it = v2.begin(); it != v2.end(); ++it){
					m_outputImage.setPixel(it->x, it->y, sf::Color::Green);
					i++;
				}
			}
		}
	}
}

void Application::print(std::vector<std::vector<point>> shapes)
{
	int i = 0;
	for (std::vector<point> v : shapes) {
		if (v.size() >= 3 && shapeLargeEnough(v, 5)) {
			Correct(v);
			for (auto it = v.begin(); it != v.end(); ++it){
				m_outputImage.setPixel(it->x, it->y, sf::Color::Green);
				i++;
			}
		}
	}
}


void Application::print(std::vector<point> shapes)
{
	int i = 0;
	if (shapes.size() >= 3 && shapeLargeEnough(shapes, 5)) {
		Correct(shapes);
		for (auto it = shapes.begin(); it != shapes.end(); ++it){
			m_outputImage.setPixel(it->x, it->y, sf::Color::Green);
			i++;
		}
	}
}

bool Application::isOnBorder(sf::Vector2i& v)
{
    if (v.x - 1 < 0 || v.x + 1 >= (int)m_inputImage.getSize().x)
		return true;
    if (v.y - 1 < 0 || v.y + 1 >= (int)m_inputImage.getSize().y)
		return true;
	if (!hasMinimumOpacity(sf::Vector2i(v.x - 1, v.y))) //Left
		return true;
	if (!hasMinimumOpacity(sf::Vector2i(v.x, v.y - 1))) //Top
		return true;
	if (!hasMinimumOpacity(sf::Vector2i(v.x + 1, v.y))) //Right
		return true;
	if (!hasMinimumOpacity(sf::Vector2i(v.x, v.y + 1))) //Bottom
		return true;

	return false;
}
bool Application::hasMinimumOpacity(const sf::Vector2i& v)
{
	return (m_inputImage.getPixel(v.x, v.y).a > m_minimumOpacity * 255);

}

bool Application::isInList(sf::Vector2i& v)
{
    for (auto it = m_shapes.begin(); it != m_shapes.end(); ++it)
        for (auto it2 = it->begin(); it2 != it->end(); ++it2)
            if (v == *(it2))
                return true;

	return false;
}
void Application::exploreShape(Shape* shape)
{
	sf::Vector2i c = shape->at(0);

    while ((c = getNext(c, shape)).x != -1)
        shape->push_back(c);
}

sf::Vector2i Application::getNext(sf::Vector2i& c, Shape* s)
{
	sf::Vector2i p;
    if (c.x - 1 >= 0){
		if (isOnBorder(p = (sf::Vector2i(c.x - 1, c.y))) && !isInList(p) && hasMinimumOpacity(p)) //Left
            return p;
        if (c.y - 1 >= 0)
			if (isOnBorder(p = (sf::Vector2i(c.x - 1, c.y - 1))) && !isInList(p) && hasMinimumOpacity(p)) //Left - Top
				return p;
    }

	if (c.y - 1 >= 0){
		if (isOnBorder(p = (sf::Vector2i(c.x, c.y - 1))) && !isInList(p) && hasMinimumOpacity(p)) //Top
			return p;
        if (c.x + 1 < (int)m_inputImage.getSize().x)
			if (isOnBorder(p = (sf::Vector2i(c.x + 1, c.y - 1))) && !isInList(p) && hasMinimumOpacity(p)) //Top - Right
				return p;
	}
    if (c.x + 1 < (int)m_inputImage.getSize().x){
		if (isOnBorder(p = (sf::Vector2i(c.x + 1, c.y))) && !isInList(p) && hasMinimumOpacity(p)) //Right
			return p;
        if (c.y + 1 < (int)m_inputImage.getSize().y)
			if (isOnBorder(p = (sf::Vector2i(c.x + 1, c.y + 1))) && !isInList(p) && hasMinimumOpacity(p)) //Right - Bottom
				return p;
	}
    if (c.y + 1 < (int)m_inputImage.getSize().y){
		if (isOnBorder(p = (sf::Vector2i(c.x, c.y + 1))) && !isInList(p) && hasMinimumOpacity(p)) //Bottom
			return p;
        if (c.x - 1 >= 0)
			if (isOnBorder(p = (sf::Vector2i(c.x - 1, c.y + 1))) && !isInList(p) && hasMinimumOpacity(p)) //Bottom - Left
				return p;
	}

	p.x = -1;
	return p;
}

void Application::writeShape(std::vector<Shape>& vec, std::string path, bool xml)
{
	for (Shape s : vec)
		writeShape(s, path, xml);
}

void Application::writeShape(Shape& vec, std::string path, bool xml)
{
	std::ofstream st;
	st.open(path);

	st << "<shape>" << std::endl;
    for (sf::Vector2i c : vec) {
		if (xml)
			st << "\t<point x=\"" << c.x << "\" y=\"" << c.y << "\" />" << std::endl;
		else
			st << "\t" << c.x << "," << c.y << std::endl;
	}
	st << "</shape>" << std::endl;

	st.close();
}

std::vector<std::vector<point>> Application::getPointsList()
{
	std::vector<std::vector<point>> vec;

    for (Shape s : m_shapes) {
		std::vector<point>* sub;
		vec.emplace_back();
		sub = &vec[vec.size() - 1];
		for (sf::Vector2i c : s)
			sub->push_back(point(c.x, c.y));
	}

	return vec;
}

std::vector<std::vector<point>> Application::cut(std::vector<point> v)
{
	int mid, end;
	point p_mid, p_end;
	std::vector<std::vector<point>> result;
	std::vector<point> v_a, v_b;

	end = v.size() - 1;
	mid = end / 2;

	p_mid = v.at(mid);
	p_end = v.at(end);

    for (int i = 0; i < (int)v.size(); i++) {
		if (i <= mid)
			v_a.push_back(v.at(i));
		if (i >= mid)
			v_b.push_back(v.at(i));
	}

	v_a.push_back(p_end);

	if (toCut(v_a))
		for (std::vector<point> tmp : cut(v_a))
			result.push_back(tmp);
	else
		result.push_back(v_a);

	if (toCut(v_b))
		for (std::vector<point> tmp : cut(v_b))
			result.push_back(tmp);
	else
		result.push_back(v_b);

	return result;
}

bool Application::toCut(std::vector<point> v)
{
	int max_vertices = 8;
    if ((int)v.size() > max_vertices)
		return true;
	return false;
}
