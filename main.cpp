#include "main.h"

#include "kwConst.h"
#include "kwImage_object.h"
#include "kwMatrix.h"
#include "kwPoint.h"
#include "kwTransPerspective.h"
#include "kwImageTool.h"
#include "kwMsrPoint.h"
#include "kwFrame.h"
#include "kwReaderQR.h"


int gNumPointFrame = 2;
kwFrame *gpPointFrame;

int main(int argc, char* argv[])
{
    cout << "Hello CV!" << endl;

    // create PointFrame
    gpPointFrame = new kwFrame[gNumPointFrame];

    cv::Mat matColor, matGray;
    // image path: CLI arg > default test image
    const char* imgPath = (argc > 1) ? argv[1] : "test_images/QR/V5_H.png";
    matColor = cv::imread(imgPath);
    if(matColor.empty()){
        // fallback: try webcam
        cv::VideoCapture vcCam(0);
        if(vcCam.isOpened()){
            vcCam >> matColor;
        }
    }
    if(matColor.empty()){
        cout << "ERROR: no image loaded from '" << imgPath << "' and webcam failed." << endl;
        delete[] gpPointFrame;
        return 1;
    }
//    cv::imshow("Original Image", matColor);
//    cv::imwrite("D:/summer_project/QR/result_image/color.png", matColor);
    cv::cvtColor(matColor, matGray, cv::COLOR_BGR2GRAY);   //另一種轉灰階的方法
//    cv::imwrite("D:/summer_project/QR/result_image/gray.png", matGray);
    kwImageU8 imCorrected(matGray.rows, matGray.cols);
//    cout << matGray.rows << "," << matGray.cols << endl;
    //注意是 matGray.data
    memcpy( imCorrected.membase, matGray.data, imCorrected.numOfPixels * sizeof(uchar) );
//    kwMsrPoint P;
//    kwPoint start(gpPointFrame[0].getx(), gpPointFrame[0].gety()), end(gpPointFrame[1].getx(), gpPointFrame[1].gety());
//          kwPoint start(10 , 30), end(198, 30);
//    P.SetMsrBounds(imCorrected, start, end);
//    cout << P.numOfEdgePts<< endl;
    kwReaderQR QR;
    unsigned char text[1000];
    QR.DoReading(imCorrected, text);
    cout << text << endl;

    return 0;
}



//#include <iostream>
//#include "opencv2/opencv.hpp"

//#include "main.h"
//#include "kwConst.h"
//#include "kwImage_object.h"
//#include "kwMatrix.h"
//#include "kwPoint.h"
//#include "kwTransPerspective.h"
//#include "kwImageTool.h"
//#include "kwMsrPoint.h"
//#include "kwFrame.h"
//#include "kwReaderQR.h"


//using namespace std;
//using namespace cv;

//int OpenDevice(VideoCapture& cap, int& cam_index, int& res_w, int& res_h);

//// mouse event dunction
//void MyMouseEvent(int event, int x, int y, int flags, void* userdata);
//int HitFrame(int HitX, int HitY);
//int DragFrame(int HitX, int HitY);
//int ReleaseFrame();
//int DrawFrame(Mat& DstMat);


//// MyPointFrame setting
//int gNumPointFrame = 2;
//kwFrame* gpPointFrame;

//kwImageU8 imCorrected;
//int res_w=204, res_h=192;
//kwMsrPoint P;
//Mat grayimg, img;
//int main()
//{

//    imCorrected.SetSize(res_h, res_w);
//    // create window and set window momuse event
//    string window_name = "webcam";
//    namedWindow(window_name);
//    setMouseCallback(window_name, MyMouseEvent);

//    // create PointFrame
//    gpPointFrame = new kwFrame[gNumPointFrame];

//    while(1)
//    {
//        img = imread("D:/summer_project/QR/TestImage/QR/small.jpg" );

