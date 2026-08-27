#include "kwImageTool.h"

//-------------------------------------
void kwImageTools::ResampleSubpixel( kwImageU8& imImage, kwPoint ptXY[], float GrayLevels[], const int nLength,
                       float (*Approxmethod)(kwImageU8&, kwPoint) ){
    for ( int i = 0; i < nLength; ++i ){
        GrayLevels[i] = Approxmethod(imImage, ptXY[i]);
    }
}

//-------------------------------------
float kwImageTools::SubPixel_Method_NearestNeighbor(kwImageU8& img, kwPoint pt){  //要注意 pt 的座標是 xy 座標系；img 是 row,col 座標系
    int leftcoord = floor(pt.x), upcoord = floor(pt.y);
    if(leftcoord < 0)
        leftcoord = 0;
    else if(leftcoord > img.cols-2) // rightcoord = img.cols-1
        leftcoord = img.cols-2;
    if(upcoord < 0)
        upcoord = 0;
    else if(upcoord > img.rows-2)   // downcoord = img.rows-1
        upcoord = img.rows-2;

    int x, y;
    float midx = leftcoord+0.5, midy = upcoord+0.5;
    // 看點靠左還是靠右
    if(pt.x < midx)
        x = leftcoord;
    else
        x = leftcoord+1;
    if(pt.y < midy)
        y = upcoord;
    else
        y = upcoord+1;
    // cout << y << ','<< x << ", value:" << float(img.imgElement[y][x]) << endl;
    return float(img.imgElement[y][x]);  // 注意到從 image 取出 pixel value 要用 [y][x]
}

//-------------------------------------
float kwImageTools::SubPixel_Method_InverseDistanceWeighting(kwImageU8& img, kwPoint pt){
    int leftcoord = floor(pt.x), upcoord = floor(pt.y);
// case: point is out of img
    bool OutOfBound = false;
    if(leftcoord < 0){
        leftcoord = 0;
        OutOfBound = true;
    }
    else if(leftcoord > img.cols-2){    // rightcoord = img.cols-1
        leftcoord = img.cols-1;
        OutOfBound = true;
    }
    if(upcoord < 0){
        upcoord = 0;
        OutOfBound = true;
    }
    else if(upcoord > img.rows-2){  // downcoord = img.rows-1
        upcoord = img.rows-1;
        OutOfBound = true;
    }
    if(OutOfBound)
        return  (float)img.imgElement[upcoord][leftcoord];

// case: points are all within img
    // direct return img's pixel
    if(pt.x == int(pt.x) && pt.y == int(pt.y))
        return (float)img.imgElement[int(pt.y)][int(pt.x)];

    kwPoint pt1(leftcoord, upcoord), pt2(leftcoord+1, upcoord), pt3(leftcoord+1, upcoord+1), pt4(leftcoord, upcoord+1);
    float dist1, dist2, dist3, dist4, subpixelvalue;

    float d1 = pt.DistanceTo(pt1), d2 = pt.DistanceTo(pt2), d3 = pt.DistanceTo(pt3), d4 = pt.DistanceTo(pt4);
    dist1 = (d1 > 1e-6f) ? 1.0f / d1 : 0.0f;
    dist2 = (d2 > 1e-6f) ? 1.0f / d2 : 0.0f;
    dist3 = (d3 > 1e-6f) ? 1.0f / d3 : 0.0f;
    dist4 = (d4 > 1e-6f) ? 1.0f / d4 : 0.0f;

    // 加權算出 pixel value
    subpixelvalue = dist1 * img.imgElement[(int)pt1.y][(int)pt1.x] +
               dist2 * img.imgElement[(int)pt2.y][(int)pt2.x] +
               dist3 * img.imgElement[(int)pt3.y][(int)pt3.x] +
               dist4 * img.imgElement[(int)pt4.y][(int)pt4.x];
    float weightSum = dist1 + dist2 + dist3 + dist4;
    if (weightSum == 0)
        return (float)img.imgElement[upcoord][leftcoord];
    subpixelvalue /= weightSum;
    // cout << "value:" << subpixel << endl;
    return subpixelvalue;
}


//-------------------------------------
void kwImageTools::Histogram(kwImageU8& Source, int nHistogram[]) {
    memset(nHistogram, 0, 256 * sizeof(int));
    for (int i = 0; i < Source.rows; ++i) {
        for (int j = 0; j < Source.cols; ++j) {
            nHistogram[(int)Source.imgElement[i][j]]++;
        }
    }
}

//-------------------------------------
void kwImageTools::Histogram(kwPixel membase[], int length, int nHistogram[]) {   // array[] 表示用 membase[]
    memset(nHistogram, 0, 256 * sizeof(int));
    for (int i = 0; i < length; i++) {
        nHistogram[(int)membase[i]]++;
    }
}

//-------------------------------------
//bool Histogram_Equality(kwImage& imSource, kwImage& imDestination) {
//  if (imDestination.m_nColNum != imSource.m_nColNum ||
//      imDestination.m_nRowNum != imSource.m_nRowNum) {
//    return false;
//  } else {
//    int imHistogram[256], PrefixPixel[256], graylevel, pixel, totalpixel;
//    memset(PrefixPixel, 0, 256 * sizeof(int));
//    Histogram(imSource, imHistogram);
//    PrefixPixel[0] = imHistogram[0];

