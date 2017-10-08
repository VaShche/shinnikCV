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

Shinik::ClassificatorRoadSign::ClassificatorRoadSign(const std::string& path)
	:m_svm_dir(path) {

	std::cout << "loading SVM from file" << std::endl;
	svm = Algorithm::load<ml::SVM>(m_svm_dir + "/trained_svm");
}

void Shinik::ClassificatorRoadSign::Train() const {
    
	std::ifstream file(m_svm_dir + "/gt_train.csv");
	if (!file.is_open()) {
		std::cerr << "Cannot read train data." << std::endl;
		return;
	}

	std::string line;
	std::vector<std::string> file_names;
	std::vector<int> labels;

	std::cout << "reading lines from file" << std::endl;
	std::getline(file, line); // skip first line
	while (std::getline(file, line)) {
		const std::size_t found = line.find_last_of(",");

		const std::string file_name = m_svm_dir + "/train/" + line.substr(0, found);
		file_names.push_back(file_name);
		
		const int label = std::stoi(line.substr(found + 1));
		labels.push_back(label);
	}

	const int number_of_images = file_names.size();
	const int number_of_hog_features = 324;
	
	const Size crop_size(32, 32);
	const Size winSize(32, 32);
	const Size blockSize(16, 16);
	const Size blockStride(8, 8);
	const Size cellSize(8, 8);
	const int nbins = 9;
	std::vector<float> features;
	cv::HOGDescriptor hog(winSize, blockSize, blockStride, cellSize, nbins);

	Mat training_mat(number_of_images, number_of_hog_features, CV_32FC1);
	Mat labels_mat(number_of_images, 1, CV_32SC1);

	std::cout << "creating training mat\n";
	for (int row = 0; row < file_names.size(); row++) {
		Mat image = imread(file_names[row], CV_LOAD_IMAGE_COLOR);

		if (!image.data){
			std::cerr << "Could not open or find the image " << file_names[row] << std::endl;
			return;
		}
		cv::resize(image, image, crop_size);
		hog.compute(image, features);

		if (features.size() != number_of_hog_features) {
			std::cerr << "Invalid number of HOG features detected. Actual " << features.size() 
				<< " expected " << number_of_hog_features << std::endl;
			return;
		}

		for (int col = 0; col < features.size(); col++) {
			training_mat.at<float>(row,col) = features[col];
		}

		labels_mat.at<int>(row) = labels[row];
	}
	std::cout << "training_mat " << training_mat.rows << " " << training_mat.cols << std::endl;

	std::cout << "creating SVM" << std::endl;
	Ptr<cv::ml::SVM> svm_train = cv::ml::SVM::create();
	svm_train->setType(SVM::C_SVC);
	svm_train->setKernel(SVM::LINEAR);
	svm_train->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	
	std::cout << "training SVM" << std::endl;
	svm_train->train(training_mat, ROW_SAMPLE, labels_mat);

	std::cout << "saving SVM" << std::endl;
	svm_train->save(m_svm_dir + "/trained_svm");
}

Shinik::Sign Shinik::ClassificatorRoadSign::Process(const Mat& imageSign) const {
	Sign result;

	std::cout << "processing image" << std::endl;
	if (!imageSign.data) {
		std::cerr << "Passed empty image. " << std::endl;
		return {};
	}
	const Size size(32, 32);
	Mat image32;
	cv::resize(imageSign, image32, size);//resize image

	const Size winSize(32, 32);
	const Size blockSize(16, 16);
	const Size blockStride(8, 8);
	const Size cellSize(8, 8);
	const int nbins = 9;
	std::vector<float> features;
	cv::HOGDescriptor hog(winSize, blockSize, blockStride, cellSize, nbins);
	hog.compute(image32, features);

	const int predicted_class = std::round(svm->predict(features));
	std::cout << "Predicted sign class: " << " " << predicted_class << std::endl;

	if (predicted_class < 0 || predicted_class > 105) {
		std::cerr << "Invalid predicted class id." << std::endl;
	}
	result.sign_id = class_sign.at(predicted_class);

	if (additional_info.find(predicted_class) != additional_info.end()) {
		result.add_info = additional_info.at(predicted_class);
		result.has_add_info = true;
	}

	std::cout << "check if the sign is temporary" << std::endl;
	Mat hsv;
	cvtColor(image32, hsv, CV_BGR2HSV);

	Mat h_hist;
	std::vector<Mat> hsv_vector;
	split(hsv, hsv_vector);
	const int histSize = 180;
	const float range[] = { 0, 179 };
	const float* histRange = { range };
	calcHist(&hsv_vector[0], 1, 0, Mat(), h_hist, 1, &histSize, &histRange, true, false);

	double maxVal = 0;
	cv::Point max_id;
	minMaxLoc(h_hist, 0, &maxVal, 0, &max_id);
	int yellow_value = max_id.y;
	if (yellow_value >= 7 && yellow_value <= 34) {
		if ((predicted_class >= 0 && predicted_class <= 23)				// sign number 1.*
			|| (predicted_class >= 31 && predicted_class <= 56)) {		// sign number 3.*
			std::cout << "temporary sign" << std::endl;
			result.is_temporary = true;
		}
	}

	return result;
}

void Shinik::ClassificatorRoadSign::Predict() const {
	
	std::ifstream file(m_svm_dir + "/gt_test.csv");
	std::string line;
	std::vector<std::string> file_names;
	std::vector<int> labels;

	std::cout << "reading lines from file" << std::endl;
	std::getline(file, line); // skip first line
	while (std::getline(file, line)) {
		const std::size_t found = line.find_last_of(",");

		const std::string file_name = m_svm_dir + "/test/" + line.substr(0, found);
		file_names.push_back(file_name);

		const int label = std::stoi(line.substr(found + 1));
		labels.push_back(label);
	}

	const int number_of_images = file_names.size();
	const int number_of_hog_features = 324;
	int true_labels_count = 0;

	const Size crop_size(32, 32);
	const Size winSize(32, 32);
	const Size blockSize(16, 16);
	const Size blockStride(8, 8);
	const Size cellSize(8, 8);
	const int nbins = 9;
	std::vector<float> features;
	cv::HOGDescriptor hog(winSize, blockSize, blockStride, cellSize, nbins);

	std::cout << "calculaing HOG detections and prediction" << std::endl;
	for (int row = 0; row < file_names.size(); row++) {
		Mat image = imread(file_names[row], CV_LOAD_IMAGE_COLOR);

		if (!image.data) {
			std::cerr << "Could not open or find the image " << file_names[row] << std::endl;
			return;
		}
		cv::resize(image, image, crop_size);//resize image

		hog.compute(image, features);

		if (features.size() != number_of_hog_features) {
			std::cerr << "Invalid number of HOG features detected. Actual " << features.size()
				<< " expected " << number_of_hog_features << std::endl;
			return;
		}

		const float predicted_class = svm->predict(features);
		std::cout << file_names[row] << " " << predicted_class << " " << labels[row] << std::endl;
		
		if (predicted_class == labels[row]) {
			true_labels_count++;
		}
	}

	std::cout << "SVM Quality: " << (float)true_labels_count / number_of_images
		<< std::endl << "true assigned: " << true_labels_count
		<< std::endl << "all images count: " << number_of_images 
		<< std::endl;
}