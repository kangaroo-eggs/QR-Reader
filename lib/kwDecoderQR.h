#ifndef KWDECODERQR_H
#define KWDECODERQR_H

#include "kwImage_object.h"
#include "kwPoint.h"
#include "CxDecoderGRS_QR.h"

class kwDecoderQR {
public:
    ///@brief Constructor
    kwDecoderQR() {}
    ///@brief Destructor
    ~kwDecoderQR() {}

    //-------------------------------------
    ///@brief UnMask the binary QR_Data and Mark the Eye
    void        UnMask( kwImageU8 source, int maskNum, int verNum, kwImageU8 &unmasked_result );
    ///@brief Get the QR Codeword and using Reed-Solomon code, i.e. retrival
    void        GetCodeword( kwImageU8 source, int verNum, int ECClevel, kwPixel codeWordBefore[] );
    ///@brief separate the data into blocks
    void        Solve_Interleaving( int verNum, int ECClevel, kwPixel codeWordBefore[], kwPixel* *saperatedData );
    ///@brief Decode the QR codeword, block by block
    int         GetDecodeData( int verNum, int ECClevel, kwPixel decodedDataBefore[], kwPixel decodedDataAfter[] );
    ///@brief together with UncompressText()
    int         GetBitSequence( unsigned cBytePool[], int nNumBytes, int iStartBit, int nNumBitsToGet );
    ///@brief Decode the Encoding DATA reduct the Decode DATA to EncodeDATA
    int         UncompressText( unsigned char cByteString[], int nNumBytes, int nSymVersion, unsigned char cOutputText[] );

//    ///@brief Decode the Encoding DATA reduct the Decode DATA to EncodeDATA
//    void        DecodeText(kwPixel decodeDATA[], kwPixel encodingDATA[], int verNum, int ECClevel);
    ///@brief The Docode process return the code word
    static int  Decoedinprocesss(int decode_digit, kwPixel*& dataPosition, int& dataRemainder);

    ///@brief from vernum & ECClevel to get the index of below tables
    int         GetIndex( int verNum, int ECClevel );
};

// -- for SHORT Block --
//version 1 | version 2 | version 3 | version 4  | version 5  | version 6 | version 7 | version 8  | version 9  | version 10| version 11 | version 12 | version 13 | version 14 | version 15 | version 16 | version 17 | version 18 | version 19 | version 20 | version 21 | version 22 | version 23 | version 24 | version 25 | version 26 | version 27 | version 28 | version 29 | version 30 | version 31 | version 32 | version 33 | version 34 | version 35 | version 36 | version 37 | version 38 | version 39 | version 40 |
//L  M  Q  H| L  M  Q  H| L  M  Q  H|  L  M  Q  H|  L  M  Q  H| L  M  Q  H| L  M  Q  H|  L  M  Q  H|  L  M  Q  H| L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|
const unsigned char    Ecc_S_numofBlocks[160] =
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,  1, 2, 2, 4,  1, 2, 2, 2, 2, 4, 4, 4, 2, 4, 2, 4,  2, 2, 4, 4,  2, 3, 4, 4, 2, 4, 6, 6,  4, 1, 4, 3,  2, 6, 4, 7,  4, 8, 8,12,  3, 4,11,11,  5, 5, 5,11,  5, 7,15, 3,  1,10, 1, 2,  5, 9,17, 2,  3, 3,17, 9,  3, 3,15,15,  4,17,17,19,  2,17, 7,34,  4, 4,11,16,  6, 6,11,30,  8, 8, 7,22, 10,19,28,33,  8,22, 8,12,  3, 3, 4,11,  7,21, 1,19,  5,19,15,23, 13, 2,42,23, 17,10,10,19, 17,14,29,11, 13,14,44,59, 12,12,39,22,  6, 6,46, 2, 17,29,49,24,  4,13,48,42, 20,40,43,10, 19,18,34,20};
const unsigned char    Ecc_S_c[160] =   // = (datawords length)total number of codewords (data + ECC)
{26,26,26,26,44,44,44,44,70,70,35,35,100,50,50,25,134,67,33,33,86,43,43,43,98,49,32,39,121,60,40,40,146,58,36,36,86,69,43,43,101,80,50,36,116,58,46,42,133,59,44,33,145,64,36,36,109,65,54,36,122,73,43,45,135,74,50,42,150,69,50,42,141,70,47,39,135,67,54,43,144,68,50,46,139,74,54,37,151,75,54,45,147,73,54,46,132,75,54,45,142,74,50,46,152,73,53,45,147,73,54,45,146,73,53,45,145,75,54,45,145,74,54,45,145,74,54,45,145,74,54,45,145,74,54,46,151,75,54,45,151,75,54,45,152,74,54,45,152,74,54,45,147,75,54,45,148,75,54,45};
const unsigned char    Ecc_S_k[160] =   // = number of data codewords
{19,16,13, 9,34,28,22,16,55,44,17,13, 80,32,24, 9,108,43,15,11,68,27,19,15,78,31,14,13, 97,38,18,14,116,36,16,12,68,43,19,15, 81,50,22,12, 92,36,20,14,107,37,20,11,115,40,16,12, 87,41,24,12, 98,45,19,15,107,46,22,14,120,43,22,14,113,44,21,13,107,41,24,15,116,42,22,16,111,46,24,13,121,47,24,15,117,45,24,16,106,47,24,15,114,46,22,16,122,45,23,15,117,45,24,15,116,45,23,15,115,47,24,15,115,46,24,15,115,46,24,15,115,46,24,15,115,46,24,16,121,47,24,15,121,47,24,15,122,46,24,15,122,46,24,15,117,47,24,15,118,47,24,15};
const unsigned char    Ecc_S_r[160] =   // = number of correction capacity (乘 2 之後變成 number of ECC codewords)
{ 2, 4, 6, 8, 4, 8,11,14, 7,13, 9,11, 10, 9,13, 8, 13,12, 9,11, 9, 8,12,14,10, 9, 9,13, 12,11,11,13, 15,11,10,12, 9,13,12,14, 10,15,14,12, 12,11,13,14, 13,11,12,11, 15,12,10,12, 11,12,15,12, 12,14,12,15, 14,14,14,14, 15,13,14,14, 14,13,13,13, 14,13,15,14, 14,13,14,15, 14,14,15,12, 15,14,15,15, 15,14,15,15, 13,14,15,15, 14,14,14,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15};

