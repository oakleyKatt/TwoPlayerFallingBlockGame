//
//  BlockGenerator.cpp
//  TwoPlayerTetris
//
//  Created by Oakley Katterheinrich on 12/28/17.
//  Copyright © 2017 Oakley Katterheinrich. All rights reserved.
//

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <opencv2/opencv.hpp>

Mat generateBlock(const int M, const int N);
void printBlock(const Mat& block);
Mat buildBlock(Mat block, std::pair<size_t, size_t> origin, int vert, double pth_stop, double pth0);
Mat buildSimpleBlock(Mat block, std::pair<size_t, size_t> origin, int vert);
Mat buildSimpleBlock_steps(Mat block, std::pair<size_t, size_t> origin, int vert);
Mat buildCrawlBlock(Mat block, std::pair<size_t, size_t> origin, int pth_cont, int prevdir);
Mat buildCrawlBlock_steps(Mat block, std::pair<size_t, size_t> origin, int pth_cont, int prevdir);
bool canCrawlInDir(Mat block, std::pair<size_t, size_t> origin, std::map<int, std::pair<int, int> > dirmap, int dir);
int getRandom(int min, int max);

// Absolute max line length
const int MAX = 3;

Mat generateBlock(const int M, const int N) {
    // initialize empty block grid
    // TODO - implement one-liner init
    //mat block(M, N);
    Mat block = Mat::zeros(M, N, CV_32SC1);
	// init origin @ middle of grid
	std::pair<size_t, size_t> origin;
	origin.first = 2;
	origin.second = 2;
	int vert = 0;
	double pth_stop = 0.5;
	double pth0 = 0.2;
    //return buildBlock(block, origin, vert, pth_stop, pth0);
	//return buildSimpleBlock(block, origin, vert);
	int pth_cont = 90;
	return buildCrawlBlock(block, origin, pth_cont, -1);
}

/**
 int prevdir - previous crawl direction
		0
	1	x	2
		3
*/
Mat buildCrawlBlock(Mat block, std::pair<size_t, size_t> origin, int pth_cont, int prevdir) {
	
	// Create direction map
	std::map<int, std::pair<int, int> > dirmap;
	dirmap[0] = std::pair<int, int>(-1,0);
	dirmap[1] = std::pair<int, int>(0,-1);
	dirmap[2] = std::pair<int, int>(0,1);
	dirmap[3] = std::pair<int, int>(1,0);
	
	// Ensure the origin block is filled in
	block.at<int>(origin.first, origin.second) = 1;
	
	// Choose a direction to crawl in
	int dir;
	do {
		dir = getRandom(0,3);
	} while(dir == prevdir && canCrawlInDir(block, origin, dirmap, dir));
	std::cout << "Direction: " << dir << "\n";
	
	// Add the crawl to the block + SHIFT origin in direction
	std::cout << "Origin: " << origin.first << ", " << origin.second << "\n";
	origin.first = origin.first + dirmap[dir].first;
	origin.second = origin.second + dirmap[dir].second;
	
	block.at<int>(origin.first, origin.second) = 1;
	std::cout << "New Origin: " << origin.first << ", " << origin.second << "\n";
	
	// Determine if the next block will be placed
	int pth = getRandom(0,100);
	// Continue adding crawl to the block
	if (pth < pth_cont) {
		// decrement chance of crawl continuing to add blocks
		pth_cont -= 10;
		return buildCrawlBlock(block, origin, pth_cont, dir);
	} else {
		return block;
	}
}

