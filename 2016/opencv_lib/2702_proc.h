#pragma once
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <iostream>
//#include <opencv2/imgproc/imgproc.hpp>
struct pos {
    int x;
    int y;
    long minVal;
    long maxVal;
};

struct settings{
    settings()
        : match_method(5)
        , edgeDetectParam1(163)
        , edgeDetectParam2(111)
        , stdDevGoal(35)
        , templatePixelsPerInch(1.12f)
        , templateLineThickness(2)
        , tooBrightPixelValues(183)
        , tooDimPixelValue(0)
        , hMin(0)
        , hMax(0)
        , sMin(0)
        , sMax(255)
        , vMin(255)
        , vMax(255)
        , greenMultiplyer(25500)
    {
    }

    settings(int *args)
        : match_method(args[0])
        , edgeDetectParam1(args[1])
        , edgeDetectParam2(args[2])
        , stdDevGoal(args[3])
        , templatePixelsPerInch(args[4]/100.f)
        , templateLineThickness(args[5])
        , tooBrightPixelValues(args[6])
        , tooDimPixelValue(args[7])
        , hMin(args[8])
        , hMax(args[9])
        , sMin(args[10])
        , sMax(args[11])
        , vMin(args[12])
        , vMax(args[13])
        , greenMultiplyer(args[14]*(2.0f/255.0f))
        {
        }

    int match_method;
    const int edgeDetectParam1;
    const int edgeDetectParam2;
    const float stdDevGoal;
    const float templatePixelsPerInch;
    const int templateLineThickness;
    const int tooBrightPixelValues;
    const int tooDimPixelValue;
    const int hMin;
    const int hMax;
    const int sMin;
    const int sMax;
    const int vMin;
    const int vMax;
    const float greenMultiplyer;
};


pos process(cv::Mat, settings s);

const int ARG_COUNT = 15;


int getms (void);

int getdir (std::string dir, std::vector<std::string> &files);

bool fileExists (const std::string& name);


