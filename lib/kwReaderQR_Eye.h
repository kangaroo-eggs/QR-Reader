#ifndef KWREADERQR_Eye_H
#define KWREADERQR_Eye_H

#include "kwImageTool.h"
#include "kwTransPerspective.h"
#include "kwMsrPoint.h"
#include "kwMatrix.h"
#include "kwPoint.h"
//#include "kwDecoderQR.h"


///////////////////////////////////////////////////////////////////////
/// \brief Data Structure for a Eye
///
class kwReaderQR_Eye
{
public:
    /// @brief This eye's center position.
    kwPoint                 ptCenter;
    /// @brief This eye's horizontal unit module width in pixels.
    float                   eyeWidth_H;
    /// @brief This eye's vertical unit module width in pixels.
    float                   eyeWidth_V;
    /// @brief This eye is a reversed or not.
    bool                    beReversed;

    float                   ranking;

    //-------------------------------------
    kwReaderQR_Eye();
    kwReaderQR_Eye( kwReaderQR_Eye &copyfrom );
    ~kwReaderQR_Eye();
    void                    Clear();
    bool                    IsNull();

    //-------------------------------------
    kwReaderQR_Eye&         operator=( const kwReaderQR_Eye &fiFrom );
};


///////////////////////////////////////////////////////////////////////
/// \brief
///
class kwReaderQR_EyeSet {
 public:
  /// @brief three Eye  0 1
  ///                   2
  kwReaderQR_Eye            Eye0;
  kwReaderQR_Eye            Eye1;
  kwReaderQR_Eye            Eye2;

  float                     Rank;

  //-------------------------------------
  ///@brief Constructor and Destructor
  kwReaderQR_EyeSet();
  kwReaderQR_EyeSet( kwReaderQR_EyeSet &copyfrom );
  ~kwReaderQR_EyeSet();
  ///@brief set the Eye set to NULL
  void                      Clear();
  ///@brief Determine the Eye set is NULL
  bool                      IsNull();
  /// @brief Set the Eye;
  void                      SetEye(kwReaderQR_Eye _Eye0, kwReaderQR_Eye _Eye1,
                                      kwReaderQR_Eye _Eye2);

  //-------------------------------------
  void                      operator()(kwReaderQR_Eye _Eye0, kwReaderQR_Eye _Eye1,
                                       kwReaderQR_Eye _Eye2);
};

#endif // KWREADERQR_Eye_H
