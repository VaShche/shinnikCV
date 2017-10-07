#include "ClassificatorRoadSign.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/ml.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <fstream>

using namespace ml;

int Shinik::ClassificatorRoadSign::Init(const std::string& dir) {
    
	std::ifstream file(dir + "/gt_train.csv");
	std::string line;
	std::vector<std::string> file_names;
	std::vector<int> labels;

	std::cout << "reading lines from file\n";
	std::getline(file, line); // skip first line
	while (std::getline(file, line)) {
		const std::size_t found = line.find_last_of(",");

		const std::string file_name = dir + "/train/" + line.substr(0, found);
		file_names.push_back(file_name);
		
		const int label = std::stoi(line.substr(found + 1));
		labels.push_back(label);
	}

	const int number_of_images = file_names.size();
	const int number_of_hog_features = 324;
	
	Mat training_mat(number_of_images, number_of_hog_features, CV_32FC1);
	Mat labels_mat(number_of_images, 1, CV_32SC1);

	std::cout << "creating training mat\n";
	for (int row = 0; row < file_names.size(); row++) {
		Mat image = imread(file_names[row], CV_LOAD_IMAGE_COLOR);

		if (!image.data){
			std::cerr << "Could not open or find the image " << file_names[row] << std::endl;
			return -1;
		}
		const Size size(32, 32);	
		cv::resize(image, image, size);//resize image

		cv::imwrite("32x32.png", image);

		const Size winSize(32, 32);
		const Size blockSize(16, 16);
		const Size blockStride(8, 8);
		const Size cellSize(8, 8);
		const int nbins = 9;
		std::vector<float> features;
		cv::HOGDescriptor hog(winSize, blockSize, blockStride, cellSize, nbins);
		hog.compute(image, features);
		if (features.size() != number_of_hog_features) {
			std::cerr << "Invalid number of HOG features detected. Actual " << features.size() 
				<< " expected " << number_of_hog_features << std::endl;
			return -1;
		}

		for (int col = 0; col < features.size(); col++) {
			training_mat.at<float>(row,col) = features[col];
		}

		labels_mat.at<int>(row) = labels[row];
	}
	std::cout << "training_mat " << training_mat.rows << " " << training_mat.cols << std::endl;

	std::cout << "creating SVM\n";
	Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	
	std::cout << "training SVM\n";
	svm->train(training_mat, ROW_SAMPLE, labels_mat);

	std::cout << "saving SVM\n";
	svm->save(dir + "/trained_svm");
	
    return 0;
}

Shinik::Sign Shinik::ClassificatorRoadSign::Process(const Mat& imageSign ) const {
    //TODO:
    Shinik::Sign res;

    return res;

}