#ifndef FACEDETECTION_H
#define FACEDETECTION_H

#include "opencv2/opencv.hpp"
#include <vector>

using std::vector;

class FaceDetectionAlgo
{
public:
    static int detect(const cv::Mat& image, vector<cv::Rect>& faceRects);
};

#endif