Mat buildCrawlBlock_steps(Mat block, std::pair<size_t, size_t> origin, int pth_cont, int prevdir) {
	
	// Create direction map
	std::map<int, std::pair<int, int> > dirmap;
	dirmap[0] = std::pair<int, int>(-1,0);
	dirmap[1] = std::pair<int, int>(0,-1);
	dirmap[2] = std::pair<int, int>(0,1);
	dirmap[3] = std::pair<int, int>(1,0);
	
	// Ensure the origin block is filled in
	block.at<int>(origin.first, origin.second) = 1;
	
	// Choose a direction to crawl in
	int dir;
	do {
		dir = getRandom(0,3);
	} while(dir == prevdir && canCrawlInDir(block, origin, dirmap, dir));
	std::cout << "Direction: " << dir << "\n";
	
	// Add the crawl to the block + SHIFT origin in direction
	std::cout << "Origin" << origin.first << ", " << origin.second << "\n";
	origin.first = origin.first + dirmap[dir].first;
	origin.second = origin.second + dirmap[dir].second;
	
	block.at<int>(origin.first, origin.second) = 1;
	std::cout << "New Origin" << origin.first << ", " << origin.second << "\n";
	
	
	// Determine if the next block will be placed
	int pth = getRandom(0,100);
	// Continue adding crawl to the block
	if (pth < pth_cont) {
		// decrement chance of crawl continuing to add blocks
		pth_cont -= 10;
		return buildCrawlBlock(block, origin, pth_cont, dir);
	} else {
		return block;
	}
}

bool canCrawlInDir(Mat block, std::pair<size_t, size_t> origin, std::map<int, std::pair<int, int> > dirmap, int dir) {
	return (origin.first + dirmap[dir].first >= block.rows || static_cast<int>(origin.first) + dirmap[dir].first < 0)
		|| (origin.second + dirmap[dir].second >= block.cols || static_cast<int>(origin.second) + dirmap[dir].second < 0);
}

/**
	int vert -	0 = line will be placed horizontally on top of origin
				1 = line will be placed vertically on top of origin
*/
Mat buildBlock(Mat block, std::pair<size_t, size_t> origin, int vert, double pth_stop, double pth0) {
	// Calculate valid [min, max] range for line length
	// ^ TODO
	int max = 3;
	// Get line length
	int lineLength = getRandom(1, max);
	// Place line on top of origin
	
	/* After line has been placed - prep recursion params */
	// Reposition origin
	int x, y;
	if (vert) {
		
		vert = 0;
	} else {
		
		vert = 1;
	}
	
	return block;
}

// TODO - shift index of where the line will be overlaid onto the origin
/**

line: [][x][]	(default)
0 0 0 0 0
0 0 0 0 0
0 0 x 0 0
0 0 0 0 0
0 0 0 0 0
results in:
0 0 0 0 0
0 0 0 0 0
0 1 1 1 0
0 0 0 0 0
0 0 0 0 0

line: [x][][]	(shifted origin)
results in:
0 0 0 0 0
0 0 0 0 0
0 0 1 1 1
0 0 0 0 0
0 0 0 0 0

*/
Mat buildSimpleBlock(Mat block, std::pair<size_t, size_t> origin, int vert) {
	// Calculate valid [min, max] range for line length
	int max = MAX;
	if (vert) {
		max = (origin.first < block.rows-origin.first) ? origin.first : block.rows-origin.first;
	} else {
		max = (origin.second < block.cols-origin.second) ? origin.second : block.cols-origin.second;
	}
	// Get line length
	int lineLength = getRandom(1, max);
	// TODO - [~?] bad handle for max = 0
	if (abs(lineLength) > MAX) {
		return block;
	}
	// Calculate number of spaces the line is shifted
	// TODO ensure line is still within block grid
	int shift = 0;
	// Place line on top of origin
	if (vert) {
		for (int i = 0; i <= lineLength; i++) {
			block.at<int>(origin.first+i+shift, origin.second) = 1;
			block.at<int>(origin.first-i+shift, origin.second) = 1;
		}
	} else {
		for (int i = 0; i <= lineLength; i++) {
			block.at<int>(origin.first, origin.second+i+shift) = 1;
			block.at<int>(origin.first, origin.second-i+shift) = 1;
		}
	}
	
	// Determine whether to continue adding lines to the block
	// TODO decrement chance of pth_stop = false with each added line
	int pth_stop = getRandom(0,10);
	// Stop adding lines, return block
	if (pth_stop < 4) {
		return block;
	} 
	// Continue adding lines to the block
	else {
		// Move origin to somewhere on newly added line
		if (vert) {
			int shiftup = getRandom(0,1);
			int shift = getRandom(0,max);
			// Shift the origin up
			if (shiftup) {
				origin.first = origin.first - shift;
			} 
			// Shift the origin down
			else {
				origin.first = origin.first + shift;
			}
			// Flip the line orientation to horizontal
			vert = 0;
		} else {
			int shiftright = getRandom(0,1);
			int shift = getRandom(0,max-1);
			// Shift the origin right
			if (shiftright) {
				origin.second = origin.second + shift;
			} 
			// Shift the origin left
			else {
				origin.second = origin.second - shift;
			}
			// Flip the line orientation to vertical
			vert = 1;
		}
		return buildSimpleBlock(block, origin, vert);
	}
}

