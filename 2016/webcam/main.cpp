#include "opencv2/opencv.hpp"
#include "../opencv_lib/2702_proc.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <iostream>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <fstream>

using namespace cv;
using namespace std;
#define SCALE_FACTOR 4

string makeFileName(int img, pos pt)
{
    stringstream ssSave;
    ssSave<<"pi-img-"<<img<< "_"<<pt.x<<","<< pt.y <<".png";
    return ssSave.str();
}

int main(int argc, char** argv)
{

    string strTargetIp = "10.27.2.21";
    int imgDelay = 5000;
    ifstream in;
    in.open("ip.txt");

    if(!in.eof() && !in.fail())
    {
        in>>strTargetIp;
        cout<<"ip.txt overrode our target IP to "<<strTargetIp<<endl;
        if(!in.eof() && !in.fail())
        {
            in>>imgDelay;
        }
    }
    in.close();

    VideoCapture cap;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0))
        return -1;
    // Brighness at -0.75 gives great contrast with green LEDS and retroreflective markers without natural light. with natural light it preforms like monkey hurlage.
    cap.set(CAP_PROP_CONTRAST,-0.75);
    cap.set(CAP_PROP_FRAME_WIDTH,640/SCALE_FACTOR);
    cap.set(CAP_PROP_FRAME_HEIGHT,480/SCALE_FACTOR); // set the scale factor for faster processing


     struct sockaddr_in myaddr;
   int sock;
   int result;

   memset(&myaddr, 0, sizeof(myaddr));
   myaddr.sin_family=AF_INET;

   myaddr.sin_addr.s_addr=htonl(INADDR_ANY);

   myaddr.sin_port=htons(0);

   if((sock=socket(AF_INET, SOCK_DGRAM, 0))<0) {
      cout <<"Failed to create socket"<<endl;
      exit(EXIT_FAILURE);
   }

   if(bind(sock,( struct sockaddr *) &myaddr, sizeof(myaddr))<0) {
      cout <<"bind failed"<<endl;
      exit(EXIT_FAILURE);
   }
    cout << "bind sucess"<<endl;

   inet_pton(AF_INET,strTargetIp.c_str(),&myaddr.sin_addr.s_addr);

   myaddr.sin_port=htons(2702);


    int startMs = getms();
    int lastMs = startMs;
    int lastSave = startMs;
    for(;;)
    {
        Mat frame;
        Mat frame2;
        cap >> frame;
        if( frame.empty() ) break; // end of video stream

        int start = getms();


        settings s(NO_UI);
        pos pt = process (frame, s);

        const int msSinceSave=getms()-lastSave;
        if(msSinceSave > imgDelay)
        {
                string fileName = makeFileName(getms(), pt);
                cout<<"trying to save to "<<fileName<<endl;

                imwrite( fileName.c_str(), frame );
                lastSave = getms();
        }


        int now = getms();
        printf("took %dms,Process took %dms, x = %d, y = %d, minVal = %ld\n", (now-lastMs),(now-start), pt.x, pt.y,pt.minVal);
        lastMs = now;

        networkData dataToSend;

        dataToSend.x = pt.x;
        dataToSend.y = pt.y;

        result = sendto(sock, &dataToSend, sizeof(dataToSend), 0, (const sockaddr*) & myaddr, sizeof(myaddr));
        cout << "sent : " << result << " bytes of data" << endl;
    }
    // the camera will be closed automatically upon exit
    // cap.close();
    return 0;
}
