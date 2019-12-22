// SIMD_Cpp_debug.cpp : A program to see what is inside the registers.
//

#include "pch.h"
#include <iostream>
#include <dvec.h>
#include <string>

using namespace std;

// Function displaying only registers with 16 x uInt8. And message.
// Use with cpp code.
void printRegister(__m128i registerToprint, const string &msg) {
	unsigned char tab_debug[16] = { 0 };
	unsigned char *dest = tab_debug;

	_mm_store_si128((__m128i*)&dest[0], registerToprint);
	
	cout << msg << endl;
	cout << "\/\/\/\/ LO \/\/\/\/" << endl;

	for (int i = 0; i < 16; i++)
		cout << dec << (unsigned int)dest[i] << endl;

	cout << "/\/\/\/\ HI /\/\/\/" << endl;
}

// Function displaying only registers with 16 x uInt8. And message.
// Use with asm code.
void printRegister(unsigned char * src) {

	for (int i = 0; i < 16; i++) {
		cout << dec << (unsigned int)src[i] << endl;
	}
}

// CPP function using SIMD programing model.
void CppSIMDFunction(unsigned char *src) {
	// Start index of src t
	int srcIndex = 0;

	// How to define float numbers as integer of uInt16 type.
	// Its const variable of short type.
	const __m128i rFactor = _mm_set1_epi16((short)(0.2989*32768.0 + 0.5));
	const __m128i gFactor = _mm_set1_epi16((short)(0.5870*32768.0 + 0.5));
	const __m128i bFactor = _mm_set1_epi16((short)(0.1140*32768.0 + 0.5));

	// vecSrc - source vector (BGRA BGRA BGRA BGRA).
	// Load data from tab[] into 128-bit register starting from adress at pointer src. (From 0 index so load all 16 elements x 8bit).
	__m128i vecSrc = _mm_loadu_si128((__m128i*) &src[srcIndex]);

	// Shuffle to configuration A0A1A2A3_R0R1R2R3_G0G1G2G3_B0B1B2B3
	// Not revers so mask is read from left (Lo) to right (Hi). And counting from righ in srcVect (Lo).
	__m128i shuffleMask = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	__m128i AAAA_R0RRR_G0GGG_B0BBB = _mm_shuffle_epi8(vecSrc, shuffleMask);

	// Put B0BBB in lower part.
	__m128i B0_XXX = _mm_slli_si128(AAAA_R0RRR_G0GGG_B0BBB, 12);
	__m128i XXX_B0 = _mm_srli_si128(B0_XXX, 12);

	// Put G0GGG in Lower part.
	__m128i G0_B_XX = _mm_slli_si128(AAAA_R0RRR_G0GGG_B0BBB, 8);
	__m128i XXX_G0 = _mm_srli_si128(G0_B_XX, 12);

	// Put R0RRR in Lower part.
	__m128i R0_G_XX = _mm_slli_si128(AAAA_R0RRR_G0GGG_B0BBB, 4);
	__m128i XXX_R0 = _mm_srli_si128(R0_G_XX, 12);

	// Unpack uint8 elements to uint16 elements.
	// The sequence in uInt8 is like (Hi) XXXX XXXX XXXX XXXX (Lo) where X represent uInt8.
	// In uInt16 is like (Hi) X_X_ X_X_ X_X_ X_X_ (Lo)
	__m128i B0BBB = _mm_cvtepu8_epi16(XXX_B0);
	__m128i G0GGG = _mm_cvtepu8_epi16(XXX_G0);
	__m128i R0RRR = _mm_cvtepu8_epi16(XXX_R0);

	// Multiply epi16 registers.
	__m128i B0BBB_mul = _mm_mulhrs_epi16(B0BBB, bFactor);
	__m128i G0GGG_mul = _mm_mulhrs_epi16(G0GGG, gFactor);
	__m128i R0RRR_mul = _mm_mulhrs_epi16(R0RRR, rFactor);

	__m128i BGR_gray = _mm_add_epi16(_mm_add_epi16(B0BBB_mul, G0GGG_mul), R0RRR_mul);

	__m128i grayMsk = _mm_setr_epi8(0, 0, 0, 0, 2, 2, 2, 2, 4, 4, 4, 4, 6, 6, 6, 6);
	__m128i vectGray = _mm_shuffle_epi8(BGR_gray, grayMsk);

	printRegister(vectGray, "Gray");
}

// Functions from .asm file.
extern "C" int testFunctionASM(unsigned char *);

int main()
{
	// Example array as 128-bit register with 16xuInt8. That represent each channel of pixel in BGRA configuration.
	unsigned char tab[] = { 100,200,250,255, 101,201,251,255, 102,202,252,255, 103,203,253,255 };

	// A pointer to source tab for simulate dll parameters reference.
	unsigned char *src = tab;

	//CppSIMDFunction(src);

	testFunctionASM(src);
	printRegister(src);
}
