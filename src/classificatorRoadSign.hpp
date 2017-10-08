#pragma once 
#ifndef __CLASSIFICATOR_ROAD_SIGN_HPP__
#define __CLASSIFICATOR_ROAD_SIGN_HPP__


#include <opencv2/core/core.hpp>
#include <string>

using namespace cv;

namespace Shinik {
	struct Rectangle {

		Rectangle() 
			: x1(0), y1(0), x2(0), y2(0) {}

		Rectangle(const int _x1, const  int _y1, const int _x2, const  int _y2)
			: x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

		std::string to_csv() {
			return std::to_string(x1) + "," + std::to_string(y2) + "," + std::to_string(x2) + "," + std::to_string(y2);
		}

		int x1;
		int y1;
		int x2;
		int y2;
	};

    struct Sign {
		Sign()
			: has_add_info(false), is_temporary(false), sign_id(""), bound(), add_info("") {}
		Sign(const bool has, const bool temp, const std::string &id, const Rectangle &bb, const std::string &info)
			: has_add_info(has), is_temporary(temp), sign_id(id), bound(bb), add_info(info){}

		std::string to_csv() {
			return std::to_string(has_add_info) + ";"
				+ std::to_string(is_temporary) + ";"
				+ sign_id + ";"
				+ bound.to_csv() + ";"
				+ add_info;
		}

		bool has_add_info;
		bool is_temporary;
		std::string sign_id;
		Rectangle bound;
		std::string add_info;
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