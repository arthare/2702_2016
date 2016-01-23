#include <iostream>
#include "2702_proc.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>

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
void filterOutCrap(vector<string>& crapFiles)
{
    for(int x = 0; x < crapFiles.size(); x++ )
    {
        string file = crapFiles[x];
        if(file.find(".txt") == string::npos)
        {
            crapFiles[x] = crapFiles.back();
            crapFiles.pop_back();
            x--;
        }
    }
}
int main()
{
    vector<string> testFiles;
    getdir("../testdata/", testFiles);


    filterOutCrap(testFiles);

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


    int ixLastImage = -1;
    Mat img;
    while(true)
    {
        int ixCurrentImage = args[5];
        ixCurrentImage = min((int)ixCurrentImage, (int)testFiles.size());
        if(ixCurrentImage != ixLastImage)
        {
            cout<<" they want us to load "<<ixCurrentImage<<", which is "<<testFiles[ixCurrentImage]<<endl;
            string strTxt = testFiles[ixCurrentImage];
            string imgFile;
            ifstream in;
            in.open(strTxt.c_str());
            in>>imgFile;
            cout<<"which leads us to imgfile = "<<imgFile<<endl;
            img = imread(imgFile.c_str(), CV_LOAD_IMAGE_COLOR);
            if(img.empty())
            {
              cout<<"Image "<<imgFile<<" was empty"<<endl;
            }

            ixLastImage = ixCurrentImage;
        }

        cout<<"processing with image: "<<ixCurrentImage<<endl;
        process(&img, args);

        waitKey(30);
    }
}
