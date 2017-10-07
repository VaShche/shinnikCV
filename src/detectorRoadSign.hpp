#pragma once 
#ifndef __DETECTOR_ROAD_SIGN_HPP__
#define __DETECTOR_ROAD_SIGN_HPP__

#include <opencv2/core/core.hpp>
#include <string>
using namespace cv;

namespace Shinik {


    class DetectorRoadSign {
    public:
       int Init(const std::string& path);

       int Process(const Mat& image, std::vector<Mat>& outSign) const;

    };

};
#endif //__DETECTOR_ROAD_SIGN_HPP__