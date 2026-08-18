#ifndef KWMSRPOINT_H
#define KWMSRPOINT_H

#include "main.h"
#include "kwMatrix.h"
#include "kwRoi.h"
#include "kwImage_object.h"
#include "kwImageTool.h"
#include "kwPoint.h"

const int kw_MAXnumOfMsrPoint = 1000; // 最多可以測 1000 條邊
///////////////////////////////////////////////////////////////////////////
/// \brief The kwMsrPoint class
///

class kwMsrPoint
{
public:
    /// @brief Measuring start point.
    kwPoint                 ptStart;
    /// @brief Measuring stop point.
    kwPoint                 ptEnd;

    /// @brief the number of detected edge points.
    int                     numOfEdgePts;
    /// @brief positions(in float XY) of edge points.   at left side of the edge
    kwPoint                 *edgePts;
    /// @brief gradient(1st derivative) of pixel value, i.e. the estimations of the detected edge.
    float                   *edgeValue;
    /// @brief edge types, B->W...true(1); W->B...false(0).
    bool                    *edgeType;

//    /// @brief the number segments, i.e. the length between edges.
//    int                     m_nNumSegmentLengths;
//    /// @brief the length between edges, e.g. the length between the edges of m_ptEdgeXY[n] and m_ptEdgeXY[n+1] is in m_fSegmentLength[n].
//    float                   *segmentLength;

    //-------------------------------------
    kwMsrPoint();
    ~kwMsrPoint();
    void                    Clear();

    //-------------------------------------
    void                    SetMsrBounds( kwImageU8 &imImage, kwPoint ptMsrStart, kwPoint ptMsrEnd );

//    void                    SetMsrBounds( kwRoi& rRoi,
//                                          kwPoint ptMsrStart,
//                                          kwPoint ptMsrEnd );
    /// @brief discrete ver. for 1st order derivative
    float                   FirstOrderDerivative(float *Data, int DestinateDerive, int Length);

    /// @brief discrete ver. for 2nd order derivative, we don't directly use FirstOrderDerivative on Derivative_1st[]
    float                   SecondOrderDerivative(float *Data, int index, int Length);

};
#endif // KWMSRPOINT_H
