#pragma once 
#ifndef __DETECTOR_ROAD_SIGN_HPP__
#define __DETECTOR_ROAD_SIGN_HPP__

#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/image_transforms.h>
#include <dlib/cmd_line_parser.h>
#include <dlib/opencv.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>

//#include "CNTKLibrary.h"

using namespace cv;
using namespace std;
using namespace dlib;


namespace Shinik {


    class DetectorRoadSign {
    public:
        DetectorRoadSign();
       int Init(const std::string& path);

       int Process(const Mat& image, std::vector<Rect>& outROIs) const;


       const unsigned long upsample_amount = 0;
 

       typedef scan_fhog_pyramid<pyramid_down<12> > image_scanner_type;

       std::vector<object_detector<image_scanner_type> > detectors;

       //CNTK::DeviceDescriptor device;
       //CNTK::FunctionPtr RTSD_model;
    };

};
#endif //__DETECTOR_ROAD_SIGN_HPP__