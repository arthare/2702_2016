#include "2702_proc.h"
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

using namespace std;

void process(Mat* img)
{

    namedWindow( "lena", CV_WINDOW_AUTOSIZE );
    imshow("lena", *img);
    //waitKey(0);
}
