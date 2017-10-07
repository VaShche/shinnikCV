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
		ClassificatorRoadSign(){}

		ClassificatorRoadSign(const std::string& path)
			:m_svm_dir(path) {}

		int Train();

		Sign Process(const Mat& imageSign) const;

		int Predict() const;

	private:
		std::string m_svm_dir="";
    };
};
#endif //__CLASSIFICATOR_ROAD_SIGN_HPP__