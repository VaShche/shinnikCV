

#include "detectorRoadSign.hpp"

using namespace std;
using namespace dlib;



struct TrafficSign {
    string name;
    string svm_path;
    rgb_pixel color;
    TrafficSign(string name, string svm_path, rgb_pixel color) :
        name(name), svm_path(svm_path), color(color) {};
};

Shinik::DetectorRoadSign::DetectorRoadSign() //:
    //device(CNTK::DeviceDescriptor::CPUDevice()) 
{

}

int Shinik::DetectorRoadSign::Init(const std::string& path) {

    // Load SVM detectors
    std::vector<TrafficSign> signs;
    signs.push_back(TrafficSign("blue_border", "data/blue_border.svm", rgb_pixel(255, 0, 0)));
    signs.push_back(TrafficSign("blue_rect", "data/blue_rect.svm", rgb_pixel(0, 255, 0)));
    signs.push_back(TrafficSign("danger", "data/danger.svm", rgb_pixel(0, 0, 255)));
    signs.push_back(TrafficSign("main_road", "data/main_road.svm", rgb_pixel(128, 0, 128)));
    signs.push_back(TrafficSign("mandatory", "data/mandatory.svm", rgb_pixel(0, 128, 128)));
    signs.push_back(TrafficSign("prohibitory", "data/prohibitory.svm", rgb_pixel(128, 128, 0)));



    for (int i = 0; i < signs.size(); i++) {
        object_detector<image_scanner_type> detector;
        deserialize(signs[i].svm_path) >> detector;
        detectors.push_back(detector);
    }

    //Load CNTK model
    std::wstring model_path = L"data/RTSD_CNN.model";
    try {
    //    RTSD_model = CNTK::Function::Load(model_path, device);
    }
    catch (std::exception& e)
    {
        std::cout << e.what()<< std::endl;
    }

    return 0;
}

int Shinik::DetectorRoadSign::Process(const Mat& cv_image, std::vector<Mat>& outSign) const {

    cv::Mat cv_image_gray;
    cv::cvtColor(cv_image, cv_image_gray, cv::COLOR_BGR2GRAY);

    dlib::cv_image<unsigned char> dlib_cv_image(cv_image_gray);
    dlib::array2d<unsigned char> dlib_image;
    dlib::assign_image(dlib_image, dlib_cv_image);
    for (unsigned long i = 0; i < upsample_amount; ++i) {
        pyramid_up(dlib_image);
    }

    typedef scan_fhog_pyramid<pyramid_down<12> > image_scanner_type;

    std::vector<rect_detection> rects;
    evaluate_detectors(detectors, dlib_image, rects, 0.5f);

    std::vector<cv::Rect> res;
    for (int i = 0; i < rects.size(); ++i) {
        
        rect_detection&  r = rects[i];
        res.push_back(cv::Rect (cv::Point2i(r.rect.left(), r.rect.top()), cv::Point2i(r.rect.right() + 1, r.rect.bottom() + 1)));
        
    }
    //return res;

    /*for (auto r : rects) {
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
    }
    */
    //outSign.push_back(image);

    return 0;

}