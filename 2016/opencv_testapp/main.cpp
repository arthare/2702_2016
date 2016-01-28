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
    for(unsigned int x = 0; x < crapFiles.size(); x++ )
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
    int totalTime = 0;
    int passes = 0;
    for(unsigned int x=0; x < testFiles.size(); x++)
    {
        const string& strTxt = testFiles[x];
        string imgFile;

        ifstream in;
        in.open(strTxt.c_str());

        in>>imgFile;

        cout<<"processing with image: "<<x<<"...";

        Mat img = imread(imgFile.c_str(), CV_LOAD_IMAGE_COLOR);
        if(img.empty())
        {
          cout<<"NOT FOUND"<<endl;
        }
        else
        {
            resize(img, img, Size(640 / SCALE_FACTOR,480 / SCALE_FACTOR));
            int left;
            int right;
            int top;
            int bottom;
            in>> left;
            in>> top;
            in>> right;
            in>> bottom;

            if(left >= 0)
            {
                left /= SCALE_FACTOR;
                top /= SCALE_FACTOR;
                right /= SCALE_FACTOR;
                top /= SCALE_FACTOR;
            }

            int before = getms();

            pos pt = process(img, 0);
           int after = getms();
           int time = after - before;
           totalTime +=time;
           if (pt.x > left && pt.x < right && pt.y > top && pt.y < bottom)
            {
                cout<<"PASSED"<<endl;
                passes ++;
            }
            else if (left < 0 && pt.x == -1 && pt.y == -1)
            {
                 cout<<"PASSED"<<endl;
                passes ++;
            }
            else
            {
                cout<<"FAILED"<<endl;
            }

        }

    }
    cout << passes << " of " << testFiles.size() << endl;
    cout << "Total time = "<< totalTime << endl;
    cout << "Average time = " << totalTime / testFiles.size() << endl;
}
