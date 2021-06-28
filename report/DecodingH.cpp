#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <string>
std::string decimal_to_binary(int in)
{
	std::string temp = "";
	std::string result = "";
	while (in)
	{
		temp += ('0' + in % 2);
		in /= 2;
	}
	result.append(8 - temp.size(), '0');//append '0' ahead to let the result become fixed length of 8
	for (int i = temp.size() - 1; i >= 0; i--)												
	{
		result += temp[i];
	}
	return result;
}
int main()
{
    std::ifstream inFile;
    std::cout<<"enter the code file: ";
    std::string FileName;
    std::cin>>FileName;
    inFile.open(FileName ,std::ios::binary|std::ios::in); //open the file have the new codes of pixel values
    if (!inFile)
    {
        std::cerr << "can't open the file: ";
    }
    std::string line;
    std::map<std::string,char> myMab;
    getline(inFile,line);
    int index = line.find(" ");
    int width =std::stoi(line.substr(0,index));
    int height =std::stoi(line.substr(index+1));
    while(getline(inFile,line))
    {
    int index = line.find(" ");
    unsigned char pixelValues=std::stoi(line.substr(0,index));
    std::string newCode=(line.substr(index+1));
    myMab[newCode]= pixelValues;
    }
    std::ifstream inFile2;
    std::cout<<"enter the compressed file: ";
    std::string fileName;
    std::cin>>fileName;
    inFile2.open(fileName ,std::ios::binary|std::ios::in);  //open the compressed file
    if (!inFile2)
    {
        std::cerr << "can't open the file";
    }
    std::string Line;
    getline(inFile2,Line); //read the padding
    int padding =Line[0];
    std::string input_text;
    std::string code_pixels="";
    while(getline(inFile2,Line)) //read the compressed file
    {
    input_text += (Line + "\n");
    }
    input_text.pop_back();
    for(int i=0 ;i<input_text.size();i++)
    {
    code_pixels += decimal_to_binary(reinterpret_cast<unsigned char &>(input_text[i]));//turn the comprissed file from decimal to binary
    }
    code_pixels = code_pixels.substr(0,code_pixels.size()-padding);
    std::string decode="";
    for (size_t i = 0; i < code_pixels.size(); i++)//search for the code and its equaivalent pixel value in a string decode
    { 
        std::string total_code="";
        int start = i;
        while(myMab.find(total_code) == myMab.end())
        {
            total_code =code_pixels.substr(start,total_code.size()+1);
            i++;
        }
        i--;
        decode += myMab [total_code];
    }
    std::ofstream outputFile;         //print the pixel values in an outputFile
    outputFile.open(FileName+"decoded",std::ios::binary|std::ios::out);
    outputFile<< decode;
    Mat outputImage = Mat::zeros(height,width, CV_8UC1);
    int pixel_index=0;
    for (int r = 0; r < outputImage.rows; r++)// write the pixel values as matrix to use imwrite which in it's documentation needs a matrix
    {
        for (int c = 0; c < outputImage.cols; c++)
        {
            outputImage.at<uchar>(r,c)= reinterpret_cast<unsigned char &>(decode[pixel_index++]);
        }
    }
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PXM_BINARY);
    imwrite("pp42.pgm",outputImage,compression_params);
}