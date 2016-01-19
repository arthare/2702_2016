#include "2702_proc.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

using namespace std;

void speak()
{
    Mat img = imread("/home/rebels/code/2016/opencv_lib/lena.jpg", CV_LOAD_IMAGE_COLOR);
    if(img.empty())
    {
      cout<<"Image was empty"<<endl;
      return;
    }
    namedWindow( "lena", CV_WINDOW_AUTOSIZE );
    imshow("lena", img);
    waitKey(0);
}
