#include "CxDecoderGRS_QR.h"

//////////////////////////////////////////////////////////////////////////
/// \brief CxDecoderGRS_QR::Decode
/// \param nLenECCwords
/// \param nLenCodewords
/// \return
///
/// Reed-Solomon Error Correcting
///
///
/// [Sample code] An example for QR ver# 2M w/ RS(44,28), max #err = 8:
///       CxDecoderGRS_QR MyTrainGRS;
///       //                             |<=================================================================== Reed Solomon Codewords RS(44,28) ========================================================================>|
///       //                             |<-------------------------------------------- Datawords ----------------------------------------------------->| |<----------------------- ECC words -------------------------->|
///       //            Original data = { 91, 73,110, 32,116,104,101, 32,109,101,109,111,114,121, 32,111,102, 32, 69, 46, 32, 71, 97,108,111,105,115, 93,  102, 0,108,142,215,133,202,226,126,191,122,245,160,106,113,238};
///       // 6 errs:                      x   x       x           x               x                   x
///       unsigned char cReceived[44] = {255,127,110, 63,116,104, 31, 32,109,101, 15,111,114,121, 32,  7,102, 32, 69, 46, 32, 71, 97,108,111,105,115, 93,  102, 0,108,142,215,133,202,226,126,191,122,245,160,106,113,238};
///       unsigned char cCorrected[44];
///       MyTrainGRS.SetInputOutput( cReceived, cCorrected );
///       MyTrainGRS.Decode( 44, 28 );
///
int CxDecoderGRS_QR::Decode( int nLenCodewords, int nLenDatawords )
{
    if ( m_cInput == nullptr || m_cOutput == nullptr )
        return 0;

    int nLenECCwords = nLenCodewords - nLenDatawords;

    unsigned char* cInputInternal = new unsigned char[nLenCodewords];
    ::memcpy( cInputInternal, m_cInput, nLenCodewords*sizeof(unsigned char) );

    // reversing
    Reverse( cInputInternal, nLenCodewords );

    //==============================
    // Calculate Syndromes
    //==============================
    for (int s=0; s < nLenECCwords; s++)
    {
        m_cSyndromes[s] = 0;
        for (int i=0; i < nLenCodewords; i++)
            m_cSyndromes[s]= m_cSyndromes[s] ^ GF256Mul(cInputInternal[i],Power2Tuple((i*(s+1))%255));
    }

    //============================================================
    // Find the err position polynomial: Sigma[]
    // -- Solving linear equ to obtain the coefficients
    //============================================================
    int rank;

    for (int i=0; i < nLenECCwords/2; i++)
        for (int j=0; j <= nLenECCwords/2; j++)
            m_cGaussSheet[i][j] = m_cSyndromes[i+j];

    rank = GaussianElim(nLenECCwords/2);

    if ( rank == 0 )
    {
        ::memcpy( m_cOutput, m_cInput, nLenCodewords*sizeof(unsigned char) );
        delete [] cInputInternal;
        return 1;
    }
    if ( rank >= nLenECCwords/2 )
    {
        ::memset( m_cOutput, 0, nLenCodewords*sizeof(unsigned char) );
        delete [] cInputInternal;
        return 0;
    }

    // === Making the solution of Sigmas NOT TRIVAL
    for ( int i=0; i < rank; i++)
        for ( int j=rank; j < nLenECCwords/2; ++j )
            m_cGaussSheet[i][nLenECCwords/2] ^= m_cGaussSheet[i][j];
    for ( int i=rank; i < nLenECCwords/2; ++i )
        m_cGaussSheet[i][nLenECCwords/2] = 1;
    //===
    for ( int i=0; i < nLenECCwords/2; ++i )
        m_cSigma[nLenECCwords/2-i] = m_cGaussSheet[i][nLenECCwords/2];
    m_cSigma[0] = 1;

    //============================================================
    // Find err locations
    // -- by finding the roots from the Err position poly: m_cSigma[]
    //============================================================
    int Eval, iPos=0;

    for ( unsigned aa=1; aa <= 255; ++aa )
    {
        Eval=0;
        for ( unsigned i=0; i <= (unsigned)(nLenECCwords/2); ++i )
            Eval ^= GF256Mul(m_cSigma[nLenECCwords/2-i],GF256Power(aa,i));
        if ((Eval==0)&&(Tuple2Power(aa)<nLenCodewords))
        {
            m_cErrPosRoots[iPos] = aa;
            m_cErrPositions[iPos] = Tuple2Power(aa);
            iPos++;
        }
    }

    //============================================================
    // Find err magnitude
    //============================================================
    for ( int i=0; i < iPos; i++ )
        for ( int j=0; j < iPos; j++ )
            m_cGaussSheet[i][j] = GF256Power(m_cErrPosRoots[j], i+1);

    for ( int i=0; i < iPos; i++ )
        m_cGaussSheet[i][iPos] = m_cSyndromes[i];

    rank = GaussianElim(iPos);

    for ( int i=0; i < iPos; i++ )
        m_cErrMagnitudes[i] = m_cGaussSheet[i][iPos];

    for ( int i=0; i < nLenCodewords; i++ )
        m_cOutput[i] = cInputInternal[i];
    for ( int i=0; i < iPos; i++ )
        m_cOutput[m_cErrPositions[i]] = (m_cErrMagnitudes[i]) ^ (m_cOutput[m_cErrPositions[i]]);

    delete [] cInputInternal;

    // reversing
    Reverse( m_cOutput, nLenCodewords );
    return 1;
}


