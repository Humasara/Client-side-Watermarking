#pragma once
#include <iostream>
#include <opencv\cv.hpp>
#include <opencv2\core.hpp>
#include <iostream>
#include <fstream>
#include <utility>

using namespace cv;
using namespace std;

Mat IndexGen(int section_key, int rows, int cols, int Watermark_Size)
{
	int length = rows * cols;
	vector<int> Index(length);
	
	for (int i = 0; i < length; i++)
	{
		Index[i] = i;
	}

	setRNGSeed(section_key);
	theRNG().state = section_key;
	randShuffle(Index);

	vector<int> Index2(Watermark_Size);
	if (Watermark_Size <= length)
	{
		for (int i = 0; i < Watermark_Size; i++)
		{
			Index2[i] = Index[i];
		}
	}

	Mat IndexMat(Watermark_Size, 2, CV_64F);
	for (int k = 0; k < Watermark_Size; k++) // k
	{
		int x = Index2[k] % cols;
		if (x == 0)
		{
			if (Index2[k] == 0)
			{
				IndexMat.at<double>(k, 0) = 0;
				IndexMat.at<double>(k, 1) = 0;
			}
			else
			{
				IndexMat.at<double>(k, 0) = Index2[k] / cols - 1;
				IndexMat.at<double>(k, 1) = cols - 1;
			}
		}
		else
		{
			IndexMat.at<double>(k, 0) = floor(Index2[k] / cols);
			IndexMat.at<double>(k, 1) = x - 1;
		}
	}

	return IndexMat;
}