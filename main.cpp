#include <stdio.h>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "imageDiff.cpp"
#include "blobDetection.cpp"
#include "GridBlock.cpp"

using namespace cv;

/** Function Headers */
void displayImage(Mat img, const String windowName, int time);
void writeImage(Mat img, const char* dataDir, const char* filename);
void writeImage(Mat img, const char* dataDir, const std::string filename);
// Draw an MxN grid on the given image
Mat drawGrid(Mat img, int m, int n, Scalar color);
std::vector<GridBlock> analyzeMask(Mat mask, int m, int n);
int getPixelCount(Mat img, int th);
std::string getRawFilename(char* full);
Mat drawGridShade(Mat ogImg, std::vector<GridBlock> results);
void generateTetrisBlock(Mat& tetrisBlock, std::vector<GridBlock> gridBlocks, const float blockTh);
void generateTetrisBlock_grid(std::vector<std::vector<int> >& tetrisBlock, std::vector<GridBlock> gridBlocks, 
		const float blockTh/*, const char* blockfilename*/);

void detectAndDisplay(Mat frame, const char* dataDir, const std::string filename);

/** Global variables */
// Colors
Scalar red = Scalar(0,0,255);
Scalar green = Scalar(0,255,0);
Scalar blue = Scalar(255,0,0);
Scalar white = Scalar(255,255,255);
Scalar black = Scalar(0,0,0);

// Data and img files
FILE * dataFile;

// Threshold % used to determine whether the GridBlock is a tetris block
float blockTh;

// Grid dimensions
const int M = 5;
const int N = 5;

int main(int argc, char** argv ) {
	
	// Two images given - do Image Diff calculation
	// argv[1]	reference image
	// argv[2]	compared image
	// argv[3]	threshold value used in imageDiff() method
	// argv[4]	threshold % used to determine whether the GridBlock is a tetris block
	
	if(argc > 4) { 
			
		// load the reference and compare images from the given image files
	    Mat refImg;
	    refImg = imread( argv[1], 1 );
	    if ( !refImg.data ) {
	        printf("No image data - refImg\n");
	        return -1;
	    }
	    Mat cmpImg;
	    cmpImg = imread( argv[2], 1 );
	    if ( !cmpImg.data ) {
	        printf("No image data - cmpImg\n");
	        return -1;
	    }
		
		// stof() - converts string to float
		float threshold = std::stof(argv[3]);
		// Threshold % used to determine whether the GridBlock is a tetris block
		blockTh = std::stof(argv[4])/100;
		
		std::cout << "Threshold = " << threshold << "\n";
		std::cout << "Block Threshold = " << blockTh << "\n";
		
		Mat diffImg = imgDiff_img(refImg, cmpImg, threshold);
		
		Mat diffMask = imgDiff_mask(refImg, cmpImg, threshold);
		
		Mat diffMaskGrid = drawGrid(diffMask, M, N, red);
		
		// Blob Detection
		// Apply Median Filter (blur) before passing it to blob detection
		for(int i = 5; i < 37; i+=6) {
			Mat medianImg;
			medianBlur(diffMask, medianImg, i);
		
			// Analyze the difference mask
			std::vector<GridBlock> result;
			result = analyzeMask(medianImg, 5, 5);
			Mat gridGradient(cmpImg.rows, cmpImg.cols, CV_8UC3, Scalar(0, 0, 0));
			
			Mat tetrisBlock(cmpImg.rows, cmpImg.cols, CV_8UC3, Scalar(0, 0, 0));
			//cvtColor(cmpImg, tetrisBlock, COLOR_BGR2GRAY);
			generateTetrisBlock(tetrisBlock, result, blockTh);
			tetrisBlock = drawGrid(tetrisBlock, M, N, red);

			//const char* blockDataFile[6] = {'b','l','o','c','k',i};
			// Initialize 2D tetris block grid array (0 = no block, 1 = block)
			std::vector<std::vector<int> > tBlock(M, std::vector<int>(N, 0));
			generateTetrisBlock_grid(tBlock, result, blockTh/*, blockDataFile*/);
			
			// Add grid to median img and Save it
			medianImg = drawGrid(medianImg, M, N, red);
			
			std::cout << "----------------------------\n";
		}
	}
    return 0;
}

void generateTetrisBlock(Mat& tetrisBlock, std::vector<GridBlock> gridBlocks, const float blockTh) {
	for(std::vector<GridBlock>::iterator it = gridBlocks.begin(); it != gridBlocks.end(); ++it) {
		// If the GridBlock has more % black pixels than the given threshold %
		if(it->isTetrisBlock(blockTh)) {
			// Draw GridBlock gradient onto its grid area
			it->drawGrid(tetrisBlock, true);
		}
	}
}

void generateTetrisBlock_grid(std::vector<std::vector<int> >& tetrisBlock, std::vector<GridBlock> gridBlocks,
	 	const float blockTh/*, const char* blockfilename*/) {
	// Initialize outfile stream for storing the 0/1s of the tetris block
	//std::ofstream blockfile(blockfilename);
	int row = 0; //tetrisBlock.size();
	int col = 0; //tetrisBlock[0].size();
	//for(std::vector<GridBlock>::iterator it = gridBlocks.begin(); it != gridBlocks.end(); ++it) {
	for (int i = 0; i < gridBlocks.size(); i++) {
		tetrisBlock[row][col] = (gridBlocks[i].isTetrisBlock(blockTh)) ? 1:0;
		std::cout << tetrisBlock[row][col] << " ";
		//blockfile << tetrisBlock[row][col] << " ";
		// If the GridBlock has more % black pixels than the given threshold %
		/*
		if(it->isTetrisBlock(blockTh)) {
			// Set grid index to 1 (has block)
			tetrisBlock[row][col] = 1;
		} else {
			// Set grid index to 0 (no block)
			tetrisBlock[row][col] = 0;
		}*/
		// If they reached the end of the column
		if (col == (tetrisBlock[0].size() - 1)) {
			row++;
			col = 0;
			std::cout << "\n";
			//blockfile << "\n";
		}else {
			col++;
		}
	}
}	

