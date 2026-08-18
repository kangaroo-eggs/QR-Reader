//#ifndef KWROI_H
//#define KWROI_H

//#include <main.h>
//#include <kwImage_object.h>
//#include <kwPoint.h>

////==============================================================
//// ROI definition of logic region for further processing
////   - need to attach to a real image for real pixel reference
////==============================================================

//class kwRoi
//{
//public:
////    kwPointInt              m_ptiOrigin;
//    int                     m_nCols;
//    int                     m_nRows;
//    kwImageU8*              m_pImage;

//    //-------------------------------
//    float                   fRanking;    // a measure used for sorting
//    kwRoi*                  pNext;
//    kwRoi*                  pPrev;

//    //====================================================================
//                            kwRoi();
//                            kwRoi( kwImageU8& Image );
//                            kwRoi( kwImageU8& Image, int OrgX, int OrgY, int height, int width );
//                            ~kwRoi();

//    void                    Clear();
//    void                    SetRegion( int orgx, int orgy, int height, int width );
//    void                    AttachImage( kwImageU8& img, int orgx, int orgy, int height, int width );
//    void                    AttachImage( kwImageU8& img );
////    inline bool             IsValid()
////                            {   return ( (m_pImage!=0) && (m_ptiOrigin.x>=0) && (m_ptiOrigin.y>=0) &&
////                                         ( m_ptiOrigin.x+m_nCols <= m_pImage->m_nColNum ) &&
////                                         ( m_ptiOrigin.y+m_nRows <= m_pImage->m_nRowNum ) );    }
//    inline bool             IsContaining( kwPoint point )
//                            {  return ( point.x >= 0.0f && point.x <= (this->m_nCols-1.0f) && point.y >= 0.0f && point.y <= (this->m_nRows-1.0f) );  }

//    float                   GetSubPixel( kwPoint ptPosition );
//    float                   GetSubPixel( float fX, float fY );
//    uint8_t                 GetPixel( int x, int y );
//    uint8_t                 GetPixel( float x, float y );


//    void                    SetPixel( int x, int y, uint8_t val );
////    kwPointInt              SetOrigin( int orgx, int orgy );
//    void                    SetSize( int height, int width );

//    kwPointPair             LineIntersection( kwPointPair ptpLineToIntersect);
//    kwPointPair             LineIntersection( kwPoint ptBase, kwPoint vcDirection);

//    inline bool             IsNull()
//                            {  return (this->m_nRows==0 || this->m_nCols==0);  }

//    bool                    IsOutsideImageBoundary(kwPoint ptPointInROI);

//    void                    ToImageU8( kwImageU8& imDest );
//    inline void             operator>>( kwImageU8& imDest )
//                            {  this->ToImageU8( imDest );  }

//    void                    FromImageU8( kwImageU8& imSource );
//    inline void             operator<<( kwImageU8& imSource )
//                            {  this->FromImageU8( imSource );  }

//    kwRoi&                  operator=( const kwRoi& srcROI );
//};

//#endif // KWROI_H
