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

static const bool NO_UI = false;
static const bool WITH_UI = true;

struct settings{
    static const int ARG_COUNT = 15;

    settings(bool showUI)
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
        this->showUI = showUI;
    }

    settings(int *args, bool showUI)
    {
        for(int i = 0;i < ARG_COUNT; ++i)
        {
            this->args[i] = args[i];
        }
        this->showUI = showUI;
    }

    void report(std::ostream& os, const char* psz) const
    {
        os << psz << std::endl;
        for(int x = 0; x < ARG_COUNT; x++)
        {
            os << "args[" << x << "] = " << args[x] << std::endl;
        }
    }

    void copyArgs(int *args)
    {
        memcpy(this->args, args, sizeof(this->args));
    }

    int match_method() const { return args[0]; }
    void set_match_method(const int m) { args[0] = m; }
    const int edgeDetectParam1() const { return args[1]; }
    const int edgeDetectParam2() const { return args[2]; }
    const float stdDevGoal() const { return args[3]; }
    const float templatePixelsPerInch() const { return args[4]; }
    const int templateLineThickness() const { return args[5]; }
    const int tooBrightPixelValue() const { return args[6]; }
    const int tooDimPixelValue() const { return args[7]; }
    const int hMin() const { return args[8]; }
    const int hMax() const { return args[9]; }
    const int sMin() const { return args[10]; }
    const int sMax() const { return args[11]; }
    const int vMin() const { return args[12]; }
    const int vMax() const { return args[13]; }
    const int greenMultiplyer() const { return args[14]*(2.0f/255.0f); }

    int args[ARG_COUNT];
    bool showUI;
};

pos process(cv::Mat, settings s);
int getms();
int getdir (std::string dir, std::vector<std::string> &files);
bool fileExists(const std::string& name);
