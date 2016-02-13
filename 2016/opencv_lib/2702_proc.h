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
    enum ARG_TYPE
    {
        TEMPLATE_ALGO,
        STDDEV_STRETCH,
        TOOBRIGHT,
        TOODIM,

        ARG_COUNT,
    };

    settings()
    {
        args[TEMPLATE_ALGO] = 4;
        args[STDDEV_STRETCH] = 35;
        args[TOOBRIGHT] = 194;
        args[TOODIM] = 12;
    }

    settings(int *args)
    {
        for(int i = 0;i < ARG_COUNT; ++i)
        {
            this->args[i] = args[i];
        }
    }

    void report(std::ostream& os, const char* psz) const
    {
        os<<psz<<std::endl;
        for(int x = 0; x < ARG_COUNT; x++)
        {
            os<<"args["<<x<<"] = "<<args[x]<<std::endl;
        }
    }


    int match_method() const { return args[TEMPLATE_ALGO];}
    void set_match_method(int m) {args[TEMPLATE_ALGO] = m;}

    const float stdDevGoal() const {return args[STDDEV_STRETCH];}
    const int tooBrightPixelValues() const {return args[TOOBRIGHT];}
    const int tooDimPixelValue() const {return args[TOODIM];}

    int args[ARG_COUNT];

};


pos process(cv::Mat, settings s);

int getms (void);

int getdir (std::string dir, std::vector<std::string> &files);

bool fileExists (const std::string& name);


