#include <iostream>

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

enum boxstate {
    TOP_LEFT,
    BOTTOM_RIGHT,
};
boxstate g_boxState = TOP_LEFT;

int ixCurrentImage = 0;
string currentFile;
int g_left;
int g_top;
int g_right;
int g_bottom;

string myReplace(const std::string& input, const std::string& oldStr, const std::string& newStr){

  string str = input;
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos){
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
  return str;
}

string textDestination(const string& badPath)
{
    return myReplace(badPath, "images/unlabeled/", "");
}
string imageDestination(const string& badPath)
{
    return myReplace(badPath, "/unlabeled", "");
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
     if  ( event == EVENT_LBUTTONDOWN )
     {
        switch(g_boxState) {
            case TOP_LEFT:
                g_left = x;
                g_top = y;
                g_boxState = BOTTOM_RIGHT;
                break;
            case BOTTOM_RIGHT:
                g_right = x;
                g_bottom = y;

                stringstream ssFile;
                ssFile<<currentFile;
                ssFile<<".txt";

                ofstream out;
                out.open(textDestination(ssFile.str()).c_str());
                out<<imageDestination(currentFile)<<endl;
                out<<min(g_left, g_right)<<endl;
                out<<min(g_top, g_bottom)<<endl;
                out<<max(g_left, g_right)<<endl;
                out<<max(g_top, g_bottom)<<endl;
                out.close();

                g_boxState = TOP_LEFT;
                ixCurrentImage++;
                break;
        }
     }
     else if(event == EVENT_MBUTTONDOWN)
     {
        // right mouse button means no target
        stringstream ssFile;
        ssFile<<currentFile;
        ssFile<<".txt";

        ofstream out;
        out.open(textDestination(ssFile.str()).c_str());
        out<<imageDestination(currentFile)<<endl;
        out<<-1<<endl;
        out<<-1<<endl;
        out<<-1<<endl;
        out<<-1<<endl;
        out.close();

        g_boxState = TOP_LEFT;
        ixCurrentImage++;
     }
     else if(event == EVENT_MOUSEMOVE)
     {
        switch(g_boxState) {
            case TOP_LEFT:
                g_left = g_right = x;
                g_top = g_bottom = y;
                break;
            case BOTTOM_RIGHT:
                g_right = x;
                g_bottom = y;
                break;
        }
     }
}
void filterOutCrap(vector<string>& crapFiles)
{
    for(unsigned int x = 0; x < crapFiles.size(); x++ )
    {
        string file = crapFiles[x];
        if(file.find(".png") == string::npos || file.find(".txt") != string::npos)
        {
            cout<<"Throwing out "<<file<<endl;
            crapFiles[x] = crapFiles.back();
            crapFiles.pop_back();
            x--;
        }
    }
}
int main()
{
    vector<string> testFiles;
    getdir("../testdata/images/unlabeled/", testFiles);
    filterOutCrap(testFiles);
    /// Create Windows
    namedWindow("window");

    cout<<"there are "<<testFiles.size()<<" files to label"<<endl;

    int ixLastImage = -1;
    Mat img;

    setMouseCallback("window", CallBackFunc, NULL);

    while(true)
    {
        if(ixCurrentImage != ixLastImage)
        {
            cout<<"going to load image "<<testFiles[ixCurrentImage]<<endl;
            currentFile = testFiles[ixCurrentImage];
            img = imread(testFiles[ixCurrentImage].c_str(), CV_LOAD_IMAGE_COLOR);
            ixLastImage = ixCurrentImage;
            cout<<"Showing image "<<ixCurrentImage<<"/"<<testFiles.size()<<endl;
        }

        Mat imgCopy = img.clone();
        rectangle(imgCopy, Point(g_left, g_top), Point(g_right+1,g_bottom+1), Scalar(255,255,255), 2);
        imshow("window", imgCopy);
        waitKey(20);
    }
}
