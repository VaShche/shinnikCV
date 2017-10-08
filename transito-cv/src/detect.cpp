/* HOG DETECTOR
 *
 */

#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/image_transforms.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/opencv.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

#include <opencv2/opencv.hpp>

#include "CNTKLibrary.h"
#pragma comment( lib, "Cntk.Core-2.2.lib" )

#include "hr_timer.h"

using namespace std;
using namespace dlib;

struct TrafficSign {
  string name;
  string svm_path;
  rgb_pixel color;
  TrafficSign(string name, string svm_path, rgb_pixel color) :
    name(name), svm_path(svm_path), color(color) {};
};

int main(int argc, char** argv) {
  try {
    command_line_parser parser;

    parser.add_option("h","Display this help message.");
    parser.add_option("u", "Upsample each input image <arg> times. Each \
                      upsampling quadruples the number of pixels in the image \
                      (default: 0).", 1);
    parser.add_option("wait","Wait user input to show next image.");

    parser.parse(argc, argv);
    parser.check_option_arg_range("u", 0, 8);

    const char* one_time_opts[] = {"h","u","wait"};
    parser.check_one_time_options(one_time_opts);

    // Display help message
    if (parser.option("h")) {
      cout << "Usage: " << argv[0] << " [options] <list of images>" << endl;
      parser.print_options();

      return EXIT_SUCCESS;
    }

    if (parser.number_of_arguments() == 0) {
      cout << "You must give a list of input files." << endl;
      cout << "\nTry the -h option for more information." << endl;
      return EXIT_FAILURE;
    }

    const unsigned long upsample_amount = get_option(parser, "u", 0);

	cv::Mat cv_image = cv::imread(parser[0]);
	cv::Mat cv_image_gray;
	cv::cvtColor(cv_image, cv_image_gray, cv::COLOR_BGR2GRAY);

	dlib::cv_image<unsigned char> dlib_cv_image(cv_image_gray);
	dlib::array2d<unsigned char> dlib_image;
	dlib::assign_image(dlib_image, dlib_cv_image);
    for (unsigned long i = 0; i < upsample_amount; ++i) {
        pyramid_up(dlib_image);
    }

    typedef scan_fhog_pyramid<pyramid_down<12> > image_scanner_type;

    // Load SVM detectors
    std::vector<TrafficSign> signs;
	signs.push_back(TrafficSign("blue_border", "data/blue_border.svm", rgb_pixel(255, 0, 0)));
	signs.push_back(TrafficSign("blue_rect", "data/blue_rect.svm", rgb_pixel(0, 255, 0)));
	signs.push_back(TrafficSign("danger", "data/danger.svm", rgb_pixel(0,0, 255)));
	signs.push_back(TrafficSign("main_road", "data/main_road.svm", rgb_pixel(128, 0, 128)));
	signs.push_back(TrafficSign("mandatory", "data/mandatory.svm", rgb_pixel(0, 128, 128)));
	signs.push_back(TrafficSign("prohibitory", "data/prohibitory.svm", rgb_pixel(128, 128, 0)));

    std::vector<object_detector<image_scanner_type> > detectors;	
    for (int i = 0; i < signs.size(); i++) {
      object_detector<image_scanner_type> detector;
      deserialize(signs[i].svm_path) >> detector;
      detectors.push_back(detector);
    }

	//Load CNTK model
	std::wstring model_path = L"data/RTSD_CNN.model";
	CNTK::DeviceDescriptor device = CNTK::DeviceDescriptor::CPUDevice();
	CNTK::FunctionPtr RTSD_model = CNTK::Function::Load(model_path, device);

	Timer timer;
	timer.start();
	std::vector<rect_detection> rects;
	evaluate_detectors(detectors, dlib_image, rects, 0.5f);

	for (auto r : rects) {
		cv::Rect rect(cv::Point2i(r.rect.left(), r.rect.top()), cv::Point2i(r.rect.right() + 1, r.rect.bottom() + 1));
		cv::Mat img_crop = cv_image(rect);
		cv::Mat img_crop_resize;
		cv::resize(img_crop, img_crop_resize, cv::Size(30, 30));

		CNTK::Variable inputVar = RTSD_model->Arguments()[0];
		CNTK::Variable outputVar = RTSD_model->Outputs()[4];

		std::vector<float> inputData(inputVar.Shape().TotalSize());
		for (int k = 0; k < inputVar.Shape()[2]; ++k) {
			for (int j = 0; j < inputVar.Shape()[1]; ++j) {
				for (int i = 0; i < inputVar.Shape()[0]; ++i) {
					inputData[k * inputVar.Shape()[1] * inputVar.Shape()[0] + j * inputVar.Shape()[0] + i] = 
						static_cast<float>(img_crop_resize.at<cv::Vec3b>(j, i)[k]);
				}
			}
		}
		CNTK::NDShape inputShape = inputVar.Shape().AppendShape({ 1, 1 });
		CNTK::ValuePtr inputValue = CNTK::MakeSharedObject<CNTK::Value>(CNTK::MakeSharedObject<CNTK::NDArrayView>(inputShape, inputData, true));
		std::unordered_map<CNTK::Variable, CNTK::ValuePtr> inputs = { { inputVar, inputValue } };

		std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { outputVar, nullptr } };
		RTSD_model->Evaluate(inputs, outputs, device);

		CNTK::ValuePtr outputVal = outputs[outputVar];
		std::vector<std::vector<float>> outputData;
		outputVal->CopyVariableValueTo(outputVar, outputData);
		int arg_max = max_element(outputData[0].begin(), outputData[0].begin() + outputData[0].size()) - outputData[0].begin();

		printf("rect: (%d, %d, %d, %d); arg_max: %d\n", rect.x, rect.y, rect.width, rect.height, arg_max);

		if (parser.option("wait")) {
			cv::imshow("ROI", img_crop);
			cv::waitKey(0);
		}
	}
	auto time = timer.get(1000.);
	cout << "time detect = " << time << "ms" << std::endl;

	if (parser.option("wait")) {
		image_window win;
		win.clear_overlay();
		win.set_image(dlib_image);

		for (unsigned long j = 0; j < rects.size(); ++j) {
			win.add_overlay(rects[j].rect, signs[rects[j].weight_index].color,
				signs[rects[j].weight_index].name);
		}

		system("pause");
		win.close_window();
		exit(0);
	}
  }
  catch (exception& e) {
	  cout << "\nexception thrown!" << endl;
	  cout << e.what() << endl;
  }
}
