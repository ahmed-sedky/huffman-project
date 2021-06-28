#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <string>
using namespace std;
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
    cin>>FileName;
    inFile.open(FileName ,std::ios::binary|std::ios::in); //open the file have the new codes of pixel values
    if (!inFile)
    {
        std::cerr << "can't open the file: ";
    }
    std::string line;
    std::map<std::string,char> myMab;
    getline(inFile, line); // read the first line : P5
    std::string version =line;

    int pos = inFile.tellg();
    char comm;
    inFile.read(&comm, 1);
    inFile.seekg(pos);
    std::string comment ="";
    if (comm == '#')
    {
        getline(inFile, line); // read the second line : comment
        comment =line;
        cout << "Comment : " << line << endl;
    }
    getline(inFile, line); //to get the width and height
    int index = line.find(" ");
    int height = std::stoi(line.substr(0, index));
    int width = std::stoi(line.substr(index + 1));
    getline(inFile,line); //read the maximum value
    std::string max_value=line;
    while(getline(inFile,line))
    {
    int index = line.find(" ");
    char pixelValues=std::stoi(line.substr(0,index));
    std::string newCode=(line.substr(index+1));
    myMab[newCode]= pixelValues;
    }
    std::ifstream inFile2;
    std::cout<<"enter the compressed file: ";
    std::string fileName;
    cin>>fileName;
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
    code_pixels += decimal_to_binary(reinterpret_cast<unsigned char &>(input_text[i]));
    }
    code_pixels = code_pixels.substr(0,code_pixels.size()-padding);
    ofstream of;
     of.open(FileName+"decoded.txt", std::ios::binary|std::ios::out);//
    of << code_pixels;
    std::string decode="";
    for (size_t i = 0; i < code_pixels.size(); i++)
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
    std::ofstream outputFile;
    outputFile.open(FileName+"decoded.pgm",std::ios::binary|std::ios::out);
    outputFile<<version<<endl;
    if(comment!="")
      outputFile<<comment<<endl;
    outputFile << width << " " << height << endl;
    outputFile<<max_value<<endl;
    outputFile << decode;

}