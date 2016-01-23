#include <iostream>
#include "2702_proc.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;
using namespace cv;


int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL) {

        stringstream ss;
        ss<<dir<<dirp->d_name;
        files.push_back(ss.str());
    }
    closedir(dp);
    return 0;
}

bool fileExists (const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}
int main()
{
    vector<string> testFiles;
    getdir("../testdata/", testFiles);


    /// Create Windows
    namedWindow("window");

    const int NUM_ARGS = 6;
    int args[NUM_ARGS] = {0};

    for(int x = 0; x < NUM_ARGS; x++)
    {
      char TrackbarName[50];
      sprintf( TrackbarName, "Arg %d", x );
      createTrackbar( TrackbarName, "window", &args[x], 255 );
    }

    for(int x = 0; x < testFiles.size(); x++)
    {
        if(fileExists(testFiles[x]) && testFiles[x].find("/.") == std::string::npos)
        {
            cout<<"about to load "<<testFiles[x]<<endl;
            Mat img = imread(testFiles[x].c_str(), CV_LOAD_IMAGE_COLOR);
            if(img.empty())
            {
              cout<<"Image "<<testFiles[x]<<" was empty"<<endl;
              return 0;
            }


            process(&img, args);

            waitKey(0);
        }
    } // end loop through images in dir


}
