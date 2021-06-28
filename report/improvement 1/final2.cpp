#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include "cxxopts.hpp"
#include "catch.hpp"
#include <opencv2/opencv.hpp>
using namespace std;
struct Node
{
    int freq;
    Node* right;
    Node* left;
    unsigned char pixel_values;
    std::string code;
};
struct compareHuffNode                    // to help rearrangement of the priorty queue according to the frequency
{
    bool operator()(Node* left, Node* right)
    {
        return (left->freq > right->freq);
    }
};
std::string array1[256] = { "" };
void traverse(Node* root, std::string code = "") {                   //create the new code of each pixel value
    if (root->right == nullptr && root->left == nullptr){
        array1[root->pixel_values]=code;
    }
    else {
        traverse(root->left, code + "0");
        traverse(root->right, code + "1");
    }
}
int binary_to_decimal(std::string code_pixel){
    int result =0;
    for(int i=0; i<code_pixel.size();++i)
        result =(result<<1)+code_pixel[i]-'0';
    return result;
}
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
void decoding(std::string arg,std::string arg2){

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
void encoding(std::string arg){
  int array_freq[256]; //initialize all the array of frequencies by zero
    for (int i = 0; i <= 255; i++)
    {
        array_freq[i] = 0;
    }

    std::ifstream inFile; //read the pgm file.txt line by line and store it in a vector called bytes and calcu;ate the frequency of each pixel values
    std::string line;
    std::string input_text;
    std::cout<<"enter the PGM file: "<<endl;
    std::string FileName;
    std::cin >> FileName;
    inFile.open(FileName, std::ios::binary | std::ios::in); //
    if (!inFile)
    {
        std::cerr << "can't open the file";
    }
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
    std::vector<unsigned char> bytes;
    while (getline(inFile, line))
        input_text += (line + "\n");
    input_text.pop_back();
    for (size_t i = 0; i < input_text.size(); ++i)
    {
        bytes.push_back(input_text[i]);
        array_freq[bytes.back()]++;
    }
    //build a priorty queue that arrange it self according to the frequency of each node from the least to the most
    std::priority_queue<Node *, std::vector<Node *>, compareHuffNode> pq;
    for (int j = 0; j <= 255; ++j)
    { //create a newnode if for only the pixel value"j" is exist in the pgm file
        if (array_freq[j] != 0)
        {
            Node *newNode = new Node{array_freq[j], nullptr, nullptr, j, ""};
            pq.push(newNode);
        }
    }
    while (pq.size() != 1)
    { //build the haffman tree
        Node *NewNode = new Node{0, nullptr, nullptr, 'A', ""};
        NewNode->left = pq.top();
        NewNode->freq += pq.top()->freq;
        pq.pop();
        NewNode->right = pq.top();
        NewNode->freq += pq.top()->freq;
        pq.pop();
        pq.push(NewNode);
    }

    traverse(pq.top(), "");
    std::string code_pixel = "";
    for (int y = 0; y <= 255; y++)
    {
        if (array1[y] != "")
            std::cout << y << " " << array1[y] << std::endl;
    }
    std::cout << width << " " << height << endl;
    ofstream codeFile;
    codeFile.open("code" + FileName + ".txt", std::ios::binary | std::ios::out); //
    if (!codeFile)
    {
        std::cerr << "can't open the file";
    }
    codeFile<<version<<endl;
    if(comment!="")
     codeFile<<comment<<endl;
    codeFile << width << " " << height << endl;
    codeFile<<max_value<<endl;
    for (int y = 0; y <= 255; y++)
    {
        if (array1[y] != "")
            codeFile << y << " " << array1[y] << std::endl;
    }
    //because any string consists of signed charecters
    // and we need to convert it to unsigned char beacuse the index of array1 couldn't be negative value
    for (int i = 0; i < input_text.size(); i++)
        code_pixel += array1[reinterpret_cast<unsigned char &>(input_text[i])];
    ofstream outputFile;
    outputFile.open("" + FileName + "compressed.txt", std::ios::binary | std::ios::out); //
    if (!outputFile)
    {
        std::cerr << "can't open the file";
    }
    //we can write only bytes to file not bits so if we had only 6 bits we will need padding of two bits to have a full byte to write
    char padding = 8 - (code_pixel.size() % 8);
    if (code_pixel.size() % 8 == 0)
        padding = 0;
    outputFile.write(&padding, 1);
    outputFile << endl;
    for (int d = 0; d < code_pixel.size() / 8; d++)
    { //d*8 is the start of the index and 8 is length of the substring
        std::string CodePixel = code_pixel.substr(d * 8, 8);
        char Pixels = binary_to_decimal(CodePixel); // we configure each 8 bytes in one byte
        outputFile.write(&Pixels, 1);               //1 is the number of bytes we will write
    }
    if (code_pixel.size() % 8 != 0)
    {
        std::string codePixel = code_pixel.substr((code_pixel.size() / 8) * 8);
        for (int i = 0; i < padding; i++)
            codePixel += "0";
        char Pixels = binary_to_decimal(codePixel);
        outputFile.write(&Pixels, 1);
    }
    float comp_ratio = (code_pixel.size() / 8.0) / input_text.size() * 1.0;
    std::cout << "compression ratio: " << comp_ratio << endl;
    }
void
parse(int argc, char* argv[])
{
  try
  {
    cxxopts::Options options("Final.cpp", " des");
    options
      .positional_help("[optional args]")
      .show_positional_help();

    bool apple = false;

    options
      .allow_unrecognised_options()
      .add_options()
      ("f,file", "File", cxxopts::value<std::vector<std::string>>(), "FILE")
      ("t", "A short-only option", cxxopts::value<std::string>())
      ("positional",
        "Positional arguments: these are the arguments that are entered "
        "without an option", cxxopts::value<std::vector<std::string>>())
      ("help", "Print help")
    #ifdef CXXOPTS_USE_UNICODE
      ("unicode", u8"A help option with non-ascii: à. Here the size of the"
        " string should be correct")
    #endif
    ;
    options.parse_positional({"f", "t", "file"});

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }

    if (result.count("f"))
    {  
      auto& ff = result["f"].as<std::vector<std::string>>();
      std::string arg=argv[1];
      std::string arg2=argv[3];

      //std::cout << "Files" << std::endl;
      for (const auto& f : ff)
      {
        if(result.count("t")==1){
    
       std::cout<<"Decoding"<<std::endl;
         decoding(arg,arg2);}
    else
    {
      std::cout<<"Encoding"<<std::endl;
        encoding(arg);
    }
        
      }
    }
    if (result.count("positional"))
    {
      std::cout << "Positional = {";
      auto& v = result["positional"].as<std::vector<std::string>>();
      for (const auto& s : v) {
        std::cout << s << ", ";
      }
      std::cout << "}" << std::endl;
    }

    std::cout << "Arguments remain = " << argc << std::endl;

    auto arguments = result.arguments();
    std::cout << "Saw " << arguments.size() << " arguments" << std::endl;
  }
  catch (const cxxopts::OptionException& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

int main(int argc, char* argv[])
{    
  parse(argc, argv);

  return 0;
}