//////////////////////////////////////////////////////////////////////////
/// \brief CxDecoderGRS_QR::Reverse
/// \param cCodewords
///
void CxDecoderGRS_QR::Reverse( unsigned char cCodewords[], int len )
{
    for (int i=0; i <= (len/2-1); i++)
    {
        char tmp = cCodewords[i];
        cCodewords[i] = cCodewords[len-1-i];
        cCodewords[len-1-i] = tmp;
    }
}


//////////////////////////////////////////////////////////////////////////
/// \brief CxDecoderGRS_QR::SetInputOutput
/// \param cInputArrayPtr
/// \param cOutputArrayPtr
///
void CxDecoderGRS_QR::SetInputOutput( unsigned char *cInputArrayPtr, unsigned char *cOutputArrayPtr )
{
    m_cInput = cInputArrayPtr;
    m_cOutput = cOutputArrayPtr;
}


//////////////////////////////////////////////////////////////////////////
// GaussianElim()
// -- Gauss Elimination for solving linear equations
//////////////////////////////////////////////////////////////////////////
int CxDecoderGRS_QR::GaussianElim(int nVar)
{
    unsigned char ratio1, ratio2, tmp;
    int rank;

    for ( int i=0; i < nVar; i++ )
    {
        if ( m_cGaussSheet[i][i] == 0 )
            for ( int j=i+1; j < nVar; j++ )
                if ( m_cGaussSheet[j][i] != 0 )
                {    // row exchanging
                    for ( int k=0; k < nVar+1; k++ )
                    {
                        tmp = m_cGaussSheet[j][k];
                        m_cGaussSheet[j][k] = m_cGaussSheet[i][k];
                        m_cGaussSheet[i][k] = tmp;
                    }
                    break;
                }
        if ( m_cGaussSheet[i][i] == 0 )
            break;

        ratio1 = GF256Inv(m_cGaussSheet[i][i]);
        for ( int m=0; m < nVar+1; m++ )
            m_cGaussSheet[i][m] = GF256Mul(ratio1,m_cGaussSheet[i][m]);

        for ( int j=0; j < nVar; j++ )
            if (i!=j)
            {
                ratio2 = m_cGaussSheet[j][i];
                for ( int k=0; k < nVar+1; k++ )
                    m_cGaussSheet[j][k] ^= GF256Mul(ratio2,m_cGaussSheet[i][k]);
            }
    }

    // find the rank
    for (rank=0; (rank<nVar)&&(m_cGaussSheet[rank][rank]!=0); rank++);

    return rank;
}


/////////////////////////////////////
/// GF(256) operations
/////////////////////////////////////

/////////////////////////////
// A^P in GF(2^8)          //
/////////////////////////////
unsigned char CxDecoderGRS_QR::GF256Power( unsigned char a, unsigned char p )
{
    if (a==0) return (0);
    if (p==0) return (1);
    if (p==1) return (a);
    return m_cAlpha[((unsigned)p*(unsigned)m_cGLog[a])%255];
}

///////////////////////////////
/*---Reciprocal in GF(2^8)---*/
///////////////////////////////
unsigned char CxDecoderGRS_QR::GF256Inv( unsigned char a )
{
    return (m_cAlpha[255-m_cGLog[a]]);
}

