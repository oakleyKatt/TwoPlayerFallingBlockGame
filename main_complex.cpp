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
		const float blockTh, const char* blockfilename);

void detectAndDisplay(Mat frame, const char* dataDir, const std::string filename);

/** Global variables */
String fullbody_cascade_name = "haarcascade/haarcascade_fullbody.xml";
String lowerbody_cascade_name = "haarcascade/haarcascade_lowerbody.xml";
String upperbody_cascade_name = "haarcascade/haarcascade_upperbody.xml";

CascadeClassifier fullbody_cascade;
CascadeClassifier lowerbody_cascade;
CascadeClassifier upperbody_cascade;

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
		
		//-- 1. Load the cascades
	    if(!fullbody_cascade.load(fullbody_cascade_name)){ 
			printf("--(!)Error loading fullbody cascade\n"); 
			return -1; 
		};
	    if(!lowerbody_cascade.load(lowerbody_cascade_name)){ 
			printf("--(!)Error loading lowerbody cascade\n"); 
			return -1; 
		};
	    if(!upperbody_cascade.load(upperbody_cascade_name)){ 
			printf("--(!)Error loading upperbody cascade\n"); 
			return -1; 
		};
		
		// stof() - converts string to float
		float threshold = std::stof(argv[3]);
		// Threshold % used to determine whether the GridBlock is a tetris block
		blockTh = std::stof(argv[4])/100;
		
		std::cout << "Threshold = " << threshold << "\n";
		std::cout << "Block Threshold = " << blockTh << "\n";
		
		// Make directory name and mkdir command
		const char* cmd = "mkdir -p testData/";
		// Generate a directory name with the two input file names
		std::string rawname_ref = getRawFilename(argv[1]);
		std::string rawname_cmp = getRawFilename(argv[2]);
		std::string dirName = rawname_ref.append("_");
		dirName.append(rawname_cmp.append("__"));
		std::string thName = argv[3];
		dirName.append(thName);
		// vvv causes string out of bounds error vvv
		//size_t decIdx = dirName.find_last_of(".");
		//dirName.replace(decIdx, 1, "_");
		// Directory path w/o last '/' in it
		const char* mkdirCmd = new char [std::strlen(cmd)+std::strlen(dirName.c_str())+1];
		std::string rawDirName = dirName;
		dirName.append("/");
		std::strcat(const_cast<char*>(mkdirCmd),cmd);
		//std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		std::strcat(const_cast<char*>(mkdirCmd),dirName.c_str());
		//std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		std::cout << "argv[1]:\t" << argv[1] << "\n";
		std::cout << "argv[2]:\t" << argv[2] << "\n";
		//std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		// make 'dataDir' path -> will be used later in program as path for writing files to
		const char* constDir = "testData/";
		//std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		const char* dataDir = new char [std::strlen(constDir)+std::strlen(dirName.c_str())+1];
		//std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		std::strcat(const_cast<char*>(dataDir),constDir);
		//std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		std::strcat(const_cast<char*>(dataDir),dirName.c_str());
		//std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		// Call mkdir command
		//std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		//std::cout << "dirName:\t" << dirName << "\n";
		std::cout << "dataDir:\t" << dataDir << "\n";
		std::cout << "mkdirCmd:\t" << mkdirCmd << "\n";
		
		const char* dataFileDirName = "data/";
		const char* mkDataFileDirCmd = new char[std::strlen(mkdirCmd)+std::strlen(dataFileDirName)+1];
		std::strcat(const_cast<char*>(mkDataFileDirCmd),mkdirCmd);
		std::strcat(const_cast<char*>(mkDataFileDirCmd),dataFileDirName);
		
		const char* dataFileDir = new char[std::strlen(dataDir)+std::strlen(dataFileDirName)+1];
		std::strcat(const_cast<char*>(dataFileDir),dataDir);
		std::strcat(const_cast<char*>(dataFileDir),dataFileDirName);
		std::cout << "dataFileDir:\t" << dataFileDir << "\n";
		std::cout << "mkDataFileDirCmd:\t" << mkDataFileDirCmd << "\n";
		const int dir_err = system(mkdirCmd);
		if (-1 == dir_err) {
		    printf("Error creating directory!n");
		    exit(1);
		}
		
		const int dir_err1 = system(mkDataFileDirCmd);
		if (-1 == dir_err1) {
		    printf("Error creating directory!n");
		    exit(1);
		}
		
		Mat diffImg = imgDiff_img(refImg, cmpImg, threshold);
		//displayImage(diffImg, "Diff Img", 0);
		std::string diffImgName = rawDirName;
		diffImgName.append("_diffImg.jpg");
		//std::string diffImgName = rawDirName.append("_diffImg.jpg");
		writeImage(diffImg, dataDir, diffImgName.c_str());
		
		Mat diffMask = imgDiff_mask(refImg, cmpImg, threshold);
		//displayImage(diffMask, "Diff Img", 0);
		std::string diffMaskName = rawDirName;
		diffMaskName.append("_diffMask.jpg");
		writeImage(diffMask, dataDir, diffMaskName.c_str());
		
		Mat diffMaskGrid = drawGrid(diffMask, M, N, red);
		std::string diffMaskGridName = rawDirName;
		diffMaskGridName.append("_diffMaskGrid.jpg");
		writeImage(diffMaskGrid, dataDir, diffMaskGridName.c_str());
		
		// Blob Detection
		// Apply Median Filter (blur) before passing it to blob detection
		for(int i = 5; i < 37; i+=6) {
			Mat medianImg;
			medianBlur(diffMask, medianImg, i);
			//Mat blobImg = detectBlobs(medianImg,5);
			//Mat blobImgGrid = drawGrid(blobImg, 5, 5, red);
			
			std::string medianImgName = rawDirName;
			medianImgName.append("_medianImg");
			medianImgName.append(std::to_string(i));
			std::string tetrisBlockName = medianImgName + "_tetris.jpg";
			std::string gradientName = medianImgName + "_gg.jpg";
			std::string dataname = medianImgName + "_data.txt";
			std::string blockfilename = medianImgName + "_tetrisBlock.txt";
			std::string haarName = medianImgName + "_haar.jpg";
			medianImgName.append(".jpg");
			
			const char* medianDataFile = new char[std::strlen(dataFileDir) + dataname.length() + 1];
			std::strcat(const_cast<char*>(medianDataFile),dataFileDir);
			std::cout << "dataFileDir = " << dataFileDir << "\n";
			std::strcat(const_cast<char*>(medianDataFile),dataname.c_str());
			std::cout << "medianDataFile = " << medianDataFile << "\n";
			// Open data file, set to write
			//dataFile = fopen (medianDataFile,"w");
			std::ofstream outfile(medianDataFile);
		
			// Analyze the difference mask
			std::vector<GridBlock> result;
			result = analyzeMask(medianImg, 5, 5);
			//Mat gridGradient;
			Mat gridGradient(cmpImg.rows, cmpImg.cols, CV_8UC3, Scalar(0, 0, 0));
			//cvtColor(medianImg, gridGradient, COLOR_BGR2GRAY);
			// Iterate through GridBlock vector
			for(std::vector<GridBlock>::iterator it = result.begin(); it != result.end(); ++it) {
				// Save to data file
				outfile << *it;
				// Draw Grid Gradient
				it->drawGrid(gridGradient, false);
			}
			// Save Grid Gradient img
			writeImage(gridGradient, dataDir, gradientName.c_str());
			
			Mat tetrisBlock(cmpImg.rows, cmpImg.cols, CV_8UC3, Scalar(0, 0, 0));
			//cvtColor(cmpImg, tetrisBlock, COLOR_BGR2GRAY);
			generateTetrisBlock(tetrisBlock, result, blockTh);
			tetrisBlock = drawGrid(tetrisBlock, M, N, red);
			// Save tetris block img
			writeImage(tetrisBlock, dataDir, tetrisBlockName.c_str());

			const char* blockDataFile = new char[std::strlen(dataFileDir) + blockfilename.length() + 1];
			std::strcat(const_cast<char*>(blockDataFile),dataFileDir);
			std::strcat(const_cast<char*>(blockDataFile),blockfilename.c_str());
			// Initialize 2D tetris block grid array (0 = no block, 1 = block)
			std::vector<std::vector<int> > tBlock(M, std::vector<int>(N, 0));
			generateTetrisBlock_grid(tBlock, result, blockTh, blockDataFile);
			
			// Apply haar cascade detection on median img
			detectAndDisplay(medianImg, dataDir, haarName);
			
			// Add grid to median img and Save it
			medianImg = drawGrid(medianImg, M, N, red);
			writeImage(medianImg, dataDir, medianImgName.c_str());
			
			// Close data file
			/*
			if (!fclose(dataFile)){
				// If file was closed successfully
				printf("Data file written:  \"%s\"\n",medianDataFile);
			}else {
				printf("[!!!] zzz Error writing data file: \"%s\"\n",medianDataFile);
			}*/
			//std::string blobImgName = rawDirName;
			//blobImgName.append("_blobImg.jpg");
			//writeImage(blobImg, dataDir, blobImgName.c_str());
		}
		
		// Set up data file for Processing program
		std::string filename = argv[1];
		
		size_t lastDirIndex = filename.find_last_of("/");
		std::string ogImgName = filename.substr(lastDirIndex+1, std::strlen(argv[1]));
		size_t lastindex = ogImgName.find_last_of(".");
		// name of the reference img file (without path and filetype extension)
		std::string rawname = ogImgName.substr(0, lastindex);
		
		std::string dataname = rawDirName + "_data.txt";
		const char* procDataFile = new char[std::strlen(dataDir) + dataname.length() + 1];
		std::strcat(const_cast<char*>(procDataFile),dataDir);
		std::strcat(const_cast<char*>(procDataFile),dataname.c_str());
		
		// Open data file, set to write
		//dataFile = fopen (procDataFile,"w");
		std::ofstream dataFile(procDataFile);
		/*
		// Analyze the difference mask
		std::vector<GridBlock> gridBlocks (analyzeMask(diffMask, 5, 5));
		// Mat object of the same size as input imgs
		// init color = black
		//Mat gridGradient(Size(diffMask.cols, diffMask.rows), CV_64FC4, black);
		//Mat gridGradient;
		Mat gridGradient(cmpImg.rows, cmpImg.cols, CV_8UC3, Scalar(0, 0, 0));
		//cvtColor(cmpImg, gridGradient, COLOR_BGR2GRAY);
		// Write data to data file and draw the gradient grid
		for(std::vector<GridBlock>::iterator it = gridBlocks.begin(); it != gridBlocks.end(); ++it) {
			// Write GridBlock data to file
			dataFile << *it;
			
			// Draw GridBlock gradient onto its grid area
			it->drawGrid(gridGradient, false);
		}
		// Write grid gradient img
		std::string gridGradientName = rawDirName;
		gridGradientName.append("_gridGradient.jpg");
		writeImage(gridGradient, dataDir, gridGradientName.c_str());
		*/
		/*
		//Mat tetrisBlock;
		Mat tetrisBlock(cmpImg.rows, cmpImg.cols, CV_8UC3, Scalar(0, 0, 0));
		//cvtColor(cmpImg, tetrisBlock, COLOR_BGR2GRAY);
		generateTetrisBlock(tetrisBlock, gridBlocks, blockTh);
		tetrisBlock = drawGrid(tetrisBlock, 5, 5, red);
		// Save tetris block img
		std::string tetrisBlockName = "FINAL_TETRIS_BLOCK.jpg";
		writeImage(tetrisBlock, dataDir, tetrisBlockName.c_str());
		*/
		
		// Write the original ref and cmp images to the data directory
		std::string regImgName = getRawFilename(argv[1]).append(".jpg");
		writeImage(refImg, dataDir, regImgName.c_str());
		std::string cmpImgName = getRawFilename(argv[2]).append(".jpg");
		writeImage(cmpImg, dataDir, cmpImgName.c_str());
		
		// Close data file
		/*
		if (!fclose(dataFile)){
			// If file was closed successfully
			printf("Data file written:  \"%s\"\n",procDataFile);
		}else {
			printf("[!!!] Error writing data file: \"%s\"\n",procDataFile);
		}*/
	}
    return 0;
}