// -- for LONG Block --
//version 1 | version 2 | version 3 | version 4  | version 5  | version 6 | version 7 | version 8  | version 9  | version 10| version 11 | version 12 | version 13 | version 14 | version 15 | version 16 | version 17 | version 18 | version 19 | version 20 | version 21 | version 22 | version 23 | version 24 | version 25 | version 26 | version 27 | version 28 | version 29 | version 30 | version 31 | version 32 | version 33 | version 34 | version 35 | version 36 | version 37 | version 38 | version 39 | version 40 |
//L  M  Q  H| L  M  Q  H| L  M  Q  H|  L  M  Q  H|  L  M  Q  H| L  M  Q  H| L  M  Q  H|  L  M  Q  H|  L  M  Q  H| L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|  L  M  Q  H|
const unsigned char    Ecc_L_numofBlocks[160] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 4, 1,  0, 2, 2, 2,  0, 2, 4, 4, 2, 1, 2, 2,  0, 4, 4, 8,  2, 2, 6, 4,  0, 1, 4, 4,  1, 5, 5, 5,  1, 5, 7, 7,  1, 3, 2,13,  5, 1,15,17,  1, 4, 1,19,  4,11, 4,16,  5,13, 5,10,  4, 0, 6, 6,  7, 0,16, 0,  5,14,14,14,  4,14,16, 2,  4,13,22,13,  2, 4, 6, 4,  4, 3,26,28, 10,23,31,31,  7,7,37,26,  10,10,25,25,  3,29, 1,28,  0,23,35,35,  1,21,19,46,  6,23, 7, 1,  7,26,14,41, 14,34,10,64,  4,14,10,46, 18,32,14,32,  4, 7,22,67,  6,31,34,61};
const unsigned char    Ecc_L_c[160] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0,34,34, 0, 0, 0, 0, 0, 0,33,40,  0,61,41,41,  0,59,37,37,87,70,44,44,  0,81,51,37,117,59,47,43,  0,60,45,34,146,65,37,37,110,66,55,37,123,74,44,46,136,75,51,43,151,70,51,43,142,71,48,40,136,68,55,44,145, 0,51,47,140, 0,55, 0,152,76,55,46,148,74,55,47,133,76,55,46,143,75,51,47,153,74,54,46,148,74,55,46,147,74,54,46,146,76,55,46,146,75,55,46,  0,75,55,46,146,75,55,46,146,75,55,47,152,76,55,46,152,76,55,46,153,75,55,46,153,75,55,46,148,76,55,46,149,76,55,46};
const unsigned char    Ecc_L_k[160] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0,16,12, 0, 0, 0, 0, 0, 0,15,14,  0,39,19,15,  0,37,17,13,69,44,20,16,  0,51,23,13, 93,37,21,15,  0,38,21,12,116,41,17,13, 88,42,25,13, 99,46,20,16,108,47,23,15,121,44,23,15,114,45,22,14,108,42,25,16,117, 0,23,17,112, 0,25, 0,122,48,25,16,118,46,25,17,107,48,25,16,115,47,23,17,123,46,24,16,118,46,25,16,117,46,24,16,116,48,25,16,116,47,25,16,  0,47,25,16,116,47,25,16,116,47,25,17,122,48,25,16,122,48,25,16,123,47,25,16,123,47,25,16,118,48,25,16,119,48,25,16};
const unsigned char    Ecc_L_r[160] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 9,11, 0, 0, 0, 0, 0, 0, 9,13,  0,11,11,13,  0,11,10,12, 9,13,12,14,  0,15,14,12, 12,11,13,14,  0,11,12,11, 15,12,10,12, 11,12,15,12, 12,14,12,15, 14,14,14,14, 15,13,14,14, 14,13,13,13, 14,13,15,14, 14, 0,14,15, 14, 0,15, 0, 15,14,15,15, 15,14,15,15, 13,14,15,15, 14,14,14,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15,  0,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15, 15,14,15,15};

const unsigned char    Ecc_p[160] = // 使用 Decode( int nLenECCwords, int nLenDatawords ) 時，加在 Lendatawords 上
{ 3, 2, 1, 1, 2, 0, 0, 0, 1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};

// Total Codewords ~ Datawords + 2*EccCapability(ECCwords)
// Total number of codewords =  (short) Number of error correction blocks * Codewords + (long) Number of error correction blocks * Codewords

const kwPixel AlphaNumeric_Table[45] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '$', '%', '*', '+', '-', '.', '/', ':'};


#endif // KWDECODERQR_H
