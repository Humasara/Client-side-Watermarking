#pragma once
#include <iostream>
#include <opencv\cv.hpp>
#include <opencv2\core.hpp>
using namespace cv;
using namespace std;

#ifndef IndexGen_h
#define IndexGen_h

Mat IndexGen(int section_key, int rows, int cols, int Watermark_Size);

#endif