//    for (int i = 1; i <= 255; i++) {
//      PrefixPixel[i] = imHistogram[i] + PrefixPixel[i - 1];
//    }
//    totalpixel = PrefixPixel[255];

//    for (int i = 0; i < imSource.m_nRowNum; i++) {
//      for (int j = 0; j < imSource.m_nColNum; j++) {
//        pixel = (imSource.m_pMatrixElement[i][j] + 0.5 < 255)
//                    ? (int)(imSource.m_pMatrixElement[i][j] + 0.5)
//                    : 255;
//        graylevel = (int)((((float)PrefixPixel[pixel]) / (float)totalpixel) * 255);
//        imDestination.m_pMatrixElement[i][j] = (unsigned char)graylevel;
//      }
//    }
//    return true;
//  }
//}

//-------------------------------------
//bool Histogram_Stretch(kwImage& imSource, kwImage& imDetination, unsigned char LowBound,
//                       unsigned char GreatBound) {
//  if (imDetination.m_nColNum != imSource.m_nColNum ||
//      imDetination.m_nRowNum != imSource.m_nRowNum) {
//    return false;
//  } else {
//    float slope, L;  //     255/(G-L)*(X-L)=Y
//    L = (float)LowBound;
//    slope = 255 / ((float)GreatBound - (float)LowBound);
//    for (int i = 0; i < imSource.m_nRowNum; i++) {
//      for (int j = 0; j < imSource.m_nColNum; j++) {
//        imDetination.m_pMatrixElement[i][j] =
//            (unsigned char)slope * (imSource.m_pMatrixElement[i][j] - L);
//      }
//    }
//    return true;
//  }
//}

//-------------------------------------
kwPixel kwImageTools::Threshold2Otsu(kwPixel ImageData[], int Length) {
    int imgHistogram[256], PrefixPixelNum[256];
    kwImageTools::Histogram(ImageData, Length, imgHistogram);
    PrefixPixelNum[0] = imgHistogram[0];

    float mu1[256], mu2[256], AveragePixelVal, TotalPixelVal, VarTemp, BetweenClassVariance = 0, group1_portion;
    memset(mu1, 0, 256 * sizeof(float));
    memset(mu2, 0, 256 * sizeof(float));
    kwPixel OstuValue = 0;
    // 算 Group1Pixel, means threshold == i 時，group1 的 expected value
    for (int i = 1; i < 256; ++i) {
        PrefixPixelNum[i] = imgHistogram[i] + PrefixPixelNum[i - 1];
        if (PrefixPixelNum[i] == 0) {
            mu1[i] = 0;
        } else {
            mu1[i] = ( mu1[i - 1] * PrefixPixelNum[i - 1] + imgHistogram[i] * i ) / PrefixPixelNum[i];    // probability
        }
    }
    AveragePixelVal = mu2[0] = mu1[255];
    TotalPixelVal = mu1[255] * PrefixPixelNum[255];

    for (int i = 1; i < 256; ++i) {
        // 算 Group2Pixel, means threshold == i 時，group2 的 expected value
        if (PrefixPixelNum[255] == PrefixPixelNum[i]) { // 代表沒有比 i 更高的 pixel value 了
            mu2[i] = 0;
        } else {
            mu2[i] = ( TotalPixelVal - mu1[i] * PrefixPixelNum[i] ) / ( PrefixPixelNum[255] - PrefixPixelNum[i] );  // i+1 ~ 255
        }
        group1_portion = (float)PrefixPixelNum[i] / (float)PrefixPixelNum[255];
        VarTemp = ( mu1[i] - AveragePixelVal ) * ( mu1[i] - AveragePixelVal ) * group1_portion +
                  ( mu2[i] - AveragePixelVal ) * ( mu2[i] - AveragePixelVal ) * ( 1 - group1_portion );
        if (VarTemp > BetweenClassVariance) {
            BetweenClassVariance = VarTemp;
            OstuValue = (kwPixel)i;
        }
    }
    return OstuValue;
}

//-------------------------------------
kwPixel kwImageTools::Threshold2Otsu(float floatData[], int Length) {
    kwPixel* Data = new kwPixel[Length];
    float temp;
    // 轉換型別
    for (int i = 0; i < Length; i++) {
        temp = floatData[i] + 0.5;
        Data[i] = (temp < 255) ? (kwPixel)temp : (unsigned char)255;
    }
    kwPixel OtsuValue = kwImageTools::Threshold2Otsu(Data, Length);
    delete[] Data;
    return OtsuValue;
}

//-------------------------------------
//kwImage Threshold2Otsu_Output(kwImage& InputImage) {
//  unsigned char OtsuValue;
//  kwImage OtsuImage(InputImage.m_nRowNum, InputImage.m_nColNum);
//  OtsuValue = Threshold2Otsu(InputImage.m_pMemoryBase, InputImage.m_nColNum * InputImage.m_nRowNum);
//  for (int i = 0; i < InputImage.m_nRowNum; i++) {
//    for (int j = 0; j < InputImage.m_nColNum; j++) {
//      OtsuImage.m_pMatrixElement[i][j] = (InputImage.m_pMatrixElement[i][j] <= OtsuValue) ? 0 : 255;
//    }
//  }
//  return OtsuImage;
//}

