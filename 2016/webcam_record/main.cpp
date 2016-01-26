#include "opencv2/opencv.hpp"
#include "../opencv_lib/2702_proc.h"

using namespace cv;
int main(int argc, char** argv)
{
    VideoCapture cap;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0))
        return -1;

    cap.set(CAP_PROP_CONTRAST, -0.75);
    namedWindow("window");
    for(;;)
    {
        Mat frame;
        cap >> frame;
        if( frame.empty() ) break; // end of video stream
        imshow("window", frame);
        waitKey(30);

    }
    // the camera will be closed automatically upon exit
    // cap.close();
    return 0;
}
