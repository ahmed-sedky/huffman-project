#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>

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
int main()
{
    int array_freq[256];           //initialize all the array of frequencies by zero
    for (int i = 0; i <= 255; i++)
    {
        array_freq[i] = 0;
    }

    std::ifstream inFile;  //read the pgm file.txt line by line and store it in a vector called bytes and calcu;ate the frequency of each pixel values
    std::string line;
    std::string input_text;
    std::cout<<"enter the text file ex: Pp42.txt"<<endl;
    std::string FileName;
    std::cin>>FileName;
    inFile.open(FileName, std::ios::binary|std::ios::in);//
    if (!inFile)
    {
        std::cerr << "can't open the file";
    }
    getline(inFile,line);           //to get the width and height 
    int index = line.find(" ");
    int height= std::stoi(line.substr(0,index));
    int width =std::stoi(line.substr(index+1));
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
    std::priority_queue<Node*, std::vector<Node*>, compareHuffNode> pq;            
    for (int j = 0; j <= 255; ++j) {         //create a newnode if for only the pixel value"j" is exist in the pgm file
        if (array_freq[j] != 0) {
            Node* newNode = new Node{ array_freq[j],nullptr,nullptr,j,"" };
            pq.push(newNode);
        }
    }
    while (pq.size() != 1) {         //build the haffman tree
        Node* NewNode = new Node{ 0,nullptr,nullptr,'A',"" };
        NewNode->left = pq.top();
        NewNode->freq += pq.top()->freq;
        pq.pop();
        NewNode->right = pq.top();
        NewNode->freq += pq.top()->freq;
        pq.pop();
        pq.push(NewNode);
    }

    traverse(pq.top(), "");
    for(int y= 0 ; y<=255 ;y++){
        if(array1[y]!="")
        std::cout<<y<<" "<<array1[y]<<std::endl;
    }
    std::cout<<width<<" "<<height<<endl;
    ofstream codeFile;
    codeFile.open("code"+FileName+"", std::ios::binary|std::ios::out);//
    if (!codeFile)
    {
        std::cerr << "can't open the file";
    }
    codeFile<<width<<" "<<height<<endl;
    for(int y= 0 ; y<=255 ;y++){
        if(array1[y]!="")
        codeFile<<y<<" "<<array1[y]<<std::endl;
    }
    //because any string consists of signed charecters
    // and we need to convert it to unsigned char beacuse the index of array1 couldn't be negative value
    std::string code_pixel="";
    for(int i =0 ;i<input_text.size();i++) 
     code_pixel+= array1[reinterpret_cast<unsigned char &>(input_text[i])];
    ofstream outputFile;
    outputFile.open(""+FileName+"compressed.txt", std::ios::binary|std::ios::out);//
    if (!outputFile)
    {
        std::cerr << "can't open the file";
    }
    //we can write only bytes to file not bits so if we had only 6 bits we will need padding of two bits to have a full byte to write
    char padding=8-(code_pixel.size()%8);  
    if (code_pixel.size()%8==0)
      padding =0;
    outputFile.write(&padding,1);
    outputFile<<endl;
    for (int d= 0; d<code_pixel.size()/8;d++){  
        std::string CodePixel =code_pixel.substr(d*8,8); //d*8 is the start of the index and 8 is length of the substring and we configure each 8 bytes in one byte
        char Pixels = binary_to_decimal(CodePixel);
        outputFile.write(&Pixels,1); //1 is the number of bytes we will write
    }
    if(code_pixel.size()%8!=0){
        std::string codePixel= code_pixel.substr((code_pixel.size()/8)*8);
        for(int i=0;i<padding;i++)
          codePixel+="0";
        char Pixels = binary_to_decimal(codePixel);
        outputFile.write(&Pixels,1);
    }
    float comp_ratio= (code_pixel.size()/8.0)/input_text.size()*1.0;
    std::cout<<"compression ratio: "<<comp_ratio<<endl;
    
    }