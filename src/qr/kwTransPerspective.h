#ifndef KWTRANSPERSPECTIVE_H
#define KWTRANSPERSPECTIVE_H

#include "kwMatrix.h"
#include "kwPoint.h"

//============================================================================
// Perspective Calibration & Transformations
//  - Calibrate(): given correspondence point sets (minimum: 4 sets),
//      ( World, Camera ), to calculate the transformation matrix;
//  - MappingForward(): based on the transformation matrix,
//      mapping World Coordinates --> Camera Coordinates;
//  - MappingBackward(): based on the transformation matrix,
//      mapping Camera Coordinates --> World Coordinates;
//============================================================================

//////////////////////////////////////////////////////////////////////////////////////
/// \brief The kwTransPerspective class, related with Perspective Transformation matrix
///
class kwTransPerspective
{
public:
    float                   m_00 = 0, m_01 = 0, m_02 = 0;
    float                   m_10 = 0, m_11 = 0, m_12 = 0;
    float                   m_20 = 0, m_21 = 0, m_22 = 0;
    kwMatrix                perTransMat;     // Perspective Transformation matrix
    bool                    calibrationSuccess;

    //-------------------------------------------------------------------------------------
    kwTransPerspective();

    inline bool             IsNull()
                            { return (m_22==0.f); }

    //-------------------------------------------------------------------------------------
    kwMatrix                Calibrate( const kwPoint World[],   // (X_i, Y_i)
                                       const kwPoint Camera[],  // (u_i, v_i)
                                       const int nNum );        // number of corresponding point, one point can provide two eq, so we need at least four corresponding point

    void                    MappingForward( kwPoint ptWorld[], kwPoint ptCamera[], int nNum );

    void                    MappingBackward( kwPoint ptCamera[], kwPoint ptWorld[], int nNum );

    kwPoint                 MappingForward( kwPoint ptWorld );

    kwPoint                 MappingBackward( kwPoint ptCamera );
};


#endif // KWTRANSPERSPECTIVE_H