void detectAndDisplay(Mat frame, const char* dataDir, const std::string filename ) {
	// Grayscale the image
	//Mat grayFrame;
	//cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
	//equalizeHist(grayFrame, grayFrame);
	
	std::vector<Rect> fullbody, lowerbody, upperbody;
	
	/* Detect Full Body */
	// detectMultiScale(const Mat& image, vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int flags=0, Size minSize=Size(), Size maxSize=Size())
	// 		scaleFactor: Parameter specifying how much the image size is reduced at each image scale.
	//		minNeighbors: Parameter specifying how many neighbors each candidate rectangle should have to retain it.
	fullbody_cascade.detectMultiScale(frame, fullbody, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30,30));
	for (size_t i = 0; i < fullbody.size(); i++) {
		rectangle(frame, fullbody[i], black,2,8,0);
		//rectangle(grayFrame, fullbody[i], red,2,8,0);
	}
	/* Detect Lower Body */
	lowerbody_cascade.detectMultiScale(frame, lowerbody, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30,30));
	for (size_t i = 0; i < lowerbody.size(); i++) {
		rectangle(frame, lowerbody[i], black,2,8,0);
		//rectangle(grayFrame, lowerbody[i], green,2,8,0);
	}
	/* Detect Upper Body */
	upperbody_cascade.detectMultiScale(frame, upperbody, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30,30));
	for (size_t i = 0; i < upperbody.size(); i++) {
		rectangle(frame, upperbody[i], black,2,8,0);
		//rectangle(grayFrame, upperbody[i], blue,2,8,0);
	}
	
	//frame = drawGrid(frame, 5, 5, white);
	//grayFrame = drawGrid(grayFrame, 5, 5, white);
	
	// Write both color and gray frames/images
	writeImage(frame, dataDir, filename);
	//writeImage(grayFrame, dataDir, "grayFrameNoEq.jpg");
	
	//const String windowName = "Detected Bodies";
	//displayImage(frame, windowName, 0);
	//displayImage(grayFrame, windowName, 0);
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
	 	const float blockTh, const char* blockfilename) {
	// Initialize outfile stream for storing the 0/1s of the tetris block
	std::ofstream blockfile(blockfilename);
	int row = 0; //tetrisBlock.size();
	int col = 0; //tetrisBlock[0].size();
	//for(std::vector<GridBlock>::iterator it = gridBlocks.begin(); it != gridBlocks.end(); ++it) {
	for (int i = 0; i < gridBlocks.size(); i++) {
		tetrisBlock[row][col] = (gridBlocks[i].isTetrisBlock(blockTh)) ? 1:0;
		std::cout << tetrisBlock[row][col] << " ";
		blockfile << tetrisBlock[row][col] << " ";
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
			blockfile << "\n";
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