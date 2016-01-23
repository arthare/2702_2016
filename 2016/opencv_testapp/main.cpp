#include <iostream>
#include "2702_proc.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main()
{
    Mat img = imread("../opencv_lib/lena.jpg", CV_LOAD_IMAGE_COLOR);
    if(img.empty())
    {
      cout<<"Image was empty"<<endl;
      return 0;
    }

    process(&img);
    return 0;
}
