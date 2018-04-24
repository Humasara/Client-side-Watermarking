#pragma once
#include <opencv\cv.h>
#include <opencv\cv.hpp>
#include <opencv2\core.hpp>
#include <iostream>
#include <fstream>
#include <utility>
#include <gmp_util.h>

class Paillier
{
public:
	void KeyGeneration(mpz_t n, mpz_t g, mpz_t u, mpz_t lambda);
	void Encrypt(mpz_t c, mpz_t m, mpz_t n, mpz_t g);
	void Decrypt(mpz_t m, mpz_t c, mpz_t lambda, mpz_t u, mpz_t n);
private:

};
