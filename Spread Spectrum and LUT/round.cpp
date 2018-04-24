#include <opencv\cv.h>
#include <opencv\cv.hpp>
#include <opencv2\core.hpp>
#include "round.h"

using namespace cv;
using namespace std;

Mat roundMat(Mat matrix)
{
	int rows = matrix.rows;
	int cols = matrix.cols;
	Mat round_matrix(rows, cols, CV_64F);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			round_matrix.at<double>(i, j) = cvRound(matrix.at<double>(i, j));
		}
	}

	return round_matrix;
}