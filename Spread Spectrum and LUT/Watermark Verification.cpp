#include <opencv\cv.h>
#include <opencv\cv.hpp>
#include <opencv2\core.hpp>
#include "round.h"
#include "IndexGen.h"
#include <iostream>
#include <fstream>
#include <utility>
#include "Paillier.h"
#include <gmp_util.h>

using namespace cv;
using namespace std;

Mat WatermarkVerification(const string OriginalImage, const string WatermarkedImage, int section_key, int Watermark_Size)
{
	Mat Original_Image = imread(OriginalImage, CV_64F);
	Mat Watermarked_Image = imread(WatermarkedImage, CV_64F);
	Watermarked_Image.convertTo(Watermarked_Image, CV_64F);
	int L = 2 * Watermark_Size;

	Mat Decryption_LUT(L, 1, CV_64F), Watermark_LUT, Encrypted_LUT_D, Decrypted_LUT_D(L, 1, CV_64F), Encrypted_Image;

	FileStorage LUTs("LUTs.xml", FileStorage::READ);
	LUTs["Watermark_LUT"] >> Watermark_LUT;
	LUTs["Encrypted_LUT_D"] >> Encrypted_LUT_D;

	FileStorage WM_EN("WM_EN_Image.xml", FileStorage::READ);
	WM_EN["Encrypted_Image"] >> Encrypted_Image;

	Mat Decrypted_Image = Encrypted_Image.clone();

	int rows = Original_Image.rows;
	int cols = Original_Image.cols;

	cvtColor(Original_Image, Original_Image, CV_BGR2GRAY);
	Original_Image.convertTo(Original_Image, CV_64F);

	vector<double> Decryption_Signal(Watermark_Size), Watermark_Original(Watermark_Size), Estimated_Watermark(Watermark_Size);
	// Extract Watermark
	mpz_t n, g, u, lambda, D1, D_LUT_D;
	mpz_init(n); mpz_init(g); mpz_init(u); mpz_init(lambda); mpz_init(D_LUT_D);

	Paillier key;
	key.KeyGeneration(n, g, u, lambda); //generate PublicKey(n, g) and PrivateKey(lambda, u); 
	//Decrypt the Look-Up Table of Decryption
	for (int i = 0; i < L; i++)
	{
		long long int a = (long long int)Encrypted_LUT_D.at<double>(i, 0);
		mpz_init_set_ui(D1, a);
		key.Decrypt(D_LUT_D, D1, lambda, u, n);
		int b = mpz_get_ui(D_LUT_D);
		Decrypted_LUT_D.at<double>(i, 0) = b;
	}
	Decryption_LUT = Decrypted_LUT_D;

	setRNGSeed(section_key);
	Mat IndexMat(Watermark_Size, 2, CV_64F);
	IndexMat = IndexGen(section_key, rows, cols, Watermark_Size);

	int S = 2; //number of look-up entries
	vector<int> t_index(L);

	for (int i = 0; i < L; i++)
	{
		t_index[i] = i;
	}
	mpz_t D_I, D_I_D, D_L, D_C, D_Content, en_prod;
	mpz_init(D_I_D);
	mpz_init(D_C);
	mpz_init(D_Content);
	mpz_init_set_ui(en_prod, 1);

	setRNGSeed(section_key);
	theRNG().state = section_key;
	randShuffle(t_index);
	int j = 0;
	for (int k = 0; k < Watermark_Size; k++)
	{
		for (int i = 0; i < S; i++)
		{
			int tij = t_index[i + j];
			Decryption_Signal[k] = Decryption_Signal[k] + Decryption_LUT.at<double>(tij, 0);
			Watermark_Original[k] = Watermark_Original[k] + Watermark_LUT.at<double>(tij, 0);
			long long int temp = (long long int)Encrypted_LUT_D.at<double>(tij, 0);
			mpz_mul_ui(en_prod, en_prod, temp);
		}
		int x = IndexMat.at<double>(k, 0);
		int y = IndexMat.at<double>(k, 1);
		Watermarked_Image.at<double>(x, y) = Watermarked_Image.at<double>(x, y) + Decryption_Signal[k];
		Estimated_Watermark[k] = Watermarked_Image.at<double>(x, y) - Original_Image.at<double>(x, y);
		j = j + 2;
		
		//Decryption of the content using Encrypted Content and Encrypted LUT_D in the homomorphic space
		long long int d = (long long int)Encrypted_Image.at<double>(x, y);
		mpz_init_set_ui(D_I, d);
		mpz_mul(D_C, D_I, en_prod);
		key.Decrypt(D_Content, D_C, lambda, u, n);
		int di = mpz_get_ui(D_Content);
		Decrypted_Image.at<double>(x, y) = di;
		mpz_set_ui(en_prod, 1);
	}
	imwrite("Decrypted_Image.tif", Decrypted_Image);

	FileStorage CheckWatermark("Watermarks.xml", FileStorage::WRITE);
	CheckWatermark << "Estimated_Watermark" << Estimated_Watermark;
	CheckWatermark << "Watermark_Original" << Watermark_Original;
	CheckWatermark << "Decrypted_LUT_D" << Decrypted_LUT_D;
	CheckWatermark.release();

	return Watermarked_Image;
}