//        cvtColor(img, grayimg, CV_BGR2GRAY);
//        memcpy( imCorrected.membase, grayimg.data, imCorrected.numOfPixels * sizeof(uchar) );
////        cout << imCorrected.rows << imCorrected.cols << endl;
////        cout << grayimg.cols << grayimg.rows << endl;
////        for (int i = 0; i < imCorrected.rows; ++i)
////            for (int j = 0; j < imCorrected.cols; ++j){
////                cout << int(imCorrected.imgElement[i][j])<< endl;
////            }

////        kwPoint start(8 , 30), end(60, 30);
//        kwPoint start(gpPointFrame[0].getx(), gpPointFrame[0].gety()), end(gpPointFrame[1].getx(), gpPointFrame[1].gety());

//        cout << gpPointFrame[0].getx() <<","<< gpPointFrame[0].gety() <<","<< gpPointFrame[1].getx() <<","<< gpPointFrame[1].gety() << endl;
//        P.SetMsrBounds(imCorrected, start, end);
//        cout << "edge pt:" << P.numOfEdgePts << endl;

//        if(P.numOfEdgePts > 0){
//            for(int i = 0; i < P.numOfEdgePts; ++i){
//                cv::Point ptThis;
//                ptThis.x = P.edgePts[i].x;
//                ptThis.y = P.edgePts[i].y;
//                // draw edge point
//                cv::line( img, ptThis, ptThis, cv::Scalar(0,0,255), 1, 8 );
//            }
//        }
//        DrawFrame(img); // draw MyPointFrame，要放在 msr point 後面
//        cv::imshow(window_name, img);
//        // escape
//        if( cv::waitKey(30) == 27 ){ //'escape key'
//            kwReaderQR QR;
//            unsigned char text[1000];
//            cout << QR.DoReading(imCorrected, text) << endl;
//            break;
//        }
//    }

//    delete [] gpPointFrame;

//    return 0;
//}

//int OpenDevice(VideoCapture& cap, int& cam_index, int& res_w, int& res_h)
//{
//    int cam_idx_str = 0;
//    int cam_idx_end = 10;
//    if(cam_index != -1) cam_idx_str = cam_idx_end = cam_index;

//    // try open device
//    int deviceID = 0;
//    int apiID    = CAP_ANY;
//    for(int i=cam_idx_str; i<cam_idx_end; ++i)
//    {
//        cap.open(deviceID, apiID);
//        if(cap.isOpened()) break;
//    }
//    if (!cap.isOpened())
//    {
//        std::cout << "ERROR! Unable to open camera\n";
//        exit(-1);
//    }

//    //set resolution
//    if(res_w != -1 ) cap.set(CAP_PROP_FRAME_WIDTH,  res_w);
//    if(res_h != -1 ) cap.set(CAP_PROP_FRAME_HEIGHT, res_h);
//    cap.set(CAP_PROP_FRAME_HEIGHT,res_h);
//    res_w = cap.get(CAP_PROP_FRAME_WIDTH);
//    res_h = cap.get(CAP_PROP_FRAME_HEIGHT);

//    // try read image
//    int mattype = -1;
//    Mat test;
//    bool b = cap.read(test);
//    if(b==false)
//    {
//        std::cout << "ERROR! Unable to read image\n";
//        exit(-1);
//    }

//    // print camera info
//    res_w = test.cols;
//    res_h = test.rows;
//    mattype = test.type();

//    cout << "deviceID : " << deviceID << endl;
//    cout << "res      : " << res_w << " x " << res_h << endl;
//    cout << "mat_type : " << mattype << endl;

//    imCorrected.SetSize(res_h, res_w);

