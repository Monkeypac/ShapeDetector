/**
 * @file Optimizer.h
 */

#ifndef Optimizer_H
#define Optimizer_H

#include "external/pugixml/pugixml.hpp"
#include "Log.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <SFML/Graphics.hpp>

struct point{
	point() : x(0.f), y(0.f){}
	point(int a, int b) : x(a), y(b){}
	float x;
	float y;
};

typedef struct point point;

extern point mapsize;

extern int flags;

/**
 * @brief Sign
 * @param x
 * @return -1 if x negative, 1 else
 */
int Sign(double x);

/**
 * @brief shapeLargeEnough Says whether or not a shape is large enough or if it should be cut
 * @param vec the shape
 * @param pas the parameter to determine if the condition is validated
 * @return whether or not a shape is large enough or if it should be cut
 */
bool shapeLargeEnough(std::vector<point> vec, int pas);

/**
 * @brief drawLine "Draws" a line between two point (completes by adding the missing points)
 * @param a
 * @param b
 * @param vec
 * @param beginindex
 */
void drawLine(point a, point b, std::vector<point>& vec, std::vector<point>::iterator& beginindex);

/**
 * @brief Correct Corrects every possible hole between points
 * @param vec The shape to correct holes from
 */
void Correct(std::vector<point>& vec);

/**
 * @brief print Displays the vector
 * @param vec The vector to display
 */
void print(std::vector<point>& vec);

/**
 * @brief distance Calculates the distance between two points
 * @param x The first point
 * @param y The second point
 * @return the distance between the two points
 */
float distance(point x, point y);

/**
 * @brief loadfromXML Loads the xml file in the vector
 * @param vec The vector to load elements in
 * @param path The file from which the elements are loaded
 */
void loadfromXML(std::vector<std::vector<point>>& vec, std::string& path);

/**
 * @brief Optimize Optimizes the shape
 * @param points The shape to optimize
 * @param toleranceMult The tolerance used (angle)
 * @return The optimized shape
 */
std::vector<point> Optimize(std::vector<point> points, float toleranceMult = 1);

/**
 * Ecrit l'ensemble du contenu dans un fichier sous la forme de l'entrée
	<shape>
		<point x="123" y="123" />
	</shape>
 */

/**
 * @brief write Writes all the data in a file following the sample:
    <shape>
        <point x="123" y="123" />
    </shape>
 * @param vec The vector of shapes to write
 * @param path The file to write in
 */
void write(std::vector<std::vector<point>>& vec, std::string path);

/**
 * @brief write Writes all the data in a file following the sample:
    <shape>
        <point x="123" y="123" />
    </shape>
 * @param vec The vector of shapes to write
 * @param path The file to write in
 */
void write(std::vector<std::vector<std::vector<point>>>& vec, std::string path);

#endif