///////////////////////////////////
/*---Multiplication in GF(2^8)---*/
///////////////////////////////////
unsigned char CxDecoderGRS_QR::GF256Mul( unsigned char a,unsigned char b )
{
    unsigned tmpa, tmpb;
    if ( a==0 || b==0 )
        return 0;
    else {
        tmpa = Tuple2Power(a);
        tmpb = Tuple2Power(b);
        return( Power2Tuple((tmpa+tmpb)%255) );
    }
}

/////////////////////////////////////////////////////
/*---Alpha power rep. to Decimal rep. in GF(2^8)---*/
/////////////////////////////////////////////////////
unsigned char CxDecoderGRS_QR::Power2Tuple( unsigned char power )
{
    return(m_cAlpha[power%255]);
}

/////////////////////////////////////////////////////
/*---Decimal rep. to Alpha power rep. in GF(2^8)---*/
/////////////////////////////////////////////////////
unsigned char CxDecoderGRS_QR::Tuple2Power( unsigned char tuple )
{
    return(m_cGLog[tuple]);
}

//////////////////////////////////////////////////////////////////////////
/// \brief CxDecoderGRS_QR::CxDecoderGRS_QR
///
CxDecoderGRS_QR::CxDecoderGRS_QR()
{
    m_cInput = (unsigned char*)0;
    m_cOutput = (unsigned char*)0;
}


////////////////////////////////////
// Table 4 -- GF(2^8)             //
//  Power->Tuple  Tuple->Power    //
////////////////////////////////////
const unsigned char CxDecoderGRS_QR::m_cAlpha[256] = {
//0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1d, 0x3a, 0x74, 0xe8, 0xcd, 0x87, 0x13, 0x26,  // 0
0x4c, 0x98, 0x2d, 0x5a, 0xb4, 0x75, 0xea, 0xc9, 0x8f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0,  // 1
0x9d, 0x27, 0x4e, 0x9c, 0x25, 0x4a, 0x94, 0x35, 0x6a, 0xd4, 0xb5, 0x77, 0xee, 0xc1, 0x9f, 0x23,  // 2
0x46, 0x8c, 0x05, 0x0a, 0x14, 0x28, 0x50, 0xa0, 0x5d, 0xba, 0x69, 0xd2, 0xb9, 0x6f, 0xde, 0xa1,  // 3
0x5f, 0xbe, 0x61, 0xc2, 0x99, 0x2f, 0x5e, 0xbc, 0x65, 0xca, 0x89, 0x0f, 0x1e, 0x3c, 0x78, 0xf0,  // 4
0xfd, 0xe7, 0xd3, 0xbb, 0x6b, 0xd6, 0xb1, 0x7f, 0xfe, 0xe1, 0xdf, 0xa3, 0x5b, 0xb6, 0x71, 0xe2,  // 5
0xd9, 0xaf, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88, 0x0d, 0x1a, 0x34, 0x68, 0xd0, 0xbd, 0x67, 0xce,  // 6
0x81, 0x1f, 0x3e, 0x7c, 0xf8, 0xed, 0xc7, 0x93, 0x3b, 0x76, 0xec, 0xc5, 0x97, 0x33, 0x66, 0xcc,  // 7
0x85, 0x17, 0x2e, 0x5c, 0xb8, 0x6d, 0xda, 0xa9, 0x4f, 0x9e, 0x21, 0x42, 0x84, 0x15, 0x2a, 0x54,  // 8
0xa8, 0x4d, 0x9a, 0x29, 0x52, 0xa4, 0x55, 0xaa, 0x49, 0x92, 0x39, 0x72, 0xe4, 0xd5, 0xb7, 0x73,  // 9
0xe6, 0xd1, 0xbf, 0x63, 0xc6, 0x91, 0x3f, 0x7e, 0xfc, 0xe5, 0xd7, 0xb3, 0x7b, 0xf6, 0xf1, 0xff,  // A
0xe3, 0xdb, 0xab, 0x4b, 0x96, 0x31, 0x62, 0xc4, 0x95, 0x37, 0x6e, 0xdc, 0xa5, 0x57, 0xae, 0x41,  // B
0x82, 0x19, 0x32, 0x64, 0xc8, 0x8d, 0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0xdd, 0xa7, 0x53, 0xa6,  // C
0x51, 0xa2, 0x59, 0xb2, 0x79, 0xf2, 0xf9, 0xef, 0xc3, 0x9b, 0x2b, 0x56, 0xac, 0x45, 0x8a, 0x09,  // D
0x12, 0x24, 0x48, 0x90, 0x3d, 0x7a, 0xf4, 0xf5, 0xf7, 0xf3, 0xfb, 0xeb, 0xcb, 0x8b, 0x0b, 0x16,  // E
0x2c, 0x58, 0xb0, 0x7d, 0xfa, 0xe9, 0xcf, 0x83, 0x1b, 0x36, 0x6c, 0xd8, 0xad, 0x47, 0x8e, 0x01 };// F

