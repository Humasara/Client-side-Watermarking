#include <opencv\cv.h>
#include <opencv\cv.hpp>
#include <opencv2\core.hpp>
#include "round.h"
#include "IndexGen.h"
#include "WatermarkVerification.h"
#include <iostream>
#include <fstream>
#include <utility>
#include "Paillier.h"

using namespace cv;
using namespace std;

Mat WatermarkEncryption(const string InputImage, int section_key, int Watermark_Size)
{
	Mat inputRGBImage = imread(InputImage);
	int rows = inputRGBImage.rows;
	int cols = inputRGBImage.cols;

	vector<double> Watermark_Signal(Watermark_Size);

	//Generating Long-term Encryption LUT
	int L = 2 * Watermark_Size;
	double std_E = 2; //Standard Deviation of LUT E
	Mat sigma_E = Mat::ones(1, 1, CV_64F);
	sigma_E = std_E * sigma_E;
	Mat mean_E = Mat::ones(1, 1, CV_64F);
	mean_E = mean_E.mul(mean_E, -5);
	Mat Encryption_LUT(L, 1, CV_64F);
	randn(Encryption_LUT, mean_E, sigma_E);  //Long-term Encryption LUT
	Encryption_LUT = roundMat(Encryption_LUT);

	//Generating Watermark LUT
	double std_W = 2; //standard deviation of LUT W
	Mat sigma_W = Mat::ones(1, 1, CV_64F);
	sigma_W = std_W * sigma_W;
	Mat mean_W = Mat::ones(1, 1, CV_64F);
	mean_W = mean_W.mul(mean_W, 10);
	Mat Watermark_LUT(L, 1, CV_64F);
	randn(Watermark_LUT, mean_W, sigma_W);
	Watermark_LUT = roundMat(Watermark_LUT);  //Watermark LUT

	//Generating Decryption LUT
	Mat Decryption_LUT(L, 1, CV_64F);
	Decryption_LUT = -Encryption_LUT + Watermark_LUT;
	
	//Encryption LUTs using Paillier cryptosystem
	mpz_t n, g, u, lambda, D, E, W;
	mpz_init(n);
	mpz_init(g);
	mpz_init(u);
	mpz_init(lambda);
	Paillier key;
	key.KeyGeneration(n, g, u, lambda); //generate PublicKey(n, g) and PrivateKey(lambda, u);

	Mat Encrypted_LUT_D(L, 1, CV_64F);
	mpz_t E_encrypt, W_encrypt, D_encrypt;
	mpz_init(E_encrypt);
	mpz_init(W_encrypt);
	mpz_init(D_encrypt);
	for (int i = 0; i < L; i++)
	{
		int a = -(int)Encryption_LUT.at<double>(i, 0);
		int b = Watermark_LUT.at<double>(i, 0);
		mpz_init_set_si(E, a);
		mpz_init_set_si(W, b);

		key.Encrypt(E_encrypt, E, n, g);
		key.Encrypt(W_encrypt, W, n, g);

		mpz_mul(D_encrypt, E_encrypt, W_encrypt);
		long long int c = mpz_get_ui(D_encrypt);
		Encrypted_LUT_D.at<double>(i, 0) = c;
	}

	// Save LUTs to file
	FileStorage LUTs("LUTs.xml", FileStorage::WRITE);
	LUTs << "Encryption_LUT" << Encryption_LUT;
	LUTs << "Decryption_LUT" << Decryption_LUT;
	LUTs << "Watermark_LUT" << Watermark_LUT;
	LUTs << "Encrypted_LUT_D" << Encrypted_LUT_D;
	LUTs.release();

	//Index generator
	setRNGSeed(section_key);
	Mat IndexMat(Watermark_Size, 2, CV_64F);
	IndexMat = IndexGen(section_key, rows, cols, Watermark_Size);

	// Encrypt Watermark into Image
	int S = 2; //number of look-up entries
	vector<int> t_index(L);

	for (int i = 0; i < L; i++)
	{
		t_index[i] = i;
	}

	setRNGSeed(section_key);
	theRNG().state = section_key;
	randShuffle(t_index);
	int j = 0;

	Mat Input_Image;
	cvtColor(inputRGBImage, Input_Image, CV_BGR2GRAY);
	Input_Image.convertTo(Input_Image, CV_64F);
	Mat Watermarked_Image = Input_Image.clone();
	Mat Encrypted_Image = Input_Image.clone();
	mpz_t I, I_Encrypt;
	mpz_init(I_Encrypt);

	for (int k = 0; k < Watermark_Size; k++) 
	{
		for (int i = 0; i < S; i++)
		{
			int tij = t_index[i + j];
			Watermark_Signal[k] = Watermark_Signal[k] + Encryption_LUT.at<double>(tij, 0);
		}
		int x = IndexMat.at<double>(k, 0);
		int y = IndexMat.at<double>(k, 1);
		Watermarked_Image.at<double>(x, y) = Input_Image.at<double>(x, y) + Watermark_Signal[k];

		//Encrypt Original Content
		int c = (int)Watermarked_Image.at<double>(x, y);
		mpz_init_set_si(I, c);
		key.Encrypt(I_Encrypt, I, n, g);
		long long int ei = mpz_get_ui(I_Encrypt);
		Encrypted_Image.at<double>(x, y) = ei;
		j = j + 2;
	}
	FileStorage WM_EN("WM_EN_Image.xml", FileStorage::WRITE);
	WM_EN << "Encrypted_Image" << Encrypted_Image;
	WM_EN << "Watermarked_Image" << Watermarked_Image;
	WM_EN << "Original_Image" << Input_Image;
	WM_EN.release();

	imwrite("Original_Image.tif", Input_Image);
	imwrite("Encrypted_Image.tif", Encrypted_Image);

	FileStorage IndexMatrix("IndexMatrix.xml", FileStorage::WRITE);
	IndexMatrix << "IndexMatrix" << IndexMat;
	IndexMatrix.release();

	return Watermarked_Image;
}
