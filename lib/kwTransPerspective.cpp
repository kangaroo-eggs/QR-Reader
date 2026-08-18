#include <iostream>

#include "kwTransPerspective.h"


using namespace std;

//-------------------------------------
/// \brief kwTransPerspective::kwTransPerspective
///
kwTransPerspective::kwTransPerspective()
{
    m_00 = m_01 = m_02 = m_10 = m_11 = m_12 = m_20 = m_21 = m_22 = 0;
    perTransMat.SetSize(3,3,false);
}


//-------------------------------------
// create the Perspective transformation matrix m
/// \brief kwTransPerspective::Calibrate
/// \param World, means the real world shape, which is prior knowledge
/// \param Camera, means the reference point we choose in the image
/// \param nNum
/// \return
///
kwMatrix kwTransPerspective::Calibrate( const kwPoint World[], const kwPoint Camera[], const int nNum )
{
    if(nNum < 4){
        cout << "lake of reference pts, need at least 4 point, return a Null matrix" << endl;
        kwMatrix NotSol(1,1, true);
        NotSol.Clear();
        return NotSol;
    }
    kwMatrix vec_camera, A;    // vec_camera is image coordinate(UV), A is coefficient matrix
    vec_camera.SetSize(2*nNum, 1, false);
    for (int i = 0; i < nNum; ++i){
        vec_camera.elematrix[2*i][0] = Camera[i].x;
        vec_camera.elematrix[2*i+1][0] = Camera[i].y;
    }

    A.SetSize(2*nNum, 8, true); // 8 because we have 8 unknown(m1~m8) need to be solve
    for (int i = 0; i < nNum; ++i){
        A.elematrix[2*i][0] = World[i].x;
        A.elematrix[2*i][1] = World[i].y;
        A.elematrix[2*i][2] = 1;
        float ux = Camera[i].x * World[i].x;
        float uy = Camera[i].x * World[i].y;
        A.elematrix[2*i][6] = -ux;
        A.elematrix[2*i][7] = -uy;

        A.elematrix[2*i+1][3] = World[i].x;
        A.elematrix[2*i+1][4] = World[i].y;
        A.elematrix[2*i+1][5] = 1;
        float vx = Camera[i].y * World[i].x;
        float vy = Camera[i].y * World[i].y;
        A.elematrix[2*i+1][6] = -vx;
        A.elematrix[2*i+1][7] = -vy;
    }

    kwMatrix sol(8, 1, false);
    if(nNum == 4){
        kwMatrix ExactSol, inverse;
        inverse = (!A);
        ExactSol = inverse * vec_camera;
        sol = ExactSol;
        cout << "4 point Calibrate" << endl;
    }
    if(nNum > 4){
        kwMatrix A_T, LeastSqrSol;
        A_T = A.Transpose();
        LeastSqrSol = (!(A_T * A)) * A_T * vec_camera;
        sol = LeastSqrSol;
    }
    m_00 = perTransMat.elematrix[0][0] = sol.membase[0];
    m_01 = perTransMat.elematrix[0][1] = sol.membase[1];
    m_02 = perTransMat.elematrix[0][2] = sol.membase[2];
    m_10 = perTransMat.elematrix[1][0] = sol.membase[3];
    m_11 = perTransMat.elematrix[1][1] = sol.membase[4];
    m_12 = perTransMat.elematrix[1][2] = sol.membase[5];
    m_20 = perTransMat.elematrix[2][0] = sol.membase[6];
    m_21 = perTransMat.elematrix[2][1] = sol.membase[7];
    m_22 = perTransMat.elematrix[2][2] = 1;

    return sol;
}


//-------------------------------------
/// @brief kwTransPerspective::MappingForward
/// @param ptWorld
/// @param ptCamera
/// @param nNum
///
void kwTransPerspective::MappingForward( kwPoint ptWorld[], kwPoint ptCamera[], int nNum )
{
    for (int i = 0; i < nNum; ++i) {
        ptCamera[i].x = ((this->m_00 * ptWorld[i].x + this->m_01 * ptWorld[i].y + this->m_02) /
                      (this->m_20 * ptWorld[i].x + this->m_21 * ptWorld[i].y + this->m_22));
        ptCamera[i].y = ((this->m_10 * ptWorld[i].x + this->m_11 * ptWorld[i].y + this->m_12) /
                      (this->m_20 * ptWorld[i].x + this->m_21 * ptWorld[i].y + this->m_22));
    }
}


//-------------------------------------
// this is the non-linear transformation, 用來算出該從哪裡取出 pixel value(pixelwise)
/// \brief kwTransPerspective::MappingForward
/// \param ptWorld (XY)
/// \return
///
kwPoint kwTransPerspective::MappingForward( kwPoint ptWorld )
{
    kwPoint OldPixel;
    OldPixel.x = (this->m_00 * ptWorld.x + this->m_01 * ptWorld.y + this->m_02) /
                 (this->m_20 * ptWorld.x + this->m_21 * ptWorld.y + this->m_22 + kw_FMIN_POSITIVE);
    OldPixel.y = (this->m_10 * ptWorld.x + this->m_11 * ptWorld.y + this->m_12) /
                 (this->m_20 * ptWorld.x + this->m_21 * ptWorld.y + this->m_22 + kw_FMIN_POSITIVE);
    return  OldPixel;
}


//-------------------------------------
/// \brief kwTransPerspective::MappingBackward
/// \param ptCamera
/// \param ptWorld
/// \param nNum
///
void kwTransPerspective::MappingBackward( kwPoint ptCamera[], kwPoint ptWorld[], int nNum )
{

}


//-------------------------------------
/// \brief kwTransPerspective::MappingBackward
/// \param ptCamera
/// \return
///
kwPoint kwTransPerspective::MappingBackward( kwPoint ptCamera )
{

}

