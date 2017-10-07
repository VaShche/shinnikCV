#pragma once 
#ifndef __CLASSIFICATOR_ROAD_SIGN_HPP__
#define __CLASSIFICATOR_ROAD_SIGN_HPP__


#include <opencv2/core/core.hpp>
#include <string>
using namespace cv;

namespace Shinik {
    class ClassificatorRoadSign {
    public:
        int Init(const std::string& path);

        int Process(const Mat& image, std::vector<Mat>& outSign) const;

    };
};
#endif //__CLASSIFICATOR_ROAD_SIGN_HPP__