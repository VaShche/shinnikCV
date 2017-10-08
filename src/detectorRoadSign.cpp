#include "detectorRoadSign.hpp"

int Shinik::DetectorRoadSign::Init(const std::string& path) {

    return 0;
}

int Shinik::DetectorRoadSign::Process(const Mat& image, std::vector<Mat>& outSign) const {
    //TODO:
	outSign.push_back(image);

    return 0;
}
