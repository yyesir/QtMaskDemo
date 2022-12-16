#include "FaceDetectionAlgo.h"
#include "facedetect-dll.h"
#include "opencv2/imgproc/types_c.h"
#include <assert.h>

#define DETECT_BUFFER_SIZE 49156

int FaceDetectionAlgo::detect(const cv::Mat& image, vector<cv::Rect> &faceRects )
{
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);

	unsigned char * buf = (unsigned char*) malloc(DETECT_BUFFER_SIZE);
	memset(buf, 0, DETECT_BUFFER_SIZE);
	int* result = facedetect_frontal_surveillance(buf, (unsigned char*)(grayImage.ptr(0)), grayImage.cols, grayImage.rows
		, grayImage.step, 1.2f, 3,  40, 0);
	if(result == NULL) {
		free(buf);
		return 0;
	}

    vector<cv::Rect> rects;
	for(int i = 0; i < (result ? *result : 0); i++) {
		short * p = ((short*)(result+1))+6*i;
        cv::Rect rect;
        rect.x = p[0];
        rect.y = p[1];
        rect.width = p[2];
        rect.height = p[3];
		rects.push_back(rect);
	}
	free(buf);

	if (rects.empty()) {
		return 0;
	}

	//根据头像大小进行排序
	for(auto srcIt = rects.begin(); srcIt != rects.end(); ++srcIt) {
        cv::Rect &rect = *srcIt;
		if(faceRects.size() <= 0) {
			faceRects.push_back(rect);
		} else {
			for(auto dstIt = faceRects.begin(); dstIt != faceRects.end(); ++dstIt) {
                cv::Rect& tempRect = *dstIt;
                if(tempRect.width <= rect.width) {
					faceRects.insert(dstIt, rect);
					break;
				} else if(dstIt == faceRects.end()) {
					faceRects.push_back(rect);
				}
			}
		}
	}

	return faceRects.size();
}