////    // for opencv camera debug
////    int ret;
////    char* pchar = (char*)&ret;
////    ret = cap.get(CAP_PROP_POS_MSEC);             cout << "CAP_PROP_POS_MSEC             = " << ret << endl;
////    ret = cap.get(CAP_PROP_POS_FRAMES);           cout << "CAP_PROP_POS_FRAMES           = " << ret << endl;
////    ret = cap.get(CAP_PROP_POS_AVI_RATIO);        cout << "CAP_PROP_POS_AVI_RATIO        = " << ret << endl;
////    ret = cap.get(CAP_PROP_FRAME_WIDTH);          cout << "CAP_PROP_FRAME_WIDTH          = " << ret << endl;
////    ret = cap.get(CAP_PROP_FRAME_HEIGHT);         cout << "CAP_PROP_FRAME_HEIGHT         = " << ret << endl;
////    ret = cap.get(CAP_PROP_FPS);                  cout << "CAP_PROP_FPS                  = " << ret << endl;
////    ret = cap.get(CAP_PROP_FOURCC);               cout << "CAP_PROP_FOURCC               = " << ret << " : " << (int)pchar[0] << "," << (int)pchar[1] << "," << (int)pchar[2] << "," << (int)pchar[3] << endl;
////    ret = cap.get(CAP_PROP_FRAME_COUNT);          cout << "CAP_PROP_FRAME_COUNT          = " << ret << endl;
////    ret = cap.get(CAP_PROP_FORMAT);               cout << "CAP_PROP_FORMAT               = " << ret << endl;
////    ret = cap.get(CAP_PROP_MODE);                 cout << "CAP_PROP_MODE                 = " << ret << endl;
////    ret = cap.get(CAP_PROP_BRIGHTNESS);           cout << "CAP_PROP_BRIGHTNESS           = " << ret << endl;
////    ret = cap.get(CAP_PROP_CONTRAST);             cout << "CAP_PROP_CONTRAST             = " << ret << endl;
////    ret = cap.get(CAP_PROP_SATURATION);           cout << "CAP_PROP_SATURATION           = " << ret << endl;
////    ret = cap.get(CAP_PROP_HUE);                  cout << "CAP_PROP_HUE                  = " << ret << endl;
////    ret = cap.get(CAP_PROP_GAIN);                 cout << "CAP_PROP_GAIN                 = " << ret << endl;
////    ret = cap.get(CAP_PROP_EXPOSURE);             cout << "CAP_PROP_EXPOSURE             = " << ret << endl;
////    ret = cap.get(CAP_PROP_CONVERT_RGB);          cout << "CAP_PROP_CONVERT_RGB          = " << ret << endl;
////    ret = cap.get(CAP_PROP_WHITE_BALANCE_BLUE_U); cout << "CAP_PROP_WHITE_BALANCE_BLUE_U = " << ret << endl;
////    ret = cap.get(CAP_PROP_RECTIFICATION);        cout << "CAP_PROP_RECTIFICATION        = " << ret << endl;
////    ret = cap.get(CAP_PROP_MONOCHROME);           cout << "CAP_PROP_MONOCHROME           = " << ret << endl;
////    ret = cap.get(CAP_PROP_SHARPNESS);            cout << "CAP_PROP_SHARPNESS            = " << ret << endl;
////    ret = cap.get(CAP_PROP_AUTO_EXPOSURE);        cout << "CAP_PROP_AUTO_EXPOSURE        = " << ret << endl;
////    ret = cap.get(CAP_PROP_GAMMA);                cout << "CAP_PROP_GAMMA                = " << ret << endl;
////    ret = cap.get(CAP_PROP_TEMPERATURE);          cout << "CAP_PROP_TEMPERATURE          = " << ret << endl;
////    ret = cap.get(CAP_PROP_TRIGGER);              cout << "CAP_PROP_TRIGGER              = " << ret << endl;
////    ret = cap.get(CAP_PROP_TRIGGER_DELAY);        cout << "CAP_PROP_TRIGGER_DELAY        = " << ret << endl;
////    ret = cap.get(CAP_PROP_WHITE_BALANCE_RED_V);  cout << "CAP_PROP_WHITE_BALANCE_RED_V  = " << ret << endl;
////    ret = cap.get(CAP_PROP_ZOOM);                 cout << "CAP_PROP_ZOOM                 = " << ret << endl;
////    ret = cap.get(CAP_PROP_FOCUS);                cout << "CAP_PROP_FOCUS                = " << ret << endl;
////    ret = cap.get(CAP_PROP_GUID);                 cout << "CAP_PROP_GUID                 = " << ret << endl;
////    ret = cap.get(CAP_PROP_ISO_SPEED);            cout << "CAP_PROP_ISO_SPEED            = " << ret << endl;
////    ret = cap.get(CAP_PROP_BACKLIGHT);            cout << "CAP_PROP_BACKLIGHT            = " << ret << endl;
////    ret = cap.get(CAP_PROP_PAN);                  cout << "CAP_PROP_PAN                  = " << ret << endl;
////    ret = cap.get(CAP_PROP_TILT);                 cout << "CAP_PROP_TILT                 = " << ret << endl;
////    ret = cap.get(CAP_PROP_ROLL);                 cout << "CAP_PROP_ROLL                 = " << ret << endl;
////    ret = cap.get(CAP_PROP_IRIS);                 cout << "CAP_PROP_IRIS                 = " << ret << endl;
////    ret = cap.get(CAP_PROP_SETTINGS);             cout << "CAP_PROP_SETTINGS             = " << ret << endl;
////    ret = cap.get(CAP_PROP_BUFFERSIZE);           cout << "CAP_PROP_BUFFERSIZE           = " << ret << endl;
////    ret = cap.get(CAP_PROP_AUTOFOCUS);            cout << "CAP_PROP_AUTOFOCUS            = " << ret << endl;

