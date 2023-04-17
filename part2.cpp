//Mark Scheidker 
//cpe381 project part 2
//4/17/2023
//compile with: "g++ part2.cpp -o part2" all 3 files must be in the same directory
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <sys/time.h>
#include <stdlib.h>
#include "B_11025.h"
#include "B_22050.h"

using namespace std;

struct header {
    unsigned int ChunkID;
    unsigned int ChunkSize;
    unsigned int Format;
    unsigned int Subchunk1ID;
    unsigned int Subchunk1Size;
    unsigned short AudioFormat;
    unsigned short NumChannels;
    unsigned int SampleRate;
    unsigned int ByteRate;
    unsigned short BlockAlign;
    unsigned short BitsPerSample;
    unsigned int Subchunk2ID;
    unsigned int Subchunk2Size;
};

int main() {

    string infile_name, outfile_name;     //declare file variables
    FILE *infile;
    FILE *outfile;
    short data_L;
    short data_R;
    short max_val_L=0;
    short max_val_R=0;
    struct timeval start, end;
    
    //declare variables for convolution array length, and convolution array
    int BL;
    double* B = NULL;

    cout << "Enter the input file name: ";   //ask for file names
    cin >> infile_name;
    cout << "Enter the output file name: ";
    cin >> outfile_name;

    infile = fopen(infile_name.c_str(), "r");   //open both files
    outfile = fopen(outfile_name.c_str(), "w+");
    
    //quit if files cannot be opened
    if (infile == NULL) {
      cout << "Error: Could not open input file." << endl;
      return 1;
    }
    if (outfile == NULL) {
      cout << "Error: Could not open output file." << endl;
      return 1;
    }
    
    //create header struct and fill with data from input file, checking for supported sample rate before continuing
    struct header my_header;
    fread(&my_header, sizeof(my_header),1, infile);
    
    //supported sample rates have corresponding convolution array and length variables initialized  
    if(my_header.SampleRate == 11025){
        BL = BL_11025;
        //this is done through pointers and the new keyword to enable variable length arrays at compile time
        B = new double[BL];
        for(int i = 0; i<BL; i++){
            B[i] = (double)B_11025[i];
        }
    }
    else if(my_header.SampleRate == 22050){
        BL = BL_22050;
        //same for 11025 but double the length. length is kept track of using BL so no errors occur.
        B = new double[BL];
        for(int i = 0; i<BL; i++){
            B[i] = (double)B_22050[i];
        } 
    }
    else{
        cout<<"Audio file sample rate does not match supported sample rates [11025, 22050]"<<endl;
        return 0;
    }
    
    //write the entire header to the new file
    fwrite(&my_header, sizeof(my_header),1, outfile);
    
    //create an array for each chanel with the size of the convolution array, filling it with zeros
    double data_buffer_L[BL];
    double data_buffer_R[BL];
    for(int i = 0; i<BL; i++){
        data_buffer_L[i] = 0.0;
        data_buffer_R[i] = 0.0;
    }
    
    //initialize variables to accumulate convolution of each sample
    double tempL;
    double tempR;
    
    //start the timer
    gettimeofday(&start, NULL);
    
    //iterate through every sample of the file
    for (int i = 0; i < my_header.Subchunk2Size / 4; i++){
        //shift all sample array items to the right by one
        for(int j = BL-1; j>0; j--){
            data_buffer_L[j] = data_buffer_L[j-1];
            data_buffer_R[j] = data_buffer_R[j-1];
        }
        //read in new data from file
        fread(&data_L, sizeof(data_L), 1, infile);
        fread(&data_R, sizeof(data_R), 1, infile);
        
        //recast data from int sample to double 
        data_buffer_L[0] = (double)(data_L); 
        data_buffer_R[0] = (double)(data_R);
        
        //set accumulators to zero
        tempL = 0.0;
        tempR = 0.0;
        
        //calculate the value of one sample (L and R) by multiplying and accumulating the data buffer and the convolution array
        for(int j=0; j<BL; j++){
            tempL += data_buffer_L[j] * B[j];
            tempR += data_buffer_R[j] * B[j];  
        }
        
        //convert accumulator back to int
        data_L = ((short)(tempL));
        data_R = ((short)(tempR));
        
        //write data into new file
        fwrite(&data_L, sizeof(data_L), 1, outfile);
        fwrite(&data_R, sizeof(data_R), 1, outfile);
    }
    
    //output compute time 
    gettimeofday(&end, NULL);  
    double elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0; // Convert to milliseconds
    elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0; // Add microseconds as milliseconds
    cout<<"Program Runtime: "<<elapsed_time<<"ms"<<endl;  
    
    //free memory from stored convolution array pointer to prevent memory leaks
    delete[] B;
}