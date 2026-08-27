#ifndef KWFRAME_H
#define KWFRAME_H

#include "kwCommon.h"
#include "kwImageTool.h"
#include "kwMsrPoint.h"
#include "kwPoint.h"

/////////////////////////////////////////////////////////////////////////////////
/// \brief 作用是在點的周圍畫出 frame, 便於觀察
///
class kwFrame : public kwPointInt
{
public:
    enum HitStatus
    {
        NONE = 0,
        HITTED = 1,
    };

public:
    kwFrame();

    int         HitTest(int HitX, int HitY);
    void        ReleaseHitStatus();
    void        DragFrame(int HitX, int HitY);
    /// @param DstMat is opencv's image container
    void        DrawFrame(cv::Mat &DstMat);

private:
    HitStatus   bHitFlag = NONE;
    int         HitAcceptRange = 5;
};

#endif // KWFRAME_H
