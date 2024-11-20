sample:		sample.cpp
		g++ -framework OpenGL -framework GLUT sample.cpp -o sample.runme -I. -Wno-deprecated -std=c++17 -Wc++11-extensions
