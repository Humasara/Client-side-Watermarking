#pragma once
#include <iostream>
#include <opencv\cv.hpp>
#include <opencv2\core.hpp>
#include <iostream>
#include <fstream>
#include <utility>

using namespace cv;
using namespace std;

#ifndef WatermarkEncryption_h
#define WatermarkEncryption_h

Mat WatermarkEncryption(const string InputImage, int section_key, int Watermark_Size);

#endif