#include <gmp_util.h>
#include "Paillier.h"


using namespace std;


void Paillier::KeyGeneration(mpz_t n, mpz_t g, mpz_t u, mpz_t lambda)
{
	//public_key(n, g);
	//private_key(lambda, u);

	int p = 53;
	int q = 59;
	int product = p * q;		//n=p*q
	mpz_init_set_ui(n, product);
	mpz_t p1, q1;
	mpz_init(lambda);
	mpz_init_set_ui(p1, p - 1);
	mpz_init_set_ui(q1, q - 1);
	mpz_lcm(lambda, p1, q1);	//lambda = lcm(p-1, q-1)
	
	mpz_t n_2, x;
	mpz_init(x);
	mpz_init(n_2);
	mpz_add_ui(g, n, 1);		// g = n + 1
	mpz_mul(n_2, n, n);
	mpz_powm(x, g, lambda, n_2);

	mpz_t x_1, L_f;
	mpz_t n_1;
	mpz_init(u);
	mpz_init(x_1);
	mpz_init(L_f);
	mpz_init_set_ui(n_1, product);
	mpz_sub_ui(x_1, x, 1);

	mpz_div(L_f, x_1, n_1);
	mpz_invert(u, L_f, n_1);	//u
}

void Paillier::Encrypt(mpz_t c, mpz_t m, mpz_t n, mpz_t g)
{
	//c: encrypted message
	//m: message to be encrypted, 0 < m < n = pq
	//(n, g): public_key

	mpz_t r, n_2, prod1, prod2, prod;
	mpz_init_set_ui(r, 2);		//random r

	mpz_init(prod1);
	mpz_init(prod2);
	mpz_init(prod);
	mpz_init(n_2);
	mpz_mul(n_2, n, n);
	mpz_powm(prod1, g, m, n_2);	//(g^m mod n^2)
	mpz_powm(prod2, r, n, n_2); //(r^n mod n^2)
	mpz_mul(prod, prod1, prod2);
	mpz_mod(c, prod, n_2); 
}

void Paillier::Decrypt(mpz_t m, mpz_t c, mpz_t lambda, mpz_t u, mpz_t n)
{
	//m: message after decrypted
	//c: decrypted message
	//(lambda, u): private_key
	//(n, g): public_key

	mpz_t x_1, L_f;
	mpz_t n_2, x;
	mpz_t m1, m2, m12;
	mpz_init(x_1);
	mpz_init(n_2);
	mpz_init(L_f);
	mpz_init(x);
	mpz_init(m1); mpz_init(m2); mpz_init(m12);

	mpz_mul(n_2, n, n);
	mpz_powm(x, c, lambda, n_2);
	mpz_sub_ui(x_1, x, 1);
	mpz_div(L_f, x_1, n);

	mpz_mod(m1, L_f, n);
	mpz_mod(m2, u, n);
	mpz_mul(m12, m1, m2);
	mpz_mod(m, m12, n);
}