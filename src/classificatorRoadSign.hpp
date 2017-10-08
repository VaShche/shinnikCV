#pragma once 
#ifndef __CLASSIFICATOR_ROAD_SIGN_HPP__
#define __CLASSIFICATOR_ROAD_SIGN_HPP__


#include <opencv2/core/core.hpp>
#include <string>
#include <map>

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
		std::string m_svm_dir;

		std::map<int, std::string> additional_info = {
			{ 33, "5t" },
			{ 34, "3.5" },
			{ 35, "4.5" },
			{ 41, "10" },
			{ 42, "20" },
			{ 43, "30" },
			{ 44, "40" },
			{ 45, "5" },
			{ 46, "50" },
			{ 47, "60" },
			{ 48, "70" },
			{ 49, "80" },
			{ 56, "8t" },
			{ 89, "20" },
			{ 90, "50" },
			{ 91, "60" },
			{ 92, "70" }
		};

		std::map<int, std::string> class_sign = {
			{ 0, "1.1" },
			{ 1, "1.11.1" },
			{ 2, "1.11.2" },
			{ 3, "1.12.1" },
			{ 4, "1.12.2" },
			{ 5, "1.13" },
			{ 6, "1.14" },
			{ 7, "1.15" },
			{ 8, "1.16" },
			{ 9, "1.17" },
			{ 10, "1.18" },
			{ 11, "1.19" },
			{ 12, "1.2" },
			{ 13, "1.20.1" },
			{ 14, "1.20.2" },
			{ 15, "1.20.3" },
			{ 16, "1.21" },
			{ 17, "1.22" },
			{ 18, "1.23" },
			{ 19, "1.25" },
			{ 20, "1.27" },
			{ 21, "1.33" },
			{ 22, "1.5" },
			{ 23, "1.8" },
			{ 24, "2.1" },
			{ 25, "2.2" },
			{ 26, "2.3.1" },
			{ 27, "2.3.2" },
			{ 28, "2.3.3" },
			{ 29, "2.5" },
			{ 30, "2.6" },
			{ 31, "3.1" },
			{ 32, "3.10" },
			{ 33, "3.11" },
			{ 34, "3.13" },
			{ 35, "3.13" },
			{ 36, "3.18.1" },
			{ 37, "3.18.2" },
			{ 38, "3.19" },
			{ 39, "3.2" },
			{ 40, "3.20" },
			{ 41, "3.24" },
			{ 42, "3.24" },
			{ 43, "3.24" },
			{ 44, "3.24" },
			{ 45, "3.24" },
			{ 46, "3.24" },
			{ 47, "3.24" },
			{ 48, "3.24" },
			{ 49, "3.24" },
			{ 50, "3.27" },
			{ 51, "3.28" },
			{ 52, "3.29" },
			{ 53, "3.30" },
			{ 54, "3.32" },
			{ 55, "3.4" },
			{ 56, "3.4" },
			{ 57, "4.1.1" },
			{ 58, "4.1.2" },
			{ 59, "4.1.2" },
			{ 60, "4.1.2" },
			{ 61, "4.1.3" },
			{ 62, "4.1.4" },
			{ 63, "4.1.5" },
			{ 64, "4.1.6" },
			{ 65, "4.2.1" },
			{ 66, "4.2.2" },
			{ 67, "4.2.3" },
			{ 68, "4.3" },
			{ 69, "5.11" },
			{ 70, "5.14" },
			{ 71, "5.15.1" },
			{ 72, "5.15.2" },
			{ 73, "5.15.2" },
			{ 74, "5.15.3" },
			{ 75, "5.15.5" },
			{ 76, "5.15.7" },
			{ 77, "5.16" },
			{ 78, "5.18" },
			{ 79, "5.19.1" },
			{ 80, "5.20" },
			{ 81, "5.21" },
			{ 82, "5.22" },
			{ 83, "5.3" },
			{ 84, "5.5" },
			{ 85, "5.6" },
			{ 86, "5.7.1" },
			{ 87, "5.7.2" },
			{ 88, "5.8" },
			{ 89, "6.2" },
			{ 90, "6.2" },
			{ 91, "6.2" },
			{ 92, "6.2" },
			{ 93, "6.3.1" },
			{ 94, "6.4" },
			{ 95, "6.6" },
			{ 96, "6.7" },
			{ 97, "7.1" },
			{ 98, "7.11" },
			{ 99, "7.12" },
			{ 100, "7.2" },
			{ 101, "7.3" },
			{ 102, "7.4" },
			{ 103, "7.5" },
			{ 104, "7.6" },
			{ 105, "7.7" }
		};
    };
};
#endif //__CLASSIFICATOR_ROAD_SIGN_HPP__