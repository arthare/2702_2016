#pragma once
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
struct pos {
    int x;
    int y;
};
pos process(cv::Mat, int* args);

int getms (void);

int getdir (std::string dir, std::vector<std::string> &files);

bool fileExists (const std::string& name);
