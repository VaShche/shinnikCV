#pragma once 
#ifndef __CLASSIFICATOR_ROAD_SIGN_HPP__
#define __CLASSIFICATOR_ROAD_SIGN_HPP__


#include <opencv2/core/core.hpp>
#include <string>

using namespace cv;

namespace Shinik {
    struct Sign {
        //TODO: поля дописать
    };

    class ClassificatorRoadSign {
    public:
		int Init(const std::string& path);

        Sign Process(const Mat& imageSign) const;

    };
};
#endif //__CLASSIFICATOR_ROAD_SIGN_HPP__