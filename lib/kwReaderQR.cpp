#include "kwReaderQR.h"
#include "main.h"
const float cos15 = 0.96592582628;  // cos(15(degree)) = cos(pi/12(radius))
const float cos45 = 0.70710678118;

//-------------------------------------
kwReaderQR::kwReaderQR(){

}

//-------------------------------------
kwReaderQR::~kwReaderQR(){

}

//-------------------------------------
int kwReaderQR::DoReading(kwImageU8 &imImage, unsigned char DecodedText[]) {
    kwPoint ptStart, ptEnd;
    kwList<kwPointPairNode> lineCandidate;
    kwList<kwReaderQR_Eye> eyeCandidate;   // 可能會有重複的
    kwList<kwReaderQR_EyeSet> eyeSetCandidate;

    // Scan the image by Line and Insert all possible eye to the EyeCandidate
    for (int row = 0; row < imImage.rows; row += 2) {
        ptStart(0, row);
        ptEnd(imImage.cols - 1, row);
        this->LineScanning_FindPatternEyes(imImage, ptStart, ptEnd, 0.2, lineCandidate);
    }
//    ptStart(0, 23);   // ver3
//    ptEnd(imImage.cols-1, 23);
//    this->LineScanning_FindPatternEyes(imImage, ptStart, ptEnd, 0.2, lineCandidate);

    cout << "lineCandidate: " << lineCandidate.numOfListItems << endl;

    // Create eyeCandidate List
    kwNode<kwReaderQR_Eye> eyeNode;
    kwNode<kwPointPairNode> *iter = lineCandidate.headPtr->next;
    while(iter != nullptr){
        eyeNode.nodeContent.ptCenter.x = (iter->nodeContent.p1.x + iter->nodeContent.p2.x) / 2;
        eyeNode.nodeContent.ptCenter.y = (iter->nodeContent.p1.y + iter->nodeContent.p2.y) / 2;
        eyeNode.nodeContent.eyeWidth_H = eyeNode.nodeContent.eyeWidth_V = iter->nodeContent.p2.DistanceTo(iter->nodeContent.p1);
        eyeCandidate.InsertNode(&eyeNode);
        iter = iter->next;
    }
    cout << "eyeCandidate before: " << eyeCandidate.numOfListItems << endl;

    // Create EyeSet， 將 "可能的" eyeSet 放進 eyeSetCandidate
    this->PreparingEyeSets_AllEyes(imImage, eyeCandidate, eyeSetCandidate);
    cout << "eyeCandidate after: " << eyeCandidate.numOfListItems << endl;

    // Check the Versioninformation
    kwNode<kwReaderQR_EyeSet> *tmpSet = eyeSetCandidate.headPtr->next;
    cout << "eyeSetCandidate: " << eyeSetCandidate.numOfListItems << endl;
    int iterTimes = eyeSetCandidate.numOfListItems;

    int found = 0;

    for (int i = 1; i <= iterTimes; ++i)
    {
        if (tmpSet == nullptr) break;
        if ((this->symbolVersion = this->GetVersionInfo(imImage, tmpSet->nodeContent)) != 0) {
            if (this->GetFormatInfo(imImage, tmpSet->nodeContent) == true) {
                if (this->DecodingEyeSets(imImage, tmpSet->nodeContent, DecodedText) == 1)
                {
                    cout << "Version : " << symbolVersion << endl;
                    cout << "Mask : " << symbolMaskPattern << endl;
                    cout << "Ecc : " << symbolECCLevel << endl;
                    tmpSet = tmpSet->next;
                    found = 1;
                    continue;
                }
            }
        }
        // save next pointer BEFORE removing current node
        kwNode<kwReaderQR_EyeSet>* nextSet = tmpSet->next;
        eyeSetCandidate.RemoveNode(tmpSet);
        tmpSet = nextSet;
    }
    cout << "new eyeSetCandidate: " << eyeSetCandidate.numOfListItems << endl;
    return found;
}