Mat buildSimpleBlock_steps(Mat block, std::pair<size_t, size_t> origin, int vert) {
	std::cout << "[----------------------]\n";
	std::cout << "Origin: " << origin.first << "," << origin.second << "\n";
	std::cout << "Vert: " << vert << "\n";
	// Calculate valid [min, max] range for line length
	int max = MAX;
	if (vert) {
		max = (origin.first < block.rows-origin.first) ? origin.first : block.rows-origin.first;
	} else {
		max = (origin.second < block.cols-origin.second) ? origin.second : block.cols-origin.second;
	}
	std::cout << "Max line length: " << max << "\n";
	// Get line length
	int lineLength = getRandom(1, max);
	std::cout << "Line length = " << lineLength << "\n";
	// TODO - [~?] bad handle for max = 0
	if (abs(lineLength) > MAX) {
		return block;
	}
	// Calculate number of spaces the line is shifted
	// TODO ensure line is still within block grid
	int shift = 0;
	// Place line on top of origin
	if (vert) {
		for (int i = 0; i <= lineLength; i++) {
			block.at<int>(origin.first+i+shift, origin.second) = 1;
			block.at<int>(origin.first-i+shift, origin.second) = 1;
		}
	} else {
		for (int i = 0; i <= lineLength; i++) {
			block.at<int>(origin.first, origin.second+i+shift) = 1;
			block.at<int>(origin.first, origin.second-i+shift) = 1;
		}
	}
	
	// Determine whether to continue adding lines to the block
	// TODO decrement chance of pth_stop = false with each added line
	int pth_stop = getRandom(0,10);
	std::cout << "pth_stop: " << pth_stop << "\n";
	// Stop adding lines, return block
	if (pth_stop < 4) {
		return block;
	} 
	// Continue adding lines to the block
	else {
		// Move origin to somewhere on newly added line
		if (vert) {
			int shiftup = getRandom(0,1);
			int shift = getRandom(0,max);
			// Shift the origin up
			if (shiftup) {
				std::cout << "Shift up by " << shift << "\n";
				origin.first = origin.first - shift;
			} 
			// Shift the origin down
			else {
				std::cout << "Shift down by " << shift << "\n";
				origin.first = origin.first + shift;
			}
			// Flip the line orientation to horizontal
			vert = 0;
		} else {
			int shiftright = getRandom(0,1);
			int shift = getRandom(0,max-1);
			// Shift the origin right
			if (shiftright) {
				std::cout << "Shift right by " << shift << "\n";
				origin.second = origin.second + shift;
			} 
			// Shift the origin left
			else {
				std::cout << "Shift left by " << shift << "\n";
				origin.second = origin.second - shift;
			}
			// Flip the line orientation to vertical
			vert = 1;
		}
		printBlock(block);
		return buildSimpleBlock(block, origin, vert);
	}
}

// Randomly generated int within range [min, max]
int getRandom(int min, int max) {
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(min, max);
	return distr(eng);
}

void printBlock(const Mat& block) {
    for (size_t r = 0; r < block.rows; r++) {
        for (size_t c = 0; c < block.cols; c++) {
            std::cout << block.at<int>(r, c) << " ";
        }
        std::cout << "\n";
    }
}