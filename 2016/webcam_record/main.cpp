#include "opencv2/opencv.hpp"
#include "../opencv_lib/2702_proc.h"
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>


using namespace std;
using namespace cv;

Mat imgCurrent;
int imgSaved = 0;

string makeFileName(int img)
{
    stringstream ssSave;
    ssSave<<"../testdata/images/auto-"<<imgSaved<<".png";
    return ssSave.str();
}
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if(event == EVENT_MBUTTONDOWN)
    {
        string fileName = makeFileName(imgSaved);
        cout<<"trying to save to "<<fileName<<endl;
        imwrite( fileName.c_str(), imgCurrent );
        imgSaved++;
    }
}

bool fileExists (const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

int main(int argc, char** argv)
{
    while(fileExists(makeFileName(imgSaved)))
    {
        imgSaved++;
    }

    VideoCapture cap;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0))
        return -1;

    cap.set(CAP_PROP_CONTRAST, -0.75);
    cap.set(CAP_PROP_BRIGHTNESS, -0.75);
    namedWindow("window");
    setMouseCallback("window", CallBackFunc, NULL);
    for(;;)
    {
        cap >> imgCurrent;
        if( imgCurrent.empty() ) break; // end of video stream
        pos mypos = process(imgCurrent, 0);

        Mat imgWithCircle = imgCurrent.clone();
        circle(imgWithCircle, Point(mypos.x, mypos.y), 20, Scalar(255, 0, 0));
        imshow("window", imgWithCircle);
        waitKey(30);


    }
    // the camera will be closed automatically upon exit
    // cap.close();
    return 0;
}
