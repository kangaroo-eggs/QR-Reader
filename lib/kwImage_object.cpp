#include <iostream>
#include <memory.h>

#include "kwImage_object.h"

using namespace std;


//-------------------------------------
kwImageU8::kwImageU8( const int nRowNum, const int nColNum , const bool clear)
{
    if ( nRowNum <= 0 || nColNum <= 0 ){ // return NULL image
        this->rows = this->cols = 1;
        this->membase = 0;
        return;
    }
    else
        this->rows = nRowNum, this->cols = nColNum;

    this->numOfPixels = nRowNum*nColNum;
    this->membase = new kwPixel[this->numOfPixels];     // allocate 的 mem 是亂數的
    this->memend = this->membase + this->numOfPixels - 1;
    this->imgElement = new kwPixel*[rows];
    if( clear == true )
        memset( membase, 0, rows*cols*sizeof(kwPixel) );  // 將整個 image 的值清成 0
    kwPixel* tmp = membase;
    for ( int i = 0; i < rows; ++i, tmp += cols ){    // tmp 在做加法時會自動辨別 tmp 的 type，每次都是加 cols 個 kwPixel*
        this->imgElement[i] = tmp;
    }
}

//-------------------------------------
kwImageU8::kwImageU8( const kwImageU8& imCopyFrom )
{
    this->rows = imCopyFrom.rows, this->cols = imCopyFrom.cols, this->numOfPixels = imCopyFrom.numOfPixels;
    this->membase = new kwPixel[this->numOfPixels];
    this->memend = this->membase + this->numOfPixels - 1;   // 實際上加 (this->numOfPixels - 1)*1 byte
    this->imgElement = new kwPixel*[this->rows];
    memcpy(this->membase, imCopyFrom.membase, imCopyFrom.numOfPixels * sizeof(kwPixel));    // copy
    kwPixel *ptr = this->membase;
    for (int i = 0; i < rows; ++i, ptr += cols ){
        this->imgElement[i] = ptr;
    }
}

//-------------------------------------
kwImageU8::~kwImageU8()
{
    if( this->membase != nullptr ){   // 因為 0 平常是用不到的(OS在用)，所以用 0 來表示這個指標目前沒指到東西，來防止 delete 掉沒用到的 mem space
        delete [] this->membase;
    }
    if( this->imgElement != nullptr ){
        delete [] this->imgElement;
    }
}

//-------------------------------------
bool kwImageU8::IsNull()
{
    return this->membase == 0 ? true : false;
}

//-------------------------------------
bool kwImageU8::SetSize( int nRowNum, int nColNum, bool clear)
{
    if ( nRowNum <= 0 || nColNum <= 0 )
           return false;
    if ( nRowNum == this->rows && nColNum == this->cols ){
        if (clear)
            memset( this->membase, 0, this->numOfPixels * sizeof(kwPixel) );
        return true;
    }

    this->rows = nRowNum, this->cols = nColNum, this->numOfPixels = nRowNum * nColNum;
    if ( this->membase != 0 )
        delete [] this->membase;
    this->membase = new kwPixel[this->numOfPixels];
    this->memend = this->membase + this->numOfPixels - 1;
    if (clear)
        memset( this->membase, 0, this->numOfPixels * sizeof(kwPixel) );
    if ( this->imgElement != 0 )
        delete [] this->imgElement;
    this->imgElement = new kwPixel*[this->rows];
    kwPixel* bp = this->membase;
    for ( int i=0; i < this->rows; ++i, bp += this->cols )
        this->imgElement[i] = bp;
    return true;
}

//-------------------------------------
bool kwImageU8::SetSize( kwImageU8& Refimg, bool clear)
{
    return SetSize(Refimg.rows, Refimg.cols, clear);
}

//-------------------------------------
kwImageU8& kwImageU8::operator=(const kwImageU8& imCopyFrom)
{
    if( this != &imCopyFrom ){
        // reallocate mem
        if( this->cols != imCopyFrom.cols || this->rows != imCopyFrom.rows ){
            delete [] this->membase;    // 因為原來可能有資料，不 delete 的話，會發生 memory leak
            this->numOfPixels = imCopyFrom.numOfPixels;
            this->membase = new kwPixel[this->numOfPixels];
            this->memend = this->membase + this->numOfPixels - 1;
            if( this->rows != imCopyFrom.rows ){
                delete [] this->imgElement;
                this->imgElement = new kwPixel*[rows];
            }
            this->cols = imCopyFrom.cols, this->rows = imCopyFrom.rows;

            kwPixel* ptr = this->membase;
            for( int i = 0; i < rows; ++i, ptr+=cols ){
                this->imgElement[i] = ptr;
            }
        }
        // copy the content of image
        memcpy( this->membase, imCopyFrom.membase, this->numOfPixels*sizeof(kwPixel) );
    }
    return *this;
}

//-------------------------------------
kwImageU8 kwImageU8::operator+( const kwImageU8& imAdded )
{
    if( this->cols != imAdded.cols || this->rows != imAdded.rows ){
        cout << "wrong size, return scalar 0" << endl;
        kwImageU8 Resultimg(1,1);
        return Resultimg;    // return copy, will call constructor
    }

    kwImageU8 ResultImg( this->rows, this->cols );
    for (int i = 0; i < rows; ++i)
        for(int j = 0; j < cols; ++j)
            ResultImg.imgElement[i][j] = this->imgElement[i][j] + imAdded.imgElement[i][j];
    return ResultImg;
}

//-------------------------------------////
kwImageU8 kwImageU8::operator-(const kwImageU8& imSubtracted)
{
    if ( this->rows != imSubtracted.rows || this->cols != imSubtracted.cols ){
        kwImageU8 Resultimg(1,1);
        return Resultimg;
    }

    kwImageU8 Resultimg(this->rows,this->cols);
    for (int i=0; i < this->rows; ++i)
        for (int j=0; j < this->cols; ++j)
            Resultimg.imgElement[i][j] = this->imgElement[i][j] - imSubtracted.imgElement[i][j];
    return Resultimg;
}

//-------------------------------------////
kwImageU8 kwImageU8::operator*(const float fmult)
{
    kwImageU8 Resultimg(*this);
    for (int r=0; r < this->rows; ++r)
        for (int c=0; c < this->cols; ++c)
            Resultimg.imgElement[r][c] = (kwPixel)( (float)Resultimg.imgElement[r][c] * fmult + 0.5f );
    return Resultimg;
}

//-------------------------------------////
kwImageU8 kwImageU8::operator!()
{
    kwImageU8 imResult( this->cols, this->rows, false );
    kwPixel* pSrc = this->membase;
    kwPixel* pDst = imResult.membase;
    for ( ; pSrc <= this->memend; )
        (*(pDst++)) = 255 - (*(pSrc++));    // 先 *pDst, 再 pDst++
    return imResult;
}
