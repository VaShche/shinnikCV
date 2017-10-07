#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;
int main(int argc, char * argv[]) {

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if (!image.data)                              // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    std::ofstream outFile("results.csv");
    outFile << "rr";

    return 0;
}