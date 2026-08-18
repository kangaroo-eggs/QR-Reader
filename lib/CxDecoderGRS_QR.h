#ifndef CXDECODERGRS_QR_H
#define CXDECODERGRS_QR_H

#define CXDECODERGRS_QR_VERSION_NUM    "CXDECODERGRS_QR_v1.06.208170"

#include <string.h>

#define MAX_NUM_ERRORS      15
#define MAX_LEN_CODEWORDS   255
#define GALOIS_FIELD        256

class CxDecoderGRS_QR
{
protected:
    static const unsigned char  m_cAlpha[GALOIS_FIELD];
    static const unsigned char  m_cGLog[GALOIS_FIELD];

    unsigned char               m_cErrPosRoots[MAX_NUM_ERRORS+1];
    unsigned char               m_cErrPositions[MAX_NUM_ERRORS+1];
    unsigned char               m_cErrMagnitudes[MAX_NUM_ERRORS+1];
    unsigned char               m_cSyndromes[2*MAX_NUM_ERRORS];
    unsigned char               m_cSigma[MAX_NUM_ERRORS+1];

    unsigned char               m_cGaussSheet[MAX_NUM_ERRORS][MAX_NUM_ERRORS+1];

    int                         GaussianElim( int );
    unsigned char               GF256Mul( unsigned char, unsigned char );
    unsigned char               GF256Inv( unsigned char );
    unsigned char               GF256Power( unsigned char, unsigned char );
    unsigned char               Tuple2Power( unsigned char );
    unsigned char               Power2Tuple( unsigned char );
    void                        Reverse(unsigned char cCodewords[], int len);

public:
    CxDecoderGRS_QR();

    unsigned char*              m_cInput;
    unsigned char*              m_cOutput;

    int                         Decode( int nLenECCwords, int nLenDatawords );
    void                        SetInputOutput( unsigned char* cInputArrayPtr, unsigned char* cOutputArrayPtr );
};

#endif // CXDECODERGRS_QR_H
