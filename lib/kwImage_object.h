#ifndef KWIMAGE_OBJECT_H
#define KWIMAGE_OBJECT_H

#include "kwConst.h"
//Note: the image read from opencv api, has size = m(Rows) by n(Cols)

/////////////////////////////////////////////////////////////////////////////////
/// @brief
///

class kwImageU8{    // Rows by Cols 的 unsigned char matrix
public:
    int                 rows;
    int                 cols;
    int                 numOfPixels;
    kwPixel             *membase;
    kwPixel             *memend;
    kwPixel*            *imgElement;

    //-------------------------------------
    // Default constructors
    kwImageU8( const int rowNum = 1, const int colNum = 1 ,const bool Clear = true );
    // Copy constructors
    kwImageU8( const kwImageU8 &copyFrom );
    // Destructor
    ~kwImageU8();

    // NULL image: membase = 0
    bool                IsNull();
    void                SetAll( float val );
    bool                SetSize( int rowNum, int colNum, bool Clear = true );
    bool                SetSize( kwImageU8 &refImg, bool Clear = true);

    //-------------------------------------
    kwImageU8&          operator=(const kwImageU8 &copyFrom);
    kwImageU8           operator+(const kwImageU8 &added);
    kwImageU8           operator-(const kwImageU8 &subtracted);
    kwImageU8           operator*(const float multiplier);
    kwImageU8           operator!();      // inverse Image(負片)

};

#endif // KWIMAGE_OBJECT_H
