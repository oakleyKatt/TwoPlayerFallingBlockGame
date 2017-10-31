#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

/** Function Headers */
Mat detectBlobs(Mat refImg, Mat cmpImg, float threshold);
Mat detectBlobs_simple(Mat refImg, Mat cmpImg, float threshold);

Mat detectBlobs(Mat img, float threshold) {

	

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;
 
	// Change thresholds
	params.minThreshold = 10;
	params.maxThreshold = 200;
 
	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 1500;
 
	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1;
 
	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87;
 
	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01;
 
 	// Set up detector with params
 	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	// SimpleBlobDetector::create creates a smart pointer. 
	// So you need to use arrow ( ->) instead of dot ( . )
	// detector->detect( im, keypoints);
	
	// Detect blobs.
	std::vector<KeyPoint> keypoints;
	detector->detect(img, keypoints);
 
	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	Mat img_with_keypoints;
	drawKeypoints(img, keypoints, img_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
 
	return img_with_keypoints;
	
}

Mat detectBlobs_simple(Mat img, float threshold) {
	// Set up the detector with default parameters.
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create();
 
	// Detect blobs.
	std::vector<KeyPoint> keypoints;
	detector->detect(img, keypoints);
 
	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	Mat img_with_keypoints;
	drawKeypoints(img, keypoints, img_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
 
	return img_with_keypoints;
}

