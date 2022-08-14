#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <time.h>
#include <chrono>
using namespace std;

int main() {
    std::cout<<"program started"<<std::endl;
    uint32_t ChunkID;   //declare header information variables (many of these are not used)
    uint32_t ChunkSize;
    uint32_t Format;
    uint32_t Subchunk1ID;
    uint32_t Subchunk1Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    uint32_t Subchunk2ID;
    uint32_t Subchunk2Size;
    uint16_t data_L;
    uint16_t data_R;
    uint16_t max_val_L=0;
    uint16_t max_val_R=0;

    string infile_name, outfile_name;     //declare file variables
    infile_name = "input.wav";
    outfile_name = "output.wav";
    ifstream infile;
    ofstream outfile;

    infile.open(infile_name.c_str(), ios::binary);   //open both files
    outfile.open(outfile_name.c_str(), ios::binary);
//--------------------------------------------------------------------------------

    auto start_time = std::chrono::high_resolution_clock::now(); //start counting execution time

    cout << "\n-----File Data-----\n";

    infile.read(reinterpret_cast<char *>(&ChunkID), sizeof(ChunkID));
    outfile.write(reinterpret_cast<char *>(&ChunkID), sizeof(ChunkID));
    cout << "ChunkID: " << hex << ChunkID << endl;

    infile.read(reinterpret_cast<char *>(&ChunkSize), sizeof(ChunkSize));
    outfile.write(reinterpret_cast<char *>(&ChunkSize), sizeof(ChunkSize));
    cout << "ChunkSize: " << hex << ChunkSize << endl;

    infile.read(reinterpret_cast<char *>(&Format), sizeof(Format));
    outfile.write(reinterpret_cast<char *>(&Format), sizeof(Format));
    cout << "Format: " << hex << Format << endl;

    infile.read(reinterpret_cast<char *>(&Subchunk1ID), sizeof(Subchunk1ID));
    outfile.write(reinterpret_cast<char *>(&Subchunk1ID), sizeof(Subchunk1ID));
    cout << "Subchunk1ID: " << hex << Subchunk1ID << endl;

    infile.read(reinterpret_cast<char *>(&Subchunk1Size), sizeof(Subchunk1Size));
    outfile.write(reinterpret_cast<char *>(&Subchunk1Size), sizeof(Subchunk1Size));
    cout << "Subchunk1Size: " << hex << Subchunk1Size << endl;

    infile.read(reinterpret_cast<char *>(&AudioFormat), sizeof(AudioFormat));
    outfile.write(reinterpret_cast<char *>(&AudioFormat), sizeof(AudioFormat));
    cout << "AudioFormat: " << hex << AudioFormat << endl;

    infile.read(reinterpret_cast<char *>(&NumChannels), sizeof(NumChannels));
    outfile.write(reinterpret_cast<char *>(&NumChannels), sizeof(NumChannels));
    cout << "NumChannels: " << hex << NumChannels << endl;

    infile.read(reinterpret_cast<char *>(&SampleRate), sizeof(SampleRate));
    outfile.write(reinterpret_cast<char *>(&SampleRate), sizeof(SampleRate));
    cout << "SampleRate: " << hex << SampleRate << endl;

    infile.read(reinterpret_cast<char *>(&ByteRate), sizeof(ByteRate));
    outfile.write(reinterpret_cast<char *>(&ByteRate), sizeof(ByteRate));
    cout << "ByteRate: " << hex << ByteRate << endl;

    infile.read(reinterpret_cast<char *>(&BlockAlign), sizeof(BlockAlign));
    outfile.write(reinterpret_cast<char *>(&BlockAlign), sizeof(BlockAlign));
    cout << "BlockAlign: " << hex << BlockAlign << endl;

    infile.read(reinterpret_cast<char *>(&BitsPerSample), sizeof(BitsPerSample));
    outfile.write(reinterpret_cast<char *>(&BitsPerSample), sizeof(BitsPerSample));
    cout << "BitsPerSample: " << hex << BitsPerSample << endl;

    infile.read(reinterpret_cast<char *>(&Subchunk2ID), sizeof(Subchunk2ID));
    outfile.write(reinterpret_cast<char *>(&Subchunk2ID), sizeof(Subchunk2ID));
    cout << "Subchunk2ID: " << hex << Subchunk2ID << endl;

    infile.read(reinterpret_cast<char *>(&Subchunk2Size), sizeof(Subchunk2Size));
    outfile.write(reinterpret_cast<char *>(&Subchunk2Size), sizeof(Subchunk2Size));
    cout << "Subchunk2Size: " << hex << Subchunk2Size << endl;

    static short left_channel_array[308700];  //create an array for each channel and the output
    static short right_channel_array[308700];

    double convolution_array[] =   //initialize the convoluion array
            {0.754099080755355,
             -0.306424955216438,
             -0.0109785668104055,
             0.142043857656818,
             0.182584655545621,
             0.155338118223979,
            0.100782582663640,
            0.0466458350888621,
            0.00690933523812086,
            -0.0150953852744983,
            -0.0223221510949994,
            -0.0201448952888434,
            -0.0137591184875568,
            -0.00690807634309335,
            -0.00161601489128165,
            0.00149690415120219,
            0.00268659566167547,
            0.00258712835121095,
            0.00185857993415420,
            0.00100126168784284,
            0.000303177124177513,
            -0.000131040693363771,
            -0.000317368032265161,
            -0.000328887757982972,
            -0.000248551643462863,
            -0.000142587953008504,
            -0.0000513698943484396,
            0.00000844927664211670,
            0.0000366349981789294};

    for (int i = 0; i < Subchunk2Size / 4; i++)   //get the audio data from the file and store it in arrays
    {
        infile.read(reinterpret_cast<char *>(&data_L), sizeof(data_L));
        left_channel_array[i] = data_L;

        infile.read(reinterpret_cast<char *>(&data_R), sizeof(data_R));
        right_channel_array[i] = data_R;
    }


    for(int i = 0; i < Subchunk2Size / 4; i++)
    {
        short summation_left=0;
        short summation_right=0;

        for(int j = 0; j<min(29,min(i+1,308700-i)); j++)
        {
            summation_left += left_channel_array[i-j] * convolution_array[j];
            summation_right += right_channel_array[i-j] * convolution_array[j];
        }

        outfile.write(reinterpret_cast<char *>(&summation_left), sizeof(summation_left));
        outfile.write(reinterpret_cast<char *>(&summation_right), sizeof(summation_right));
    }

    outfile.close();
    infile.close();

    auto end_time = std::chrono::high_resolution_clock::now();  //end time count
    auto time = end_time - start_time;

    ofstream data_output;
    data_output.open("Scheidker_M_sum.txt");
    data_output<<"-----Summary-----\n";
    data_output<<"Name: Mark Scheidker\n";
    data_output<<"File: CPE_381_project_1.cpp\n";
    data_output<<"Number of Channels: "<<NumChannels<<endl;
    data_output<<"Sampling Frequency: "<<SampleRate<<endl;
    data_output<<"Audio Length: "<<(float)Subchunk2Size/(float)NumChannels/(float)SampleRate/2.0<<"s"<<endl;
    if(NumChannels == 2)
    {
        data_output<<"Max Value Left: "<<max_val_L<<endl;
        data_output<<"MaX Value Right: "<<max_val_R<<endl;
    }
    if(NumChannels == 1)
    {
        data_output<<"Max Value: "<<max_val_L<<endl;
    }

    data_output<<"Program Runtime: "<<time/std::chrono::milliseconds(1)<<"ms"<<endl;
    data_output.close();
}
