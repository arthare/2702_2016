#pragma once

#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
struct pos {
    int x;
    int y;
    long minVal;
    long maxVal;
};
pos process(cv::Mat, int* args);
