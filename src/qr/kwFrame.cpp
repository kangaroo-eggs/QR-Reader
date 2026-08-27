#include "kwFrame.h"

//-------------------------------------------------
kwFrame::kwFrame()  {}

//-------------------------------------------------
int kwFrame::HitTest(int HitX, int HitY)
{
    if( abs(this->x - HitX) < HitAcceptRange && abs(this->y - HitY) < HitAcceptRange )
    {
        bHitFlag = HITTED;
        return bHitFlag;
    }
    else{
        bHitFlag = NONE;
        return bHitFlag;
    }
}

//-------------------------------------------------
void kwFrame::ReleaseHitStatus()
{
    bHitFlag = NONE;
}

//-------------------------------------------------
void kwFrame::DragFrame(int HitX, int HitY)
{
    if(bHitFlag != HITTED)
        return;
    this->x = HitX;
    this->y = HitY;
    return;
}

//-------------------------------------------------
void kwFrame::DrawFrame(cv::Mat& DstMat)
{
    // set frame
    cv::Point FrameP1(this->x - HitAcceptRange, this->y - HitAcceptRange);
    cv::Point FrameP2(this->x + HitAcceptRange, this->y - HitAcceptRange);
    cv::Point FrameP3(this->x + HitAcceptRange, this->y + HitAcceptRange);
    cv::Point FrameP4(this->x - HitAcceptRange, this->y + HitAcceptRange);

    // draw frame by line()
//    cv::line(DstMat, FrameP1, FrameP2, cv::Scalar(255,0,0));
//    cv::line(DstMat, FrameP2, FrameP3, cv::Scalar(255,0,0));
//    cv::line(DstMat, FrameP3, FrameP4, cv::Scalar(255,0,0));
//    cv::line(DstMat, FrameP4, FrameP1, cv::Scalar(255,0,0));
    cv::line(DstMat, FrameP1, FrameP3, cv::Scalar(0,255,0));
    cv::line(DstMat, FrameP2, FrameP4, cv::Scalar(0,255,0));

    return;
}