//    return 0;
//}

//void MyMouseEvent(int event, int x, int y, int flags, void* userdata)
//{
//    if (event == EVENT_LBUTTONDOWN)
//    {   //when left button clicked
////        cout << "Left click has been made, Position:(" << x << "," << y << ")" << endl;
//        HitFrame(x,y);
//    } else if (event == EVENT_LBUTTONUP)
//    {   //when left button up
////        cout << "Left button up has been made, Position:(" << x << "," << y << ")" << endl;
//        ReleaseFrame();
//    } else if (event == EVENT_MOUSEMOVE && flags == EVENT_FLAG_LBUTTON)
//    { //when mouse pointer moves
////        cout << "Current mouse position:(" << x << "," << y << ")" << endl;
//        DragFrame(x, y);
//    }
//}

//int HitFrame(int HitX, int HitY)
//{
//    for(int i=0;i<gNumPointFrame;++i)
//        if(gpPointFrame[i].HitTest(HitX, HitY) == kwFrame::HITTED)
//            return 1;
//    return 0;
//}

//int DragFrame(int HitX, int HitY)
//{
//    for(int i=0;i<gNumPointFrame;++i)
//        gpPointFrame[i].DragFrame(HitX, HitY);
//    return 1;
//}

//int ReleaseFrame()
//{
//    for(int i=0;i<gNumPointFrame;++i)
//        gpPointFrame[i].ReleaseHitStatus();
//    return 1;
//}

//int DrawFrame(Mat& DstMat)
//{
//    for(int i=0;i<gNumPointFrame;++i)
//        gpPointFrame[i].DrawFrame(DstMat);
//    return 1;
//}




//#include "main.h"

//#include "kwConst.h"
//#include "kwImage_object.h"
//#include "kwMatrix.h"
//#include "kwPoint.h"
//#include "kwTransPerspective.h"
//#include "kwImageTool.h"
//#include "kwMsrPoint.h"

//static int iCurrCorrespondence;
//static kwPoint ptCorrespondences[4];    // 透視轉換至少 4 個對應點
//void CallBackFunc(int event, int x, int y, int flags, void* userdata);

//int main()
//{
//    cout << "Hello CV!" << endl;

//    // initialize
//    iCurrCorrespondence = 0;
//    for (int i=0; i<4; ++i)
//    {
//        ptCorrespondences[i].x = kw_FMAX;
//        ptCorrespondences[i].y = kw_FMAX;
//    }

//    cv::Mat matGray;
//    matGray = cv::imread("", CV_LOAD_IMAGE_GRAYSCALE );
//    cv::imshow("Original Image", matGray);
//    cv::setMouseCallback("Original Image", CallBackFunc, NULL);   // select 4 point by mouse 

