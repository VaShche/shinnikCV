#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "classificatorRoadSign.hpp"
#include "detectorRoadSign.hpp"

using namespace cv;
using namespace std;

std::string getOutputFilename(const std::string& inputFilename) {
    std::size_t found = inputFilename.find_last_of("/\\");

    std::string outputFile = inputFilename.substr(found + 1);

    std::size_t foundDot = outputFile.find_last_of(".");
    outputFile = outputFile.substr(0, foundDot);
    outputFile = outputFile + std::string(".csv");

    return outputFile;

}


int main(int argc, char * argv[]) {
    if (argc != 2)
    {
        cout << " Usage: Shinik image.jpg" << endl;
        return -1;
    }
    std::string inputFile = argv[1];
    std::string outputFile = getOutputFilename(inputFile);
 
    Mat image;
    image = imread(inputFile, CV_LOAD_IMAGE_COLOR);   // Read the file

    if (!image.data)                              // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        return -2;
    }
    Shinik::DetectorRoadSign detector;
    detector.Init("./");

    Shinik::ClassificatorRoadSign classificator;
    classificator.Init("D:/Education/Programming/HackCV/rtsd-r3");

    std::vector<Mat> roadSigns;
    detector.Process(image, roadSigns);

    std::ofstream outFile(outputFile);

#pragma omp parallel  for
        for (int i = 0; i < roadSigns.size(); ++i) {
            Shinik::Sign  sign = classificator.Process(roadSigns[i]);
        #pragma omp critical
            {
            //print sign
            }
        }
    


    outFile << "";

    return 0;
}