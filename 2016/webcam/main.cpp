#include "opencv2/opencv.hpp"
#include "../opencv_lib/2702_proc.h"

using namespace cv;
#define SCALE_FACTOR 4
int getms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

    return s * 1000 + ms;
}


int main(int argc, char** argv)
{
    VideoCapture cap;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0))
        return -1;
    // Brighness at -0.75 gives great contrast with green LEDS and retroreflective markers without natural light. with natural light it preforms like monkey hurlage.
    cap.set(CAP_PROP_CONTRAST,-0.75);
    cap.set(CAP_PROP_FRAME_WIDTH,640/SCALE_FACTOR);
    cap.set(CAP_PROP_FRAME_HEIGHT,480/SCALE_FACTOR); // set the scale factor for faster processing


    int lastMs = getms();
    for(;;)
    {
        Mat frame;
        Mat frame2;
        cap >> frame;
        if( frame.empty() ) break; // end of video stream

        int start = getms();
        pos pt = process (frame, 0);




        int now = getms();
        printf("took %dms,Process took %dms, x = %d, y = %d, minVal = %ld\n", (now-lastMs),(now-start), pt.x, pt.y,pt.minVal);
        lastMs = now;

    }
    // the camera will be closed automatically upon exit
    // cap.close();
    return 0;
}
