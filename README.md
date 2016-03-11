# ShapeDetector
A simple shape detector using sfml.

## Authors
- Yohan (HadesFufu) Masson
- Thomas (MonkeyPac) Magalhaes

## General
The project was developped for the necessity of a game development.
It uses the SFML library. 
It can be used for others purposes like for example, just detecting the shapes in an image.

## Use
For using the program, the picture needs to have a transparent background (alpha = 0)
The program is able to:
	- find all the points contituting all the shapes in the image
	- optimize a shape by reducting it's number of points. The main 
	  appearance of the shape should be kept. The optimization of the shape 
	  is parametrable using the tolerance parameter.
	- curve the optimized shapes in smaller ones in order to make it convex
	  and loadable without too much use of memory in any other software.
	- This software is originaly made for Box2D library.

## Install
In order to install the program, you need to have downloaded all the sources.
Goto the folder containing the sources.
Open a terminal and enter the following commands:
	make
