#pragma once
#include <iostream>
#include <opencv\cv.hpp>
#include <opencv2\core.hpp>
#include <iostream>
#include <fstream>
#include <utility>

using namespace cv;
using namespace std;

#ifndef WatermarkVerification_h
#define WatermarkVerification_h

Mat WatermarkVerification(const string OriginalImage, const string WatermarkedImage, int section_key, int Watermark_Size);

#endif

