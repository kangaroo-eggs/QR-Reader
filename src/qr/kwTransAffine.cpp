//#include "kwTransAffine.h"

//kwTransAffine::kwTransAffine() {
//  m_fM00 = m_fM01 = m_fM02 = m_fM10 = m_fM11 = m_fM12 = m_fM20 = m_fM21 = m_fM22 = kw_fMAX;
//  TransfM.SetSize(3, 3, false);
//}
////-------------------------------------------------------------------------------------------------
//kwMatrix<float> kwTransAffine::Calibrate(const kwPoint World[], const kwPoint Camara[],
//                                         const int Num) {
//  if (Num < 3) {
//    kwMatrix<float> temp;
//    return temp;
//  } else {
//    kwMatrix<float> temp(Num * 2, 6, true);
//    kwMatrix<float> CamaraCol(Num * 2, 1);
//    kwMatrix<float> transpose;
//    kwMatrix<float> TransMatrixCol;
//    for (int i = 0; i < Num; i++) {
//      temp.m_pMtrixElement[i * 2][0] = World[i].x;
//      temp.m_pMtrixElement[i * 2][1] = World[i].y;
//      temp.m_pMtrixElement[i * 2][2] = 1;
//      CamaraCol.m_pMtrixElement[i * 2][0] = Camara[i].x;

//      temp.m_pMtrixElement[i * 2 + 1][3] = World[i].x;
//      temp.m_pMtrixElement[i * 2 + 1][4] = World[i].y;
//      temp.m_pMtrixElement[i * 2 + 1][5] = 1;
//      CamaraCol.m_pMtrixElement[i * 2 + 1][0] = Camara[i].y;
//    }
//    transpose = temp.Transpose();
//    TransMatrixCol = transpose * temp;
//    TransMatrixCol = !TransMatrixCol;
//    TransMatrixCol = TransMatrixCol * transpose;
//    TransMatrixCol = TransMatrixCol * CamaraCol;

//    m_fM00 = TransfM.m_pMtrixElement[0][0] = TransMatrixCol.m_pMemoryBase[0];
//    m_fM01 = TransfM.m_pMtrixElement[0][1] = TransMatrixCol.m_pMemoryBase[1];
//    m_fM02 = TransfM.m_pMtrixElement[0][2] = TransMatrixCol.m_pMemoryBase[2];
//    m_fM10 = TransfM.m_pMtrixElement[1][0] = TransMatrixCol.m_pMemoryBase[3];
//    m_fM11 = TransfM.m_pMtrixElement[1][1] = TransMatrixCol.m_pMemoryBase[4];
//    m_fM12 = TransfM.m_pMtrixElement[1][2] = TransMatrixCol.m_pMemoryBase[5];
//    m_fM20 = TransfM.m_pMtrixElement[2][0] = 0;
//    m_fM21 = TransfM.m_pMtrixElement[2][1] = 0;
//    m_fM22 = TransfM.m_pMtrixElement[2][3] = 1;
//    return TransfM;
//  }
//}
////-------------------------------------------------------------------------------------------------
//void kwTransAffine::MappingForward(kwPoint ptWorld[], kwPoint Camara[], const int Num) {
//  for (int i = 0; i < Num; i++) {
//    Camara[i].x = ((m_fM00 * ptWorld[i].x + m_fM01 * ptWorld[i].y + m_fM02) > 0)
//                      ? (m_fM00 * ptWorld[i].x + m_fM01 * ptWorld[i].y + m_fM02)
//                      : 0;
//    Camara[i].y = (m_fM10 * ptWorld[i].x + m_fM11 * ptWorld[i].y + m_fM12 > 0)
//                      ? m_fM10 * ptWorld[i].x + m_fM11 * ptWorld[i].y + m_fM12
//                      : 0;
//  }
//}
