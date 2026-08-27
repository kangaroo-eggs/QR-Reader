#ifndef KWREADERQR_H
#define KWREADERQR_H

#include "kwImageTool.h"
#include "kwTransPerspective.h"
#include "kwMsrPoint.h"
#include "kwMatrix.h"
#include "kwPoint.h"
#include "kwReaderQR_Eye.h"
#include "kwList.h"
#include "kwLine.h"
#include "kwDecoderQR.h"

///////////////////////////////////////////////////////////////////////
/// \brief The kwReaderQR class
///
class kwReaderQR
{
public:
    /// @brief Perspective transformer back and forth.
    kwTransPerspective          transPerspective;

    /// @brief Edge detector : float version.
    kwMsrPoint                  pointMsrTool;

    /// @brief QR decoder
    kwDecoderQR                 decoderQR;

    /// @brief reference system: Horizontal unit vector: Eye0 -> Eye1.
    /// i.e. 一個 module 幾個 pixel (in float)
    kwPoint                     unitModuleVectorH;
    /// @brief reference system: Vertical unit vector: Eye0 -> Eye2.
    kwPoint                     unitModuleVectorV;

    /// @brief detected Version #.
    int                         symbolVersion;
    /// @brief detected Error Correction Code #Level.   L:01, M:00, Q:11, H:10
    int                         symbolECCLevel;
    /// @brief detected Symbol Masking Pattern#.
    int                         symbolMaskPattern;

    /// @brief detected Symbol width (21~177 modules).
    int                         symbolWidthInModules;
    /// @brief detected Symbol height (21~177 modules).
    int                         symbolHeightInModules;

    /// @brief 3 Eyes' information.
    kwReaderQR_Eye              symbolEyes[3];

    /// @brief The format infomation table
    unsigned short int FormatTable[32] = {
        0x5412, 0x5125, 0x5E7C, 0x5B4B, 0x45F9, 0x40CE, 0x4F97, 0x4AA0, 0x77C4, 0x72F3, 0x7DAA,
        0x789D, 0x662F, 0x6318, 0x6C41, 0x6976, 0x1689, 0x13BE, 0x1CE7, 0x19D0, 0x0762, 0x0255,
        0x0D0C, 0x083B, 0x255F, 0x3068, 0x3F31, 0x3A06, 0x24B4, 0x2183, 0x2EDA, 0x2BED};

    //-------------------------------------
    kwReaderQR();
    ~kwReaderQR();

    //-------------------------------------
    // reading this barcode
    /// @brief for a given kwImageU8, conducts QR symbol's locating and decoding, and returns the length of decoded data(in chars), 0 if non-decodable.
    int                         DoReading( kwImageU8 &imImage, unsigned char DecodedData[] );

    /// @brief search patter on a given line and ... them.
    void                        LineScanning_FindPatternEyes(kwImageU8 &imImage, kwPoint ptBegin, kwPoint ptEnd,
                                                            float bias, kwList<kwPointPairNode> &ptplEyetwobounded);

    /// @brief prepare valid Eye sets and retrieve format/version for 3 or more Eyes.
    void                        PreparingEyeSets_AllEyes(kwImageU8 imImage, kwList<kwReaderQR_Eye> &lfEyeCandidates,
                                                            kwList<kwReaderQR_EyeSet> &lfsCandidateEye);

    /// @brief retrieve version info from modules given a Eye set.
    /// this is done by calculation, not decode by Version information.
    int                         GetVersionInfo(kwImageU8 &imImage, kwReaderQR_EyeSet &fsEyeSet);

    /// @brief retrieve format info moduled given a Eye set.
    bool                        GetFormatInfo(kwImageU8 &imImage, kwReaderQR_EyeSet &thisEyeSet);

    /// @brief register alignments,resample modules (enen blockwise),threshhold,retrieve
    /// codewords,codeword corrected by Reed-Solomon code decode back to source data
    int                         DecodingEyeSets(kwImageU8 &imImage, kwReaderQR_EyeSet &lfsListOfEyeSets,
                                                    unsigned char cDecodedData[]);
    /// @brief Find the fourth Eye
    kwPoint                     Get4thCalibrateEye(kwImageU8& imImage, kwReaderQR_EyeSet& fsFinderSet);
};


#endif // KWREADERQR_H
