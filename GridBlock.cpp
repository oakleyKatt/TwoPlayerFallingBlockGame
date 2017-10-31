#include <vector>
#include <iterator>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
//#include "GridBlock.h"

using namespace cv;

class GridBlock {
	friend std::ostream& operator<<(std::ostream& os, const GridBlock& gb);
public:
	GridBlock(std::pair<int, int> gridCoord, std::pair<Point, Point> pixelCoord, int pixelCount) : gridCoord(gridCoord), pixelCoord(pixelCoord), pixelCount(pixelCount) {}
	
	// TODO - don't know if I use this?
	GridBlock& operator=(const GridBlock& src) {
	    // if this is equal to source
	    if (this != &src) {
	    	this->gridCoord = src.gridCoord;
	    	this->pixelCoord = src.pixelCoord;
			this->pixelCount = src.pixelCount;
			this->maxPixelCount = src.maxPixelCount;
	    }
	    return *this;
	}
	
	void setMaxPixelCount(int maxPixelCount) {
		this->maxPixelCount = maxPixelCount;
		//std::cout << this->maxPixelCount << "\n";
	}
	
	void drawGrid(Mat& grid, bool isSolid) {
		int gradient = (isSolid) ? 255:getGradient();
		// Make GridBlock Rect
		Rect block(this->getRect());
		rectangle(grid, block, gradient, -1, 8, 0);
		//putText(grid, std::to_string(gradient), Point(pixelCoord.first), )
	}
	
	Rect getRect() {
		// Make GridBlock Rect
		const Point pt1(this->pixelCoord.first);
		const Point pt2(this->pixelCoord.second);
		Rect block(pt1, pt2);
		return block;
	}
	
	int getGradient() {
		double gradientt = ((double)this->pixelCount / (double)this->maxPixelCount);
		gradientt = gradientt * 255.0;
		int gradient = (int) gradientt;
		return gradient;
	}
	
	// Returns true if the GridBlock's % of black pixels is greater than the given threshold %
	bool isTetrisBlock(const float blockTh) {
		return ((float)this->pixelCount / (float)this->maxPixelCount) > blockTh;
		//return true;
	}
private:
	// (m, n) coordinates - index location in the MxN grid
	std::pair<int, int> gridCoord;
	// (x, y) coordinates - actual pixel location of top left and bottom right corners
	std::pair<Point, Point> pixelCoord;
	// Black pixel count
	int pixelCount;
	// Max # of black pixels in the set of GridBlocks
	int maxPixelCount;
};

std::ostream& operator<<(std::ostream& os, const GridBlock& gb) {
    os << gb.gridCoord.first << " " << gb.gridCoord.second << " ";
	os << gb.pixelCoord.first << " " << gb.pixelCoord.second << " ";
	os << gb.pixelCount << " " << gb.maxPixelCount << "\n";
    return os;
}
