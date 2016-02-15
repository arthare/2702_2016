#pragma once
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <iostream>

struct pos {
    int x;
    int y;
    long minVal;
    long maxVal;
};

static const bool NO_UI = false;
static const bool WITH_UI = true;

struct settings{
    enum ARG_TYPE
    {
        TEMPLATE_ALGO,
        TOOBRIGHT,
        TOODIM,

        ARG_COUNT,
    };

    settings(const settings& other)
    {
        std::cout<<"copy constructor"<<std::endl;
        this->showUI = other.showUI;
        for(int x = 0;x < ARG_COUNT; x++)
        {
            this->args[x] = other.args[x];
        }
    }

    settings(bool showUI)
    {
        args[TEMPLATE_ALGO] = 5;
        args[TOOBRIGHT] = 194;
        args[TOODIM] = 16;
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

    int match_method() const { return args[TEMPLATE_ALGO];}
    void set_match_method(int m) {args[TEMPLATE_ALGO] = m;}


    const int tooBrightPixelValue() const {return args[TOOBRIGHT];}
    const int tooDimPixelValue() const {return args[TOODIM];}

    int args[ARG_COUNT];
    bool showUI;
};

pos process(cv::Mat, settings& s);
int getms();
int getdir (std::string dir, std::vector<std::string> &files);
bool fileExists(const std::string& name);
