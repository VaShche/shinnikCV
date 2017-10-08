
#include "detectorRoadSign.hpp"
#include "classificatorRoadSign.hpp"
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



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
	std::cout << "run detector" << std::endl;
    Shinik::DetectorRoadSign detector;
    detector.Init("./");

    Shinik::ClassificatorRoadSign classificator("data");
    //classificator.Train();

	std::vector<cv::Rect> detected_bounding_box;
    detector.Process(image, detected_bounding_box);

	std::ofstream outFile(outputFile);
	outFile << inputFile << ";" << to_string(detected_bounding_box.size()) << std::endl;

#pragma omp parallel  for
	for (int i = 0; i < detected_bounding_box.size(); i++) {
			cv::Mat ROI = image(detected_bounding_box[i]);
			Shinik::Sign detectedSign = classificator.Process(ROI);

			detectedSign.bound = Shinik::Rectangle(detected_bounding_box[i]);
        #pragma omp critical
            {
				outFile << detectedSign.to_csv() << std::endl;
            //print sign
            }
        }

	outFile.close();

    return 0;
}