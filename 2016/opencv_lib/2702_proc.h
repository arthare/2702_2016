#pragma once

#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
struct pos {
    int x;
    int y;
};
pos process(cv::Mat, int* args);
