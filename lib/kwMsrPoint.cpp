#include "kwMsrPoint.h"

kwMsrPoint::kwMsrPoint() {
    edgePts = new kwPoint[kw_MAXnumOfMsrPoint];
    edgeValue = new float[kw_MAXnumOfMsrPoint];
    edgeType = new bool[kw_MAXnumOfMsrPoint];
    numOfEdgePts = 0;
    ptEnd(0, 0);
    ptStart(0, 0);
}

//-------------------------------------
kwMsrPoint::~kwMsrPoint() {
    delete[] edgePts;
    delete[] edgeValue;
    delete[] edgeType;
}

//-------------------------------------
void kwMsrPoint::Clear() {
    memset(edgeType, 0, kw_MAXnumOfMsrPoint * sizeof(bool));
    memset(edgeValue, 0, kw_MAXnumOfMsrPoint * sizeof(float));
    memset(edgePts, 0, kw_MAXnumOfMsrPoint * sizeof(kwPoint));
    numOfEdgePts = 0;
    ptEnd(0, 0);
    ptStart(0, 0);
}

//-------------------------------------
/// @brief find the edges in this image
void kwMsrPoint::SetMsrBounds(kwImageU8& image, kwPoint ptMsrStart, kwPoint ptMsrEnd) {
    this->Clear();
    this->ptStart = ptMsrStart;
    this->ptEnd = ptMsrEnd;
    int     TotalPointNum = (int)ptStart.DistanceTo(ptEnd);  // <= start~end 實際經過的 pixel 數量 (這是可以選擇的，端看你想要多精準)
    float   deltaX = (ptMsrEnd.x - ptMsrStart.x) / TotalPointNum,
            deltaY = (ptMsrEnd.y - ptMsrStart.y) / TotalPointNum; // 每一次的位移

    kwPoint *PointArray = new kwPoint[TotalPointNum];
    float   *GraylevelArray = new float[TotalPointNum];

    // assign the Point's Position
    for (int i = 0; i < TotalPointNum; ++i) {
        PointArray[i].x = ptMsrStart.x + deltaX * i;
        PointArray[i].y = ptMsrStart.y + deltaY * i;
    }
    // use subpixel value for calculation each point's pixel value
    kwImageTools::ResampleSubpixel(image, PointArray, GraylevelArray, TotalPointNum, kwImageTools::SubPixel_Method_InverseDistanceWeighting);

    // Setting the Otsu data
    float   derivative_1st_Tmp, Derivative_2nd_now, Derivative_2nd_next;
    float   *Derivative_1st = new float[TotalPointNum];
    bool    *Derivative_1st_GraylevelType = new bool[TotalPointNum];

    for (int i = 0; i < TotalPointNum; ++i) {
        derivative_1st_Tmp = FirstOrderDerivative(GraylevelArray, i, TotalPointNum);
        // 等下檢查 Otsu value 時, 只需比較 Derivative_1st[i] > Otsu value 即可
        if (derivative_1st_Tmp >= 0){
          Derivative_1st[i] = derivative_1st_Tmp;
          Derivative_1st_GraylevelType[i] = true;    // B->W
        } else{
          Derivative_1st[i] = -derivative_1st_Tmp;
          Derivative_1st_GraylevelType[i] = false;   // W->B
        }
    }

    // Find Otsu Value, the threshold of the FirstOrderDerivtive ( the value of difference, 用來測量 pixel value 變化得多快 )
    // 也就是說要變化的夠快才會被視為 edge
    kwPixel     OtsuValue;
    OtsuValue = kwImageTools::Threshold2Otsu(Derivative_1st, TotalPointNum);

    for (int i = 1; i <= TotalPointNum-3; ++i) { // skip the index 0 and Length-1
        Derivative_2nd_now = SecondOrderDerivative(GraylevelArray, i, TotalPointNum);
        Derivative_2nd_next = SecondOrderDerivative(GraylevelArray, i + 1, TotalPointNum);
        if (Derivative_2nd_now * Derivative_2nd_next < 0 && Derivative_1st[i] > OtsuValue) {
            //if (Derivative_1st[i] + Derivative_1st[i + 1] > kw_Ostu_NOISE_LEVEL * (float)OtsuValue) {
                numOfEdgePts++;
                edgeValue[numOfEdgePts - 1] = Derivative_1st[i];
                edgeType[numOfEdgePts - 1] = Derivative_1st_GraylevelType[i];
                // 內插出反曲點(for 2nd order derivative)所在的座標, use generalized 內插法
                edgePts[numOfEdgePts - 1].x = (PointArray[i + 1].x * Derivative_2nd_now - PointArray[i].x * Derivative_2nd_next)
                                             / (Derivative_2nd_now - Derivative_2nd_next);
                edgePts[numOfEdgePts - 1].y = (PointArray[i + 1].y * Derivative_2nd_now - PointArray[i].y * Derivative_2nd_next)
                                             / (Derivative_2nd_now - Derivative_2nd_next);
            //}
        } else if (Derivative_2nd_now == 0 && Derivative_2nd_next != 0 && Derivative_1st[i] > OtsuValue) {
            //if (Derivative_1st[i] + Derivative_1st[i + 1] > kw_Ostu_NOISE_LEVEL * (float)OtsuValue) {
                numOfEdgePts++;
                edgeValue[numOfEdgePts - 1] = Derivative_1st[i];
                edgeType[numOfEdgePts - 1] = Derivative_1st_GraylevelType[i];
                // 直接找到反曲點
                edgePts[numOfEdgePts - 1].x = PointArray[i].x;
                edgePts[numOfEdgePts - 1].y = PointArray[i].y;
            //}
        } else if (Derivative_2nd_now != 0 && Derivative_2nd_next == 0 && Derivative_1st[i + 1] > OtsuValue) {  // special case for pt[length-2] is 反曲點
            //if (Derivative_1st[i] + Derivative_1st[i + 1] > kw_Ostu_NOISE_LEVEL * (float)OtsuValue) {
                numOfEdgePts++;
                edgeValue[numOfEdgePts - 1] = Derivative_1st[i + 1];
                edgeType[numOfEdgePts - 1] = Derivative_1st_GraylevelType[i + 1];
                // 直接找到反曲點
                edgePts[numOfEdgePts - 1].x = PointArray[i].x;
                edgePts[numOfEdgePts - 1].y = PointArray[i].y;
            //}
        }
    }

    delete[] PointArray;
    delete[] GraylevelArray;
    delete[] Derivative_1st;
    delete[] Derivative_1st_GraylevelType;
}

//-------------------------------------
float kwMsrPoint::FirstOrderDerivative(float* Data, int index, int Length) {
    if (index == 0)
        return Data[1] - Data[0];
    if (index == Length - 1)
        return Data[Length-1] - Data[Length-2];
    else
        return (Data[index + 1] - Data[index - 1]) / 2;
}

//-------------------------------------
float kwMsrPoint::SecondOrderDerivative(float* Data, int index, int Length) {
    if (index == 0 || index == 1)
        return Data[index+2] - 2*Data[index+1] + Data[index];
    if (index == Length - 2 || index == Length - 1 )
        return Data[Length-1] - 2*Data[Length-2] + Data[Length-3];
    else
        return (Data[index+2] - 2*Data[index] + Data[index-2]) / 4;
}