const unsigned char CxDecoderGRS_QR::m_cGLog[256] = {
//0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
0x00, 0x00, 0x01, 0x19, 0x02, 0x32, 0x1a, 0xc6, 0x03, 0xdf, 0x33, 0xee, 0x1b, 0x68, 0xc7, 0x4b,  // 0
0x04, 0x64, 0xe0, 0x0e, 0x34, 0x8d, 0xef, 0x81, 0x1c, 0xc1, 0x69, 0xf8, 0xc8, 0x08, 0x4c, 0x71,  // 1
0x05, 0x8a, 0x65, 0x2f, 0xe1, 0x24, 0x0f, 0x21, 0x35, 0x93, 0x8e, 0xda, 0xf0, 0x12, 0x82, 0x45,  // 2
0x1d, 0xb5, 0xc2, 0x7d, 0x6a, 0x27, 0xf9, 0xb9, 0xc9, 0x9a, 0x09, 0x78, 0x4d, 0xe4, 0x72, 0xa6,  // 3
0x06, 0xbf, 0x8b, 0x62, 0x66, 0xdd, 0x30, 0xfd, 0xe2, 0x98, 0x25, 0xb3, 0x10, 0x91, 0x22, 0x88,  // 4
0x36, 0xd0, 0x94, 0xce, 0x8f, 0x96, 0xdb, 0xbd, 0xf1, 0xd2, 0x13, 0x5c, 0x83, 0x38, 0x46, 0x40,  // 5
0x1e, 0x42, 0xb6, 0xa3, 0xc3, 0x48, 0x7e, 0x6e, 0x6b, 0x3a, 0x28, 0x54, 0xfa, 0x85, 0xba, 0x3d,  // 6
0xca, 0x5e, 0x9b, 0x9f, 0x0a, 0x15, 0x79, 0x2b, 0x4e, 0xd4, 0xe5, 0xac, 0x73, 0xf3, 0xa7, 0x57,  // 7
0x07, 0x70, 0xc0, 0xf7, 0x8c, 0x80, 0x63, 0x0d, 0x67, 0x4a, 0xde, 0xed, 0x31, 0xc5, 0xfe, 0x18,  // 8
0xe3, 0xa5, 0x99, 0x77, 0x26, 0xb8, 0xb4, 0x7c, 0x11, 0x44, 0x92, 0xd9, 0x23, 0x20, 0x89, 0x2e,  // 9
0x37, 0x3f, 0xd1, 0x5b, 0x95, 0xbc, 0xcf, 0xcd, 0x90, 0x87, 0x97, 0xb2, 0xdc, 0xfc, 0xbe, 0x61,  // A
0xf2, 0x56, 0xd3, 0xab, 0x14, 0x2a, 0x5d, 0x9e, 0x84, 0x3c, 0x39, 0x53, 0x47, 0x6d, 0x41, 0xa2,  // B
0x1f, 0x2d, 0x43, 0xd8, 0xb7, 0x7b, 0xa4, 0x76, 0xc4, 0x17, 0x49, 0xec, 0x7f, 0x0c, 0x6f, 0xf6,  // C
0x6c, 0xa1, 0x3b, 0x52, 0x29, 0x9d, 0x55, 0xaa, 0xfb, 0x60, 0x86, 0xb1, 0xbb, 0xcc, 0x3e, 0x5a,  // D
0xcb, 0x59, 0x5f, 0xb0, 0x9c, 0xa9, 0xa0, 0x51, 0x0b, 0xf5, 0x16, 0xeb, 0x7a, 0x75, 0x2c, 0xd7,  // E
0x4f, 0xae, 0xd5, 0xe9, 0xe6, 0xe7, 0xad, 0xe8, 0x74, 0xd6, 0xf4, 0xea, 0xa8, 0x50, 0x58, 0xaf };// F