//-------------------------------------
void kwReaderQR::LineScanning_FindPatternEyes(kwImageU8 &imImage, kwPoint ptBegin, kwPoint ptEnd,
                                              float bias, kwList<kwPointPairNode> &ptplEyetwobounded)
{
    float* ptBoundDistance = nullptr, EyeWidth;
    pointMsrTool.SetMsrBounds(imImage, ptBegin, ptEnd);
//    cout << pointMsrTool.numOfEdgePts << endl;
    if (pointMsrTool.numOfEdgePts >= 6)
    {
        ptBoundDistance = new float[pointMsrTool.numOfEdgePts - 1];
        for (int i = 0; i < pointMsrTool.numOfEdgePts - 1; ++i)
            ptBoundDistance[i] = pointMsrTool.edgePts[i].DistanceTo(pointMsrTool.edgePts[i + 1]);

        // detect the ratio 1:1:3:1:1
        for (int i = 0; i < pointMsrTool.numOfEdgePts - 5; ++i)
        {
            EyeWidth = pointMsrTool.edgePts[i + 5].x - pointMsrTool.edgePts[i].x;   // 整個 EYE 的大小
            float tolerantRange[2] = { EyeWidth * (1 - bias), EyeWidth * (1 + bias) };

            if (tolerantRange[0] <= 7 * ptBoundDistance[i] && 7 * ptBoundDistance[i] <= tolerantRange[1] &&
                tolerantRange[0] <= 7 * ptBoundDistance[i + 1] && 7 * ptBoundDistance[i + 1] <= tolerantRange[1] &&
                3 * tolerantRange[0] <= 7 * ptBoundDistance[i + 2] && 7 * ptBoundDistance[i + 2] <= 3 * tolerantRange[1] &&
                tolerantRange[0] <= 7 * ptBoundDistance[i + 3] && 7 * ptBoundDistance[i + 3] <= tolerantRange[1] &&
                tolerantRange[0] <= 7 * ptBoundDistance[i + 4] && 7 * ptBoundDistance[i + 4] <= tolerantRange[1])
            {
                // 左右符合的話，從上到下再測一次
                kwMsrPoint measureLine_2;
                kwPoint ptStart2, ptEnd2;
                float* ptBoundDistance2 = nullptr;
                float EyeWidth2 = 0;

                // find center pt, up & down 之所以要多1, 是因為 SetMsrBounds 不一定能包含端點
                float centerx = (pointMsrTool.edgePts[i].x + pointMsrTool.edgePts[i + 5].x) / 2;
                float centeryUp = (pointMsrTool.edgePts[i].y - EyeWidth - 1 > 0) ? (pointMsrTool.edgePts[i].y - EyeWidth - 1) : 0;
                float centeryDown = (pointMsrTool.edgePts[i].y + EyeWidth + 1 < imImage.rows - 1) ? (pointMsrTool.edgePts[i].y + EyeWidth + 1) : imImage.rows-1;
                ptStart2( centerx, centeryUp );
                ptEnd2( centerx, centeryDown );
                measureLine_2.SetMsrBounds( imImage, ptStart2, ptEnd2);

                if (measureLine_2.numOfEdgePts >= 2) {
                    ptBoundDistance2 = new float[measureLine_2.numOfEdgePts - 1];
                    for (int j = 0; j < measureLine_2.numOfEdgePts - 1; ++j)
                      ptBoundDistance2[j] = measureLine_2.edgePts[j].DistanceTo(measureLine_2.edgePts[j + 1]);
                }

                if (ptBoundDistance2 != nullptr) {
                    for (int j = 0; j < measureLine_2.numOfEdgePts - 5; j++)
                    {
                        EyeWidth2 = measureLine_2.edgePts[j + 5].y - measureLine_2.edgePts[j].y;
                        float tolerantRange2[2] = { EyeWidth2 * (1 - bias), EyeWidth2 * (1 + bias) };
                        if (tolerantRange2[0] <= 7 * ptBoundDistance2[j] && 7 * ptBoundDistance2[j] <= tolerantRange2[1] &&
                            tolerantRange2[0] <= 7 * ptBoundDistance2[j + 1] && 7 * ptBoundDistance2[j + 1] <= tolerantRange2[1] &&
                            3 * tolerantRange2[0] <= 7 * ptBoundDistance2[j + 2] && 7 * ptBoundDistance2[j + 2] <= 3 * tolerantRange2[1] &&
                            tolerantRange2[0] <= 7 * ptBoundDistance2[j + 3] && 7 * ptBoundDistance2[j + 3] <= tolerantRange2[1] &&
                            tolerantRange2[0] <= 7 * ptBoundDistance2[j + 4] && 7 * ptBoundDistance2[j + 4] <= tolerantRange2[1])
                        {
                            // Eye's candidate
                            kwNode<kwPointPairNode> Eye;
                            Eye.nodeContent.p1(measureLine_2.edgePts[j].x, measureLine_2.edgePts[j].y);
                            Eye.nodeContent.p2(measureLine_2.edgePts[j].x, measureLine_2.edgePts[j + 5].y);
//                        cout << Eye.nodeContent.p1.x << ", " << Eye.nodeContent.p1.y << endl;
                            ptplEyetwobounded.InsertNode(&Eye);
                        }
                    }
                    delete[] ptBoundDistance2;
                }
            }
        }
        delete[] ptBoundDistance;
    }
}

//-------------------------------------
void kwReaderQR::PreparingEyeSets_AllEyes(kwImageU8 imImage, kwList<kwReaderQR_Eye> &EyeCandidates,
                                             kwList<kwReaderQR_EyeSet> &EyeSetCandidates)
{
    kwPointPair ptpEyeVector;
    float cosTheta;

    // filter the repeated Eyes
    int* filterIndex = new int[EyeCandidates.numOfListItems + 1];
    kwList<kwReaderQR_Eye> temp_realEye;

    for (int i = 0; i <= EyeCandidates.numOfListItems; ++i) // initialize
        filterIndex[i] = -1;

    // 兩點距離太近，歸納成同一個 Eye
    for (int i = 1; i <= EyeCandidates.numOfListItems; ++i)
    {
        if (filterIndex[i] == -1)
        {
            filterIndex[i] = i;
            for (int j = i + 1; j <= EyeCandidates.numOfListItems; ++j)
            {
                if ( EyeCandidates(i)->nodeContent.ptCenter.DistanceTo(EyeCandidates(j)->nodeContent.ptCenter)
                        < EyeCandidates(i)->nodeContent.eyeWidth_H / 2 )
                {
                    filterIndex[j] = i;
                }
            }
        }
    }

    // 只用一個檢測到的 center 當作 center 可能不準，故使用被歸納成同一個 Eye 的做平均
    for (int i = 1; i <= EyeCandidates.numOfListItems; ++i)
    {
        if (filterIndex[i] != -1)
        {
            int Count = 1;
            kwReaderQR_Eye sum;
            sum = EyeCandidates(i)->nodeContent;    // use "=" operator for kwReaderQR_Eye
            for (int j = i + 1; j <= EyeCandidates.numOfListItems; ++j)
            {
                if (filterIndex[j] == i) {
                    sum.ptCenter += EyeCandidates(j)->nodeContent.ptCenter;
                    Count++;
                    filterIndex[j] = -1;
                }
            }
            sum.ptCenter = sum.ptCenter / Count;
            kwNode<kwReaderQR_Eye> sumNode(sum);    // or kwNode<kwReaderQR_Eye> sumNode = kwNode<kwReaderQR_Eye>(sum), where we use copy constructor for kwReaderQR_Eye
            temp_realEye.InsertNode(&sumNode);
        }
    }
    EyeCandidates = temp_realEye;   // 此時已經沒有重覆的點

    // Prepare the (possible)EyeSet
    for (int i = 1; i <= EyeCandidates.numOfListItems; ++i)
    {
        for (int j = 1; j <= EyeCandidates.numOfListItems; ++j)
        {
            for (int k = 1; k <= EyeCandidates.numOfListItems; ++k)
            {
                if (i == j || j == k || i == k)
                    continue;
                else
                {
                    ptpEyeVector.p1(EyeCandidates(j)->nodeContent.ptCenter.x - EyeCandidates(k)->nodeContent.ptCenter.x,
                                        EyeCandidates(j)->nodeContent.ptCenter.y - EyeCandidates(k)->nodeContent.ptCenter.y);
                    ptpEyeVector.p2(EyeCandidates(i)->nodeContent.ptCenter.x - EyeCandidates(k)->nodeContent.ptCenter.x,
                                        EyeCandidates(i)->nodeContent.ptCenter.y - EyeCandidates(k)->nodeContent.ptCenter.y);

                    float cosDenom = ptpEyeVector.p2.Norm2() * ptpEyeVector.p1.Norm2();
                    if (cosDenom == 0)
                        continue;
                    cosTheta = (ptpEyeVector.p2 * ptpEyeVector.p1) / cosDenom;

                    if (cosTheta < cos15 && cosTheta > -cos15 && (ptpEyeVector.p2 || ptpEyeVector.p1) < 0)  // det < 0 => clockwise
                    {
                        kwReaderQR_EyeSet EyeSet;
                        EyeSet.Eye0 = EyeCandidates(k)->nodeContent;
                        EyeSet.Eye1 = EyeCandidates(j)->nodeContent;
                        EyeSet.Eye2 = EyeCandidates(i)->nodeContent;
                        kwNode<kwReaderQR_EyeSet> eyeSetNode(EyeSet);   // we use copy constructor for kwReaderQR_EyeSet
                        EyeSetCandidates.InsertNode(&eyeSetNode);
                    }
                }
            }
        }
    }
    delete[] filterIndex;
}


