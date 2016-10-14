/**
 * @file Application.h
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <vector>

#include "SFML/Graphics.hpp"

#include "Log.h"
#include "Timer.h"

#include "Optimizer.h"
#include "b2Separator.h"
#include "IAProcessing.h"


typedef std::vector <sf::Vector2i> Shape;
extern point mapsize;
extern int flags;

extern int DEBUG;
extern int PRINT;
extern int VERBOSE;
extern int TIME;


/**
 * @brief Handles every use of the application or redirect it
 */
class Application
{
public:
    /**
     * @brief Application
     * @param path The path of the input file
     */
	Application(std::string path);
	~Application();

    /**
     * @brief process Starts everything the "flags" consider
     * @param tolerance The tolerance for optimizing
     * @param output The output file
     */
	void process(float tolerance, const std::string& output);

private:
    /**
     * @brief findShapes Finds the shapes in the loaded source image
     */
    void findShapes();
    /**
     * @brief optimize Runs the optimization foreach shape in the vector
     * @param tolerance The tolerance for optimizing
     * @return  The new vector of shapes optimized
     */
    std::vector<std::vector<point>> optimize(float tolerance);

	std::vector<std::vector<std::vector<point>>> convexify(std::vector<std::vector<point>>& input);

    /**
     * @brief print Saves the shapes to a file
     * @param shapes The shapes to write
     */
    void print(std::vector<std::vector<std::vector<point>>> shapes);

	/**
	* @brief print Saves the shapes to a file
	* @param shapes The shapes to write
	*/
	void print(std::vector<std::vector<point>> shapes);

	/**
	* @brief print Saves the shapes to a file
	* @param shapes The shapes to write
	*/
	void print(std::vector<point> shapes);

    /**
     * @brief getPointsList
     * @return return the actual point list as std::vector<std::vector<point>>
     */
    std::vector<std::vector<point>> getPointsList();

    /**
     * @brief cut Cuts a shape in smaller convex ones
     * @param v The shape to cut
     * @return The list of result shapes
     */
    std::vector<std::vector<point>> cut(std::vector<point> v);

    /**
     * @brief toCut Says whether or not a shape should be cut
     * @param v The shape
     * @return true or false
     */
    bool toCut(std::vector<point> v);

    /**
     * @brief exploreShape Finds all the points belonging to a shape
     * @param shape The shape containing the starting point and in which all the supplementary points should be added
     */
    void exploreShape(Shape* shape);

    /**
     * @brief getNext Returns the next probable element of a shape
     * @param c The actual point's position
     * @param s The shape
     * @return The next point's position
     */
    sf::Vector2i getNext(sf::Vector2i& c, Shape* s);

    /**
     * @brief writeShape Writes all the shapes to a file
     * @param vec The vector of shapes
     * @param path The file to write the shapes in
     * @param xml Whether or not it should be written in XML style
     */
    void writeShape(std::vector<Shape>& vec, std::string path, bool xml = true);

    /**
     * @brief writeShape Writes the shape to a file
     * @param vec The shape
     * @param path The file to write the shape in
     * @param xml Whether or not it should be written in XML style
     */
    void writeShape(Shape& vec, std::string path, bool xml = true);


    /**
     * @brief hasMinimumOpacity Says whether or not a point in the image is opaque enough
     * @param v The point's position
     * @return true if it's opaque enough
     */
    bool hasMinimumOpacity(const sf::Vector2i& v);
    /**
     * @brief isOnBorder Says whether or not a point is a the border of a shape or inside
     * @param v The point's position
     * @return true if the point is on a shape's border
     */
    bool isOnBorder(sf::Vector2i& v);
    /**
     * @brief isInList Checks that a point isn't yet explored and in any shape
     * @param v The point's position
     * @return true if it is an unexplored point
     */
    bool isInList(sf::Vector2i& v);

private:

	sf::Image				m_inputImage;
	sf::Image				m_outputImage;
	IAProcessing*			m_IAProcessor;
	std::vector<Shape>		m_shapes;
	float					m_minimumOpacity;
	std::string				m_inputpath;
};

#endif
