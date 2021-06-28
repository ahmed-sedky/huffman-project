#include <opencv2/opencv.hpp>
using namespace cv;
#include <vector>
#include <fstream>
using namespace std;

int main()
{
    Mat grayImage = imread("NORMAL2-IM-1440-0001.pgm",IMREAD_GRAYSCALE);

    // First convert the image to grayscale.
    

    // Open the file in write mode.
    ofstream outputFile;
    outputFile.open("Pp40.txt");

    // Iterate through pixels.
    outputFile<<grayImage.rows<<" "<<grayImage.cols<<endl;
    std::string s;
    for (int r = 0; r < grayImage.rows; r++)
    {
        for (int c = 0; c < grayImage.cols; c++)
        {
            int pixel = grayImage.at<uchar>(r,c);
            s += char(pixel);
        }
    }
    outputFile << s;
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PXM_BINARY);
    imwrite("hossamTest.pgm",grayImage,compression_params);
    // Close the file
    outputFile.close();
    return 0;
    
}