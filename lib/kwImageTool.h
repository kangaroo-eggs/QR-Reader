#ifndef KWIMAGETOOL_H
#define KWIMAGETOOL_H

#include "main.h"
#include "kwMatrix.h"
#include "kwImage_object.h"
#include "kwPoint.h"

/////////////////////////////////////////////////////////////////////////////////
/// \brief The kwImageTools class
///
class kwImageTools
{
public:
    /// @brief Given points coordinate(ptXY[]), Resampling these point's pixel value from imImage and put into GrayLevels[]
    static void         ResampleSubpixel( kwImageU8 &imImage, kwPoint ptXY[], float fGrayLevels[], const int nLength,
                                          float (*Approxmethod)(kwImageU8 &, kwPoint) );


    static float        SubPixel_Method_NearestNeighbor(kwImageU8 &img, kwPoint pt);
    float               SubPixel_Method_BiLinear(kwImageU8 &img, kwPoint pt);
    float               SubPixel_Method_Quadratic(kwImageU8 &img, kwPoint pt);
    float               SubPixel_Method_Cubic(kwImageU8 &img, kwPoint pt);
    static float        SubPixel_Method_InverseDistanceWeighting(kwImageU8 &img, kwPoint pt);


    /// @brief Thresholding.
    static void         Thresholding( kwImageU8 &imInput, kwImageU8 &imOutput, const uint8_t nThreshold, const uint8_t nBinaryHigh, const uint8_t nBinaryLow );

    /// @brief Edge Detection.
    static void         Sobel( kwImageU8 &imImageIn, kwImageU8 &imImageOut, uint32_t nThreshold );
    static void         Canny( kwImageU8 &imImageIn, kwImageU8 &imImageOut, uint8_t cThreshold );
    static void         LaplacianOfGaussian( kwImageU8 &imImageIn, kwImageU8 &imImageOut, uint32_t nThreshold );
    static void         Gradient( kwImageU8 &imInput, kwImageU8 &mxOutput );

    /// @brief Corner Detection.
    static void         Corner_Harris( kwImageU8 &imImageIn, kwImageU8 &imImageOut, int nWindowSize, int nApertureSize, float fParamK );

    /// @brief Histograms.
    static void         GetHistogram( kwImageU8 &imImageIn, int nHistogram[] );

    static bool         Conversion_Gamma( kwImageU8 &imImageIn, kwImageU8 &imImageOut, float fGamma );

    static bool         Conversion_Equalization( kwImageU8 &imImageIn, kwImageU8 &imImageOut );

    static bool         Conversion_Stretch( kwImageU8 &imImageIn, kwImageU8 &imImageOut, uint8_t cBoundLow, uint8_t cBoundHigh );

    /// @brief Erode/Dilate omni-direction 3x3.
    static void         Erode3x3_Graylevel( kwImageU8 &imImageIn, kwImageU8 &imImageOut );
    static void         Dilate3x3_Graylevel( kwImageU8 &imImageIn, kwImageU8 &imImageOut );

    /// @brief Upsample/Downsample
    static void         UpsampleBy2( kwImageU8 &imImageIn, kwImageU8 &imImageOut );
    static void         DownsampleBy2( kwImageU8 &imImageIn, kwImageU8 &imImageOut );

    /// @brief Binarizing.
    static bool         Binarizing( kwImageU8 &imImageIn, kwImageU8 &imImageOut, const uint8_t cThreshold );

    /// @brief ReSizing.
    static void         Resize( kwImageU8 &imImageIn, kwImageU8 &imImageOut );

    /// @brief Reversing
    static void         Reverse( kwImageU8 &imImage );

    /// @brief
    static kwPixel      Threshold2Otsu(kwPixel ImageData[], int Length);
    static kwPixel      Threshold2Otsu(float Data[], int Length);
    //
    /// @brief
    static void         Histogram(kwImageU8 &imSource, int nHistogram[]);
    static void         Histogram(kwPixel array[], int length, int nHistogram[]);
};


#endif // KWIMAGETOOL_H