//    while ( iCurrCorrespondence < 4 )
//    {
//        for ( int kk=0; kk<4; ++kk)
//            if ( ptCorrespondences[kk].x != kw_FMAX )
//            {
//                cv::Point ptThis;
//                ptThis.x = ptCorrespondences[kk].x;
//                ptThis.y = ptCorrespondences[kk].y;
//                cv::line( matGray, ptThis, ptThis, cv::Scalar( 0,0,255), 4, 8 );
//            }
//        imshow("Original Image", matGray);
//        cv::waitKey(30);
//    }
//    cout << "Correspondence set ready!" << endl;

//    kwTransPerspective trPersp;

//    kwPoint ptWorld[4], ptCamera[4];
//    ptCamera[0] = ptCorrespondences[0];
//    ptCamera[1] = ptCorrespondences[1];
//    ptCamera[2] = ptCorrespondences[2];
//    ptCamera[3] = ptCorrespondences[3];

//    float Dx = ptCorrespondences[2].x-ptCorrespondences[3].x;
//    float Dy = ptCorrespondences[2].y-ptCorrespondences[3].y;
//    float fBaseLength = sqrtf(Dx*Dx+Dy*Dy);

//    ptWorld[0] = ( kwPoint{ptCorrespondences[3].x, ptCorrespondences[3].y-fBaseLength} );
//    ptWorld[1] = ( kwPoint{ptCorrespondences[3].x+fBaseLength, ptCorrespondences[3].y-fBaseLength} );
//    ptWorld[2] = ( kwPoint{ptCorrespondences[3].x+fBaseLength, ptCorrespondences[3].y} );
//    ptWorld[3] = ptCorrespondences[3];

//    cout << trPersp.m_00 << "," << trPersp.m_01 << "," << trPersp.m_02 << endl;
//    cout << trPersp.m_10 << "," << trPersp.m_11 << "," << trPersp.m_12 << endl;
//    cout << trPersp.m_20 << "," << trPersp.m_21 << "," << trPersp.m_22 << endl;
//    cout << "=============================" << endl;
//    trPersp.Calibrate( ptWorld, ptCamera, 4 );

//    cout << trPersp.m_00 << "," << trPersp.m_01 << "," << trPersp.m_02 << endl;
//    cout << trPersp.m_10 << "," << trPersp.m_11 << "," << trPersp.m_12 << endl;
//    cout << trPersp.m_20 << "," << trPersp.m_21 << "," << trPersp.m_22 << endl;
//    cout << "=============================" << endl;
//    kwImageU8 imCorrected( matGray.rows, matGray.cols );
//    // xx, yy are the new image coordinate

//    for (int yy =0; yy<imCorrected.rows; ++yy)
//    {
//        for (int xx =0; xx<imCorrected.cols; ++xx)
//        {
//            // take the pixel from the old image and put on the real world image
//            kwPoint ptNew = trPersp.MappingForward( kwPoint{(float)xx, (float)yy} );    // 在 old image 中取出 pixel value 的座標
//            cv::Point ptCV;
//            ptCV.x = (int)(ptNew.x+0.5f);
//            ptCV.y = (int)(ptNew.y+0.5f);
//            if ( ptCV.x < 0 || ptCV.x >= matGray.cols || ptCV.y < 0 || ptCV.y >= matGray.rows)
//                imCorrected.imgElement[yy][xx] = 0;
//            else{
//                imCorrected.imgElement[yy][xx] = matGray.at<uchar>(ptCV);
//            }
//        }
//    }
//    // put the image onto the opencv container
//    memcpy( matGray.data, imCorrected.membase, imCorrected.numOfPixels * sizeof(uchar) );
//    imshow("Corrected Image", matGray);


//    cv::waitKey(0);


//    return 0;
//}

////-------------------------------------------------------------------------------------
//void CallBackFunc(int event, int x, int y, int flags, void* userdata)
//{
//    if  ( event == cv::EVENT_LBUTTONDOWN )
//    {
//        if (iCurrCorrespondence<4)
//        {
//            ptCorrespondences[iCurrCorrespondence].x = (float)x;
//            ptCorrespondences[iCurrCorrespondence].y = (float)y;
//        }
//        ++iCurrCorrespondence;
//    }
//}