std::string getRawFilename(char* full) {
	std::string tempFilename = full;
	size_t lastDirIndex = tempFilename.find_last_of("/");
	tempFilename = tempFilename.substr(lastDirIndex+1, std::strlen(full));
	size_t fileExtIdx = tempFilename.find_last_of(".");
	std::string filename = tempFilename.substr(0,fileExtIdx);
	return filename;
}

std::vector<GridBlock> analyzeMask(Mat mask, int m, int n) {
	std::vector<int> result;
	std::vector<Rect> submasks;
	std::vector<GridBlock> gridBlocks;
	int rows = mask.rows;
	int cols = mask.cols;
	int rHeight = rows/n;
	int cWidth = cols/m;
	int count = 0;
	int maxPixelCount = 0;
	// TODO ALT IMPLEMENTATION - do % of black pixels in grid
	// Draw vertical lines
	//for (int x = 0; x < m; x++) {
	for (int y = 0; y < n; y++) {
		//line(mask, Point(cWidth*i,0), Point(cWidth*i,rows), white);
		// Draw horizontal lines
		//for (int y = 0; y < n; y++) {
		for (int x = 0; x < m; x++) {
			// Formatted output to be used in processing program - used to visualize pixel density of the given grid
			//fprintf(dataFile,"%d,%d,%d,%d,%d,%d,",x,y,x*cWidth,y*rHeight,cWidth,rHeight);
			
			Mat submask(mask, Rect(x*cWidth, y*rHeight, cWidth, rHeight));
			std::pair<int,int> gridCoord(x,y);
			std::pair<Point,Point> pixelCoord(Point(x*cWidth,y*rHeight),Point(x*cWidth+cWidth, y*rHeight+rHeight));
			gridBlocks.push_back(GridBlock(gridCoord, pixelCoord, getPixelCount(submask, 0)));
			
			// Create a submask for the current grid (m,n)
			//TODO - add submask Rect to submasks list, then draw submasks on mask
			// submasks[i] = 
			// rect(x, y, width, height)
			
			// printf("\trow: %d\tcol: %d\n",submask.rows, submask.cols);
			result.push_back(getPixelCount(submask, 0));
			if(result.back() > maxPixelCount) {
				maxPixelCount = result.back();
			}
			count++;
		}
	}

	// Formatted output to be used in processing program
	//fprintf(dataFile, "%d\n", *std::max_element(result.begin(), result.end()));
	
	// Iterate through set of GridBlocks and set the maxPixelCount
	for(std::vector<GridBlock>::iterator it = gridBlocks.begin(); it != gridBlocks.end(); ++it) {
		it->setMaxPixelCount(maxPixelCount);
	}
	return gridBlocks;
}

/*
counts the number of pixels with the given value (th) in the specified image
*/
int getPixelCount(Mat img, int th) {
	int pixelCount = 0;
	int rows = img.rows;
	int cols = img.cols;
	//printf("\t\trow: %d\tcol: %d\n",rows, cols);
	for(int i=0; i<img.rows; i++) {
	    for(int j=0; j<img.cols; j++) {
	        if (img.at<uchar>(i,j) == th) {
				//printf("\t\t(%d,%d)\n",i,j);
	        	pixelCount++;
	        }
		}
	}
	//fprintf(dataFile,"%d\n",pixelCount);
	return pixelCount;
}

// TODO - implement this
// results - map of rect points and the corresponding black pixel count
Mat drawGridShade(Mat ogImg, std::vector<GridBlock> results) {
	Mat grid;
	//Mat grid = Mat(ogImg.size());
	
	return grid;
}

/*
	Draws an MxN grid on the given image
*/
Mat drawGrid(Mat img, int m, int n, Scalar color) {
	int rows = img.rows;
	int cols = img.cols;
	int rWidth = rows/n;
	int cWidth = cols/m;
	// Draw vertical lines
	for (int i = 1; i <= m; i++) {
		line(img, Point(cWidth*i,0), Point(cWidth*i,rows), color);
	}
	// Draw horizontal lines
	for (int i = 1; i <= n; i++) {
		line(img, Point(0,rWidth*i), Point(cols,rWidth*i), color);
	}
	return img;
}

void writeImage(Mat img, const char* dataDir, const char* filename) {
	const char* path = new char [std::strlen(dataDir)+std::strlen(filename)+1];
	std::strcat(const_cast<char*>(path),dataDir);
	std::strcat(const_cast<char*>(path),filename);
	if (!imwrite(path, img)){
		printf("[!!!] Error writing image file: \"%s\"\n",path);
	}else {
		printf("Image file written: \"%s\"\n",path);
	}
}

void writeImage(Mat img, const char* dataDir, const std::string filename) {
	writeImage(img, dataDir, filename.c_str());
}

void displayImage(Mat img, const String windowName, int time) {
	namedWindow(windowName, WINDOW_NORMAL);
	resizeWindow(windowName, 1280, 800);
    imshow(windowName, img);
	// display window infinitely until any keypress
	waitKey(time);
}