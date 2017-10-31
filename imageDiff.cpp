#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

/** Function Headers */
Mat imgDiff_img(Mat refImg, Mat cmpImg, float threshold);
Mat imgDiff_mask(Mat refImg, Mat cmpImg, float threshold);

Mat imgDiff_img(Mat refImg, Mat cmpImg, float threshold) {
	Mat diffImg;
	// absdiff(input array 1, input array 2, output array)
	absdiff(refImg, cmpImg, diffImg);
    Mat foregroundMask = Mat::zeros(diffImg.rows, diffImg.cols, CV_8UC1);
	
    //float threshold = 30.0f;
    float dist;

    for(int j=0; j<diffImg.rows; ++j) {
        for(int i=0; i<diffImg.cols; ++i) {
            Vec3b pix = diffImg.at<Vec3b>(j,i);
			
            dist = (pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);
            dist = sqrt(dist);
            
			if(dist>threshold) {
                foregroundMask.at<unsigned char>(j,i) = 255;
            }
        }
	}
	
	return diffImg;
}

Mat imgDiff_mask(Mat refImg, Mat cmpImg, float threshold) {
	Mat diffImg;
	absdiff(refImg, cmpImg, diffImg);
    Mat foregroundMask = Mat::zeros(diffImg.rows, diffImg.cols, CV_8UC1);
	
    float threshold_og = 30.0f;
	float threshold_a = 20.0f;
	float threshold_b = 10.0f;
	// float threshold = 15.0f;
    float dist;

    for(int j=0; j<diffImg.rows; ++j) {
        for(int i=0; i<diffImg.cols; ++i) {
            Vec3b pix = diffImg.at<Vec3b>(j,i);

            dist = (pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);
            dist = sqrt(dist);

            if(dist>threshold) {
                foregroundMask.at<unsigned char>(j,i) = 0;
            }else{
            	foregroundMask.at<unsigned char>(j,i) = 255;
            }
        }
	}
	
	return foregroundMask;
}