//-------------------------------------
int kwReaderQR::GetVersionInfo(kwImageU8 &imImage, kwReaderQR_EyeSet &fsEyeSet)
{
    float EyeDistance1 = 0, EyeDistance2 = 0;
    int HorizonVersion, VerticalVersion;
    kwPoint ptStart, ptEnd;
    kwMsrPoint DetectTheModuleSize;

    ptStart = fsEyeSet.Eye0.ptCenter;
    ptEnd = fsEyeSet.Eye1.ptCenter;

    DetectTheModuleSize.SetMsrBounds(imImage, ptStart, ptEnd);
    cout << DetectTheModuleSize.numOfEdgePts << endl;
    if (DetectTheModuleSize.numOfEdgePts >= 6)  // 從一個 eye 到另一個 eye 至少 6 個 edge
    {
        this->unitModuleVectorH =  (ptEnd - ptStart) / ptStart.DistanceTo(ptEnd) *
                             ( (DetectTheModuleSize.edgePts[1].DistanceTo(DetectTheModuleSize.edgePts[0]) + DetectTheModuleSize.edgePts[2].DistanceTo(DetectTheModuleSize.edgePts[1])) / 2);
        ptStart = fsEyeSet.Eye0.ptCenter;
        ptEnd = fsEyeSet.Eye2.ptCenter;
        DetectTheModuleSize.SetMsrBounds(imImage, ptStart, ptEnd);


        if(DetectTheModuleSize.numOfEdgePts >= 6)
        {
            this->unitModuleVectorV =  (ptEnd - ptStart) / ptStart.DistanceTo(ptEnd) *
                                 ( (DetectTheModuleSize.edgePts[1].DistanceTo(DetectTheModuleSize.edgePts[0]) + DetectTheModuleSize.edgePts[2].DistanceTo(DetectTheModuleSize.edgePts[1])) / 2);
            EyeDistance1 = fsEyeSet.Eye0.ptCenter.DistanceTo(fsEyeSet.Eye1.ptCenter) / unitModuleVectorH.Norm2();
            EyeDistance2 = fsEyeSet.Eye0.ptCenter.DistanceTo(fsEyeSet.Eye2.ptCenter) / unitModuleVectorV.Norm2();

            HorizonVersion = int((EyeDistance1 - 7 - 2 - 1) / 4);   // 因為是中點，所以相差 3.5+3.5 = 7 個 module，再扣 2 個空白 module 以及 1 個 format information，剩下的 module數D再除以 4 就是答案
            VerticalVersion = int((EyeDistance2 - 7 - 2 - 1) / 4);
            if ( (HorizonVersion == VerticalVersion) && (HorizonVersion != 0) ) {
                symbolEyes[0] = fsEyeSet.Eye0;
                symbolEyes[1] = fsEyeSet.Eye1;
                symbolEyes[2] = fsEyeSet.Eye2;
                symbolVersion = HorizonVersion;
                return symbolVersion;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

//-------------------------------------
bool kwReaderQR::GetFormatInfo(kwImageU8 &imImage, kwReaderQR_EyeSet &thisEyeSet)
{
    kwPoint ptPosition;
    kwPoint FormatPosition[15];
    float FormatGraylevel[15];
    unsigned short int iFormat = 0, XOR = 0, Count_1, Ans_Count_1 = 16, AnsIndex;   // 16 bits(0~65535)
    kwPixel OtsuValue;

    ptPosition = ((thisEyeSet.Eye0.ptCenter) + (unitModuleVectorH * 5)) - (unitModuleVectorV * 3);  // if Eye0 is 左上
    for (int i = 0; i <= 5; ++i) {
        FormatPosition[i] = ptPosition;
        ptPosition = ptPosition + unitModuleVectorV;
    }
    ptPosition = ptPosition + unitModuleVectorV;
    FormatPosition[6] = ptPosition;
    ptPosition = ptPosition + unitModuleVectorV;
    FormatPosition[7] = ptPosition;
    ptPosition = ptPosition - unitModuleVectorH;
    FormatPosition[8] = ptPosition;
    ptPosition = ptPosition - (unitModuleVectorH * 2);
    for (int i = 9; i <= 14; ++i) {
        FormatPosition[i] = ptPosition;
        ptPosition = ptPosition - unitModuleVectorH;
    }
    kwImageTools::ResampleSubpixel(imImage, FormatPosition, FormatGraylevel, 15, kwImageTools::SubPixel_Method_InverseDistanceWeighting);
    OtsuValue = kwImageTools::Threshold2Otsu(FormatGraylevel, 15);

    // 取出黑色的部分
    for (int i = 14; 0 <= i; --i) {
        iFormat <<= 1;  // set iFormat to itself shifted by one bit to the right
        if (FormatGraylevel[i] <= OtsuValue)
            iFormat++;
    }

    for (int i = 0; i < 32; ++i) {
        Count_1 = 0;
        XOR = iFormat ^ FormatTable[i]; // ^ is XOR operator
        for (int j = 0; j < 16; j++) {
            if ((XOR & 0x0001) == 1)
                Count_1++;
            XOR >>= 1;
        }
        if (Count_1 <= 3 && Count_1 < Ans_Count_1) {    // 錯最少的
            Ans_Count_1 = Count_1;
            AnsIndex = i;   // data bits are also its index bits
        }
    }

    if (Ans_Count_1 > 3)    // 錯超過 3 個
        return false;
    else {
        symbolECCLevel = (AnsIndex >> 3);
        symbolMaskPattern = (AnsIndex & 0x0007);
        return true;
    }
}

//-------------------------------------
int kwReaderQR::DecodingEyeSets(kwImageU8 &imImage, kwReaderQR_EyeSet &EyeSet,
                                   unsigned char DecodedText[])
{
//     Get the fourth Eye

//     get the QRcode image graylevel(in pixel)

//     Get the Affine Matrix
    int QR_size = symbolVersion * 4 + 7*2 + 2 + 1;
    kwPoint camera_eye0[5], camera_eye1[5], camera_eye2[5], camera[15];
    kwPoint augmented_world_eye0[5], augmented_world_eye1[5], augmented_world_eye2[5], augmented_world[15];

    augmented_world[0] = augmented_world_eye0[0](4.5, 4.5);
    augmented_world[1] = augmented_world_eye0[1](1.5, 1.5);
    augmented_world[2] = augmented_world_eye0[2](7.5, 1.5);
    augmented_world[3] = augmented_world_eye0[3](7.5, 7.5);
    augmented_world[4] = augmented_world_eye0[4](1.5, 7.5);
    augmented_world[5] = augmented_world_eye1[0](float(QR_size+2) - 4.5, 4.5);
    augmented_world[6] = augmented_world_eye1[1](float(QR_size+2) - 7.5, 1.5);
    augmented_world[7] = augmented_world_eye1[2](float(QR_size+2) - 1.5, 1.5);
    augmented_world[8] = augmented_world_eye1[3](float(QR_size+2) - 1.5, 7.5);
    augmented_world[9] = augmented_world_eye1[4](float(QR_size+2) - 7.5, 7.5);
    augmented_world[10] = augmented_world_eye2[0](4.5, float(QR_size+2) - 4.5);
    augmented_world[11] = augmented_world_eye2[1](1.5, float(QR_size+2) - 7.5);
    augmented_world[12] = augmented_world_eye2[2](7.5, float(QR_size+2) - 7.5);
    augmented_world[13] = augmented_world_eye2[3](7.5, float(QR_size+2) - 1.5);
    augmented_world[14] = augmented_world_eye2[4](1.5, float(QR_size+2) - 1.5);

    camera[0] = camera_eye0[0] = EyeSet.Eye0.ptCenter;
    camera[1] = camera_eye0[1] = EyeSet.Eye0.ptCenter - unitModuleVectorH*3 - unitModuleVectorV*3;
    camera[2] = camera_eye0[2] = EyeSet.Eye0.ptCenter + unitModuleVectorH*3 - unitModuleVectorV*3;
    camera[3] = camera_eye0[3] = EyeSet.Eye0.ptCenter + unitModuleVectorH*3 + unitModuleVectorV*3;
    camera[4] = camera_eye0[4] = EyeSet.Eye0.ptCenter - unitModuleVectorH*3 + unitModuleVectorV*3;
    camera[5] = camera_eye1[0] = EyeSet.Eye1.ptCenter;
    camera[6] = camera_eye1[1] = EyeSet.Eye1.ptCenter - unitModuleVectorH*3 - unitModuleVectorV*3;
    camera[7] = camera_eye1[2] = EyeSet.Eye1.ptCenter + unitModuleVectorH*3 - unitModuleVectorV*3;
    camera[8] = camera_eye1[3] = EyeSet.Eye1.ptCenter + unitModuleVectorH*3 + unitModuleVectorV*3;
    camera[9] = camera_eye1[4] = EyeSet.Eye1.ptCenter - unitModuleVectorH*3 + unitModuleVectorV*3;
    camera[10] = camera_eye2[0] = EyeSet.Eye2.ptCenter;
    camera[11] = camera_eye2[1] = EyeSet.Eye2.ptCenter - unitModuleVectorH*3 - unitModuleVectorV*3;
    camera[12] = camera_eye2[2] = EyeSet.Eye2.ptCenter + unitModuleVectorH*3 - unitModuleVectorV*3;
    camera[13] = camera_eye2[3] = EyeSet.Eye2.ptCenter + unitModuleVectorH*3 + unitModuleVectorV*3;
    camera[14] = camera_eye2[4] = EyeSet.Eye2.ptCenter - unitModuleVectorH*3 + unitModuleVectorV*3;

// ****************************** method 2, use the 4th eye to locate
//    Camara[3] = Get4thCalibrateEye(imImage, EyeSet);
//    if (Camara[3].IsZero() || Camara[3].IsNULL()) {
//        return 0;
//    }
// ******************************

    this->transPerspective.Calibrate(augmented_world, camera, 15);

// ****************************** check the Eye in the is correct
//    cv::Mat eye = cv::Mat(imImage.rows, imImage.cols, CV_8UC1);
//    memcpy(eye.data, imImage.membase, imImage.numOfPixels * sizeof(uchar));
//    for(int i = 0; i < 15; ++i){
//        cv::Point ptThis;
//        ptThis.x = (int)(camera[i].x+0.5);
//        ptThis.y = (int)(camera[i].y+0.5);
//        cv::line( eye, ptThis, ptThis, cv::Scalar(255,255,255), 1, 8 );
//    }
////    cv::imwrite("D:/summer_project/QR/result_image/eye.png", eye);
//////    cout << EyeSet.Eye0.eyeWidth_H << endl;
//////    cout << EyeSet.Eye0.ptCenter.x <<", "<< EyeSet.Eye1.ptCenter.x<<", " << EyeSet.Eye0.ptCenter.DistanceTo(EyeSet.Eye1.ptCenter)<< endl;
//    cv::imshow("eye", eye);
//    cv::waitKey(0);
//    cv::destroyWindow("eye");
// ******************************

    kwPoint* coord_augmented_world = new kwPoint[(QR_size+2) * (QR_size+2)];
    kwPoint* coord_camera = new kwPoint[(QR_size+2) * (QR_size+2)];
    for (int i = 0; i < (QR_size+2); ++i) {
        for (int j = 0; j < (QR_size+2); ++j) {
            coord_augmented_world[i * (QR_size+2) + j](j + 0.5, i + 0.5);
        }
    }
    this->transPerspective.MappingForward( coord_augmented_world, coord_camera, (QR_size+2) * (QR_size+2));

// ****************************** 用來看我們會從那些地方取點
////    cv::Mat test = imread("D:/summer_project/QR/TestImage/QR/V30.png", cv::IMREAD_GRAYSCALE);
//    cv::Mat test = cv::Mat::zeros(imImage.rows, imImage.cols, CV_8UC1);
//    cv::Point pt;
//    for (int i = 0; i < (QR_size+2) * (QR_size+2); ++i) {
//        pt.x = (int)(coord_camera[i].x+0.5);
//        pt.y = (int)(coord_camera[i].y+0.5);
////        line(test, pt, pt, cv::Scalar(0,0,0));    //draw black pt
//        line(test, pt, pt, cv::Scalar(255,255,255));  //draw white pt
//    }
//    imshow("debugger", test);
//    cv::waitKey(0);
//    cv::destroyWindow("debugger");
// ******************************

    float* graylevel_augmented = new float[(QR_size+2) * (QR_size+2)];

    // put the calculated pixel value into graylevel_augment, since the value of coord_camera might not be the integer
    kwImageTools::ResampleSubpixel(imImage, coord_camera, graylevel_augmented, (QR_size+2) * (QR_size+2), kwImageTools::SubPixel_Method_InverseDistanceWeighting);

    // initial the QR Data
    kwImageU8 subtract_QR_tmp((QR_size+2), (QR_size+2)), subtract_QR(QR_size, QR_size), unmasked_QR(QR_size, QR_size);
    for (int i = 0; i < (QR_size+2) * (QR_size+2); ++i)
        subtract_QR_tmp.membase[i] = ((int)(graylevel_augmented[i] + 0.5) > 255) ? (kwPixel)255 : (kwPixel)(int)(graylevel_augmented[i] + 0.5);
    kwPixel OtsuValue;
    kwImageU8 roi(3,3);
    // local Otsu method
    for (int i = 0; i < QR_size; ++i) {
        for (int j = 0; j < QR_size; ++j) {
            for(int xx = 0; xx < 3; ++xx){
                for(int yy = 0; yy < 3; ++yy)
                    roi.imgElement[xx][yy] = subtract_QR_tmp.imgElement[i+xx][j+yy];
            }
            OtsuValue = kwImageTools::Threshold2Otsu( roi.membase, 3 * 3);
            // for calculate purpose, white is 1, black is 0
            subtract_QR.imgElement[i][j] = (subtract_QR_tmp.imgElement[i+1][j+1] <= OtsuValue) ? 0 : 1;
        }
    }
//  ****************************** output the QRcode
//    for display purpose, should turn white from 1 to 255
//    for (int i = 0; i < subtract_QR.numOfPixels; ++i) {
//        (subtract_QR.membase[i] == 1) ? subtract_QR.membase[i] = 255 : subtract_QR.membase[i] = 0;
//    }

//    cv::Mat matGray_tmp = cv::Mat(subtract_QR_tmp.rows, subtract_QR_tmp.cols, CV_8UC1);
//    memcpy( matGray_tmp.data, subtract_QR_tmp.membase, subtract_QR_tmp.numOfPixels * sizeof(kwPixel) );
//    cv::imwrite("D:/summer_project/QR/result_image/subtracted_QRImage_tmp.png", matGray_tmp);
//    imshow("subtracted_QRImage_tmp", matGray_tmp);
//    cv::waitKey(0);
//    cv::destroyWindow("subtracted_QRImage_tmp");

//    cv::Mat matGray = cv::Mat(subtract_QR.rows, subtract_QR.cols, CV_8UC1);
//    memcpy( matGray.data, subtract_QR.membase, subtract_QR.numOfPixels * sizeof(kwPixel) );
//    cv::imwrite("D:/summer_project/QR/result_image/subtracted_QRImage.png", matGray);
//    imshow("subtracted_QRImage", matGray);
//    cout << subtract_QR.cols << endl;   // 顯示的時候圖片的 size 不一定是正確的
//    cv::waitKey(0);
//    cv::destroyWindow("subtracted_QRImage");
// ******************************
    int  maxNumofCodeword = (int)(subtract_QR.numOfPixels/8), numofTotalBlocks = 0, index = 0, flag = 0;
    kwPixel* DecodeDataBefore = new kwPixel[maxNumofCodeword];
    kwPixel* DecodeDataAfter = new kwPixel[maxNumofCodeword];
    kwPixel **separatedData;

    // unmask
    this->decoderQR.UnMask(subtract_QR, this->symbolMaskPattern, this->symbolVersion, unmasked_QR);

    // retrieval, i.e. Getcodeword
    this->decoderQR.GetCodeword(unmasked_QR, this->symbolVersion, this->symbolECCLevel, DecodeDataBefore);


    // unInterleaving
    // ISO 18004 Table 9: S = shorter block group, L = longer group (0 if single);
    // the module stream is the DATA codewords interleaved row-major, followed
    // by the ECC codewords interleaved row-major (reference-encoder layout).
    index = decoderQR.GetIndex( this->symbolVersion, this->symbolECCLevel );
    int nS = Ecc_S_numofBlocks[index], cS = Ecc_S_c[index], kS = Ecc_S_k[index];
    int nL = Ecc_L_numofBlocks[index], cL = Ecc_L_c[index], kL = Ecc_L_k[index];
    int numBlocks = nS + nL;
    numofTotalBlocks = numBlocks;
    separatedData = new kwPixel*[numBlocks];
    for (int i = 0; i < numBlocks; ++i)
        separatedData[i] = new kwPixel[(cL > cS) ? cL : cS];

    {
        int k = 0, maxK = (kL > kS) ? kL : kS;
        int eS = cS - kS, eL = cL - kL, maxE = (eL > eS) ? eL : eS;
        for (int p = 0; p < maxK; ++p)
            for (int i = 0; i < numBlocks; ++i) {
                int kBlock = (i < nS) ? kS : kL;
                if (p < kBlock)
                    separatedData[i][p] = DecodeDataBefore[k++];
            }
        for (int p = 0; p < maxE; ++p)
            for (int i = 0; i < numBlocks; ++i) {
                int kBlock = (i < nS) ? kS : kL;
                int eBlock = (i < nS) ? eS : eL;
                if (p < eBlock)
                    separatedData[i][kBlock + p] = DecodeDataBefore[k++];
            }
    }

    // Reed-Solomon Error Correcting, block by block; data codewords concatenated in order
    int totalDataWords = 0;
    flag = 1;
    for (int i = 0; i < numBlocks && flag; ++i) {
        int kBlock = (i < nS) ? kS : kL;
        if (kBlock <= 0) continue;
        flag = this->decoderQR.GetDecodeData( (i < nS) ? cS : cL, kBlock, separatedData[i], DecodeDataAfter + totalDataWords );
        totalDataWords += kBlock;
    }

    // Get Decoded text, only need to send data
    if ( flag )
    {
        this->decoderQR.UncompressText(DecodeDataAfter, totalDataWords, this->symbolVersion, DecodedText);
//        this->decoderQR.DecodeText(DecodeDataAfter, DecodedText, this->symbolVersion, this->symbolECCLevel);
        delete[] graylevel_augmented;
        delete[] DecodeDataBefore;
        delete[] DecodeDataAfter;
        delete[] coord_augmented_world;
        delete[] coord_camera;
        for (int i = 0; i < numofTotalBlocks; ++i)
            delete [] separatedData[i];
        delete [] separatedData;
        return 1;
    }
    else {
        delete[] graylevel_augmented;
        delete[] DecodeDataBefore;
        delete[] DecodeDataAfter;
        delete[] coord_augmented_world;
        delete[] coord_camera;
        for (int i = 0; i < numofTotalBlocks; ++i)
            delete [] separatedData[i];
        delete [] separatedData;
        cout << "Decode Text Fail" << endl;
        return 0;
    }
}

kwPoint kwReaderQR::Get4thCalibrateEye(kwImageU8& imImage, kwReaderQR_EyeSet& fsEyeSet) {
    kwPoint Eye4(0, 0), ptCalibrate1, ptCalibrate2, Eye1_Calibrate_Vector, Eye2_Calibrate_Vector;
    kwPoint Eye1_Calibrated_sample[1000], Eye1_lineSample[1000];
    kwPoint Eye2_Calibrated_sample[1000], Eye2_lineSample[1000];
    int Eye1_SampleNum = 0, Eye2_SampleNum = 0;
    kwLine Eye1_Calibrate_Line, Eye2_Calibrate_Line;
    kwMsrPoint QRMsr;
    // Use Eye2 to find the lower bound of QR
    ptCalibrate1 = ptCalibrate2 = fsEyeSet.Eye2.ptCenter - this->unitModuleVectorH * 2;
    ptCalibrate2 += this->unitModuleVectorV * 4;

    for (int i = 0; i < 4; ++i) {
      ptCalibrate1 += this->unitModuleVectorH;
      ptCalibrate2 += this->unitModuleVectorH;
      QRMsr.SetMsrBounds(imImage, ptCalibrate1, ptCalibrate2);
      if (QRMsr.numOfEdgePts < 2) {
        return Eye4;
      }
      Eye2_Calibrated_sample[Eye2_SampleNum] =
          (QRMsr.edgePts[QRMsr.numOfEdgePts - 1] + QRMsr.edgePts[QRMsr.numOfEdgePts - 2]) / 2;
      Eye2_lineSample[Eye2_SampleNum] = QRMsr.edgePts[QRMsr.numOfEdgePts - 1];
      Eye2_SampleNum++;
    }

    // Repeat Calibrate

    for (int times = 0; times < (this->symbolVersion * 4 + 17 - 5 - 5); times++) {
      Eye2_Calibrate_Vector =
          Eye2_lineSample[Eye2_SampleNum - 1] - Eye2_lineSample[Eye2_SampleNum - 2];
      ptCalibrate1 = ptCalibrate2 = Eye2_Calibrated_sample[Eye2_SampleNum - 1];
      ptCalibrate2 += this->unitModuleVectorH.Project(Eye2_Calibrate_Vector) * 10;

      QRMsr.SetMsrBounds(imImage, ptCalibrate1, ptCalibrate2);
      if (QRMsr.numOfEdgePts < 2) {
        return Eye4;
      }
      for (int i = 0; i < QRMsr.numOfEdgePts - 1; ++i) {
        if (QRMsr.edgeType[i] == false && QRMsr.edgeType[i + 1] == true) {
          Eye2_Calibrated_sample[Eye2_SampleNum] =
              (QRMsr.edgePts[i] + QRMsr.edgePts[i + 1]) / 2;
          ptCalibrate1 = ptCalibrate2 = Eye2_Calibrated_sample[Eye2_SampleNum];
          ptCalibrate2 += this->unitModuleVectorV;
          QRMsr.SetMsrBounds(imImage, ptCalibrate1, ptCalibrate2);
          if (QRMsr.numOfEdgePts == 0) {
            return Eye4;
          }
          Eye2_lineSample[Eye2_SampleNum] = QRMsr.edgePts[0];
          Eye2_SampleNum++;
          break;
        }
      }
    }

    Eye2_Calibrate_Line.Set(Eye2_lineSample[Eye2_SampleNum - 1],
                               Eye2_lineSample[Eye2_SampleNum - 2]);

    // Use Eye1 to find the right bound of QR
    ptCalibrate1 = ptCalibrate2 = fsEyeSet.Eye1.ptCenter - this->unitModuleVectorV * 2;
    ptCalibrate2 += this->unitModuleVectorH * 4;
    for (int i = 0; i < 4; ++i) {
      ptCalibrate1 += this->unitModuleVectorV;
      ptCalibrate2 += this->unitModuleVectorV;
      QRMsr.SetMsrBounds(imImage, ptCalibrate1, ptCalibrate2);
      if (QRMsr.numOfEdgePts < 2) {
        return Eye4;
      }
      Eye1_Calibrated_sample[Eye1_SampleNum] =
          (QRMsr.edgePts[QRMsr.numOfEdgePts - 1] + QRMsr.edgePts[QRMsr.numOfEdgePts - 2]) / 2;
      Eye1_lineSample[Eye1_SampleNum] = QRMsr.edgePts[QRMsr.numOfEdgePts - 1];
      Eye1_SampleNum++;
    }
    // Repeat Calibrate
    for (int times = 0; times < (this->symbolVersion * 4 + 17 - 5 - 5); times++) {
      Eye1_Calibrate_Vector =
          Eye1_lineSample[Eye1_SampleNum - 1] - Eye1_lineSample[Eye1_SampleNum - 2];
      ptCalibrate1 = ptCalibrate2 = Eye1_Calibrated_sample[Eye1_SampleNum - 1];
      ptCalibrate2 += this->unitModuleVectorV.Project(Eye1_Calibrate_Vector) * 10;

      QRMsr.SetMsrBounds(imImage, ptCalibrate1, ptCalibrate2);
      if (QRMsr.numOfEdgePts < 2) {
        return Eye4;
      }
      for (int i = 0; i < QRMsr.numOfEdgePts - 1; ++i) {
        if (QRMsr.edgeType[i] == false && QRMsr.edgeType[i + 1] == true) {
          Eye1_Calibrated_sample[Eye1_SampleNum] =
              (QRMsr.edgePts[i] + QRMsr.edgePts[i + 1]) / 2;
          ptCalibrate1 = ptCalibrate2 = Eye1_Calibrated_sample[Eye1_SampleNum];
          ptCalibrate2 += this->unitModuleVectorH;
          QRMsr.SetMsrBounds(imImage, ptCalibrate1, ptCalibrate2);
          if (QRMsr.numOfEdgePts == 0) {
            return Eye4;
          }
          Eye1_lineSample[Eye1_SampleNum] = QRMsr.edgePts[0];
          Eye1_SampleNum++;
          break;
        }
      }
    }

    Eye1_Calibrate_Line.Set(Eye1_lineSample[Eye1_SampleNum - 1],
                               Eye1_lineSample[Eye1_SampleNum - 2]);

    // Interset 2 line to get the corner of QR
    Eye4 = Eye1_Calibrate_Line.LineIntersection(Eye2_Calibrate_Line);

    //====================Debugger=====================//
    //  Mat sample = imread("C:/QR/pic/9457.jpg", IMREAD_GRAYSCALE);
    //  Point pt;
    //  cvtColor(sample, sample, CV_GRAY2RGB);
    //  for (int i = 0; i < Eye2_SampleNum; ++i) {
    //    pt.x = Eye2_Calibrated_sample[i].x;
    //    pt.y = Eye2_Calibrated_sample[i].y;
    //    line(sample, pt, pt, Scalar(0, 0, 255));
    //    pt.x = Eye2_lineSample[i].x;
    //    pt.y = Eye2_lineSample[i].y;
    //    line(sample, pt, pt, Scalar(0, 0, 255));
    //  }
    //  for (int i = 0; i < Eye1_SampleNum; ++i) {
    //    pt.x = Eye1_Calibrated_sample[i].x;
    //    pt.y = Eye1_Calibrated_sample[i].y;
    //    line(sample, pt, pt, Scalar(0, 0, 255));
    //    pt.x = Eye1_lineSample[i].x;
    //    pt.y = Eye1_lineSample[i].y;
    //    line(sample, pt, pt, Scalar(0, 0, 255));
    //  }
    //  pt.x = Eye4.x;
    //  pt.y = Eye4.y;
    //  line(sample, pt, pt, Scalar(0, 0, 255));
    //  imshow("test", sample);
    //  cvWaitKey(0);
    //====================Debugger=====================//
    return Eye4;
}





//int QR_size = symbolVersion * 4 + 17;
//kwPoint camera_eye1[5], camera_eye2[5], camera_eye3[5];
//kwPoint augmented_world_eye1[5], augmented_world_eye2[5], augmented_world_eye3[5];
//kwPoint Augmented_World[4], real_World[4], Camara[4];
//Augmented_World[0](4.5, 4.5);
//Augmented_World[1](float(QR_size) - 4.5, 4.5);
//Augmented_World[2](4.5, float(QR_size) - 4.5);
//Augmented_World[3](float(QR_size) - 7.5, float(QR_size) - 7.5);
//Camara[0] = EyeSet.Eye0.ptCenter;
//Camara[1] = EyeSet.Eye1.ptCenter;
//Camara[2] = EyeSet.Eye2.ptCenter;
//Camara[3] = Get4thCalibrateEye(imImage, EyeSet);
//if (Camara[3].IsZero() || Camara[3].IsNULL()) {
//    return 0;
//}

//this->transPerspective.Calibrate(World, Camara, 4);
//// Set the Trans point

//kwPoint* ptData = new kwPoint[QR_size * QR_size];
//kwPoint* ptSample = new kwPoint[QR_size * QR_size];
//for (int i = 0; i < QR_size; ++i) {
//    for (int j = 0; j < QR_size; ++j) {
//        ptData[i * QR_size + j](j + 0.5, i + 0.5);
//    }
//}
//this->transPerspective.MappingForward( ptData, ptSample, QR_size * QR_size);

//// subpixel position debugger
////-------------------------------------------------------------------------------------------------
////  Mat test = imread("D:/summer_project/QR/TestImage/QR/small.jpg", IMREAD_GRAYSCALE);
////  Point pt;
////  for (int i = 0; i < QR_size * QR_size; ++i) {
////    pt.x = ptSample[i].x;
////    pt.y = ptSample[i].y;

////    line(test, pt, pt, Scalar(0, 0, 255));
////  }
////  imshow("debugger", test);
////-------------------------------------------------------------------------------------------------

//float Graylevel[QR_size * QR_size];

//kwImageTools::ResampleSubpixel(imImage, ptSample, Graylevel, QR_size * QR_size, kwImageTools::SubPixel_Method_InverseDistanceWeighting);
//// initial the QR Data
//kwImageU8 QR_DATA(QR_size, QR_size);
//for (int i = 0; i < QR_size * QR_size; ++i)
//    QR_DATA.membase[i] = ((int)(Graylevel[i] + 0.5) > 255) ? (kwPixel)255 : (kwPixel)(int)(Graylevel[i] + 0.5);

//kwPixel OtsuValue;
//// global Otsu
//OtsuValue = kwImageTools::Threshold2Otsu( QR_DATA.membase, QR_DATA.cols * QR_DATA.rows);
//cout << "Otsu Value" << OtsuValue << endl;
//for (int i = 0; i < QR_DATA.rows; ++i) {
//    for (int j = 0; j < QR_DATA.cols; ++j) {
//        QR_DATA.imgElement[i][j] = (QR_DATA.imgElement[i][j] <= OtsuValue) ? 1 : 0;
//    }
//}
////++++++++++++++++++++++ output the QRcode
//cv::Mat matGray = cv::Mat::zeros(QR_DATA.rows, QR_DATA.cols, CV_8U);
//memcpy( matGray.data, QR_DATA.membase, QR_DATA.numOfPixels * sizeof(uchar) );
//imshow("Corrected Image", matGray);
//cv::waitKey(0);
////+++++++++++++++++++++++//
//return 1;
////      // Decoder unmask
////      this->m_Decoder.DecoderQR_UnMask(QR_DATA, this->m_nSymbolMaskPattern, this->symbolVersion);
////      // Decoder Getcodeword
////      unsigned char DecodeDataBefore[QR_DATA.m_nColNum * QR_DATA.m_nRowNum];
////      unsigned char DecodeDataAfter[QR_DATA.m_nColNum * QR_DATA.m_nRowNum];

////      this->m_Decoder.GetCodeword(QR_DATA, this->symbolVersion, this->symbolECCLevel,
////                                  DecodeDataBefore);
////      // Decoder Get Decode Data

////      if (this->m_Decoder.GetDecodeData(this->symbolVersion, this->symbolECCLevel,
////                                        DecodeDataBefore, DecodeDataAfter) == 1) {
////        this->m_Decoder.Decoding(DecodeDataAfter, cDecodedData, this->symbolVersion,
////                                 this->symbolECCLevel);
////        delete[] ptData;
////        delete[] ptSample;
////        return 1;
////      } else {
////        delete[] ptData;
////        delete[] ptSample;

////        return 0;

// 在每個 eye 之中，應該要用每個 eye 的 module size 去爬，這裡是用整張圖的 module 去爬
//camera[0] = camera_eye0[0] = EyeSet.Eye0.ptCenter;
//camera[1] = camera_eye0[1] = EyeSet.Eye0.ptCenter + kwPoint(-3*EyeSet.Eye0.unitModuleWidth_H, -3*EyeSet.Eye0.unitModuleWidth_V);
//camera[2] = camera_eye0[2] = EyeSet.Eye0.ptCenter + kwPoint(3*EyeSet.Eye0.unitModuleWidth_H, -3*EyeSet.Eye0.unitModuleWidth_V);
//camera[3] = camera_eye0[3] = EyeSet.Eye0.ptCenter + kwPoint(3*EyeSet.Eye0.unitModuleWidth_H, 3*EyeSet.Eye0.unitModuleWidth_V);
//camera[4] = camera_eye0[4] = EyeSet.Eye0.ptCenter + kwPoint(-3*EyeSet.Eye0.unitModuleWidth_H, 3*EyeSet.Eye0.unitModuleWidth_V);
//camera[5] = camera_eye1[0] = EyeSet.Eye1.ptCenter;
//camera[6] = camera_eye1[1] = EyeSet.Eye1.ptCenter + kwPoint(-3*EyeSet.Eye1.unitModuleWidth_H, -3*EyeSet.Eye1.unitModuleWidth_V);
//camera[7] = camera_eye1[2] = EyeSet.Eye1.ptCenter + kwPoint(3*EyeSet.Eye1.unitModuleWidth_H, -3*EyeSet.Eye1.unitModuleWidth_V);
//camera[8] = camera_eye1[3] = EyeSet.Eye1.ptCenter + kwPoint(3*EyeSet.Eye1.unitModuleWidth_H, 3*EyeSet.Eye1.unitModuleWidth_V);
//camera[9] = camera_eye1[4] = EyeSet.Eye1.ptCenter + kwPoint(-3*EyeSet.Eye1.unitModuleWidth_H, 3*EyeSet.Eye1.unitModuleWidth_V);
//camera[10] = camera_eye2[0] = EyeSet.Eye2.ptCenter;
//camera[11] = camera_eye2[1] = EyeSet.Eye2.ptCenter + kwPoint(-3*EyeSet.Eye2.unitModuleWidth_H, -3*EyeSet.Eye2.unitModuleWidth_V);
//camera[12] = camera_eye2[2] = EyeSet.Eye2.ptCenter + kwPoint(3*EyeSet.Eye2.unitModuleWidth_H, -3*EyeSet.Eye2.unitModuleWidth_V);
//camera[13] = camera_eye2[3] = EyeSet.Eye2.ptCenter + kwPoint(3*EyeSet.Eye2.unitModuleWidth_H, 3*EyeSet.Eye2.unitModuleWidth_V);
//camera[14] = camera_eye2[4] = EyeSet.Eye2.ptCenter + kwPoint(-3*EyeSet.Eye2.unitModuleWidth_H, 3*EyeSet.Eye2.unitModuleWidth_V);
