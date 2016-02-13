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
    static const int ARG_COUNT = 15;
    settings()
    {
        args[0] = 5;
        args[1] = 163;
        args[2] = 111;
        args[3] = 35;
        args[4] = 112;
        args[5] = 2;
        args[6] = 183;
        args[7] = 0;
        args[8] = 0;
        args[9] = 0;
        args[10] = 0;
        args[11] = 255;
        args[12] = 255;
        args[13] = 255;
        args[14] = 25500;
    }

    settings(int *args)
    {
        for(int i = 0;i < ARG_COUNT; ++i)
        {
            this->args[i] = args[i];
        }
    }

    int match_method(){return args[0];}
    void set_match_method(int m){args[0] = m;}
    const int edgeDetectParam1(){return args[1];}
    const int edgeDetectParam2(){return args[2];}
    const float stdDevGoal(){return args[3];}
    const float templatePixelsPerInch(){return args[4];}
    const int templateLineThickness(){return args[5];}
    const int tooBrightPixelValues(){return args[6];}
    const int tooDimPixelValue(){return args[7];}
    const int hMin(){return args[8];}
    const int hMax(){return args[9];}
    const int sMin(){return args[10];}
    const int sMax(){return args[11];}
    const int vMin(){return args[12];}
    const int vMax(){return args[13];}
    const int greenMultiplyer(){return args[14]*(2.0f/255.0f);}

    int args[ARG_COUNT];

};


pos process(cv::Mat, settings s);

int getms (void);

int getdir (std::string dir, std::vector<std::string> &files);

bool fileExists (const std::string& name);


