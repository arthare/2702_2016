#include "opencv2/opencv.hpp"
#include "../opencv_lib/2702_proc.h"
#include <stdio.h>

using namespace cv;

/*int getms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return s * 1000 + ms;
}*/


int main(int argc, char** argv)
{
    VideoCapture cap;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0))
        return 0;
// Brighness at -0.75 gives great contrast with green LEDS and retroreflective markers without natural light. with natural light it preforms like monkey hurlage.
cap.set(CAP_PROP_CONTRAST,-0.75);
    //int lastMs = getms();
    for(;;)
    {
        Mat frame;
        Mat frame2;
        cap >> frame;
        if( frame.empty() ) break; // end of video stream
        process (&frame);
        waitKey(30);
        //int now = getms();
        //printf("took %dms\n", (now-lastMs));
        //lastMs = now;

    }
    // the camera will be closed automatically upon exit
    // cap.close();
    return 0;
}
