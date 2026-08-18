#ifndef KWLINE_H
#define KWLINE_H

#include "kwPoint.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The kwLine class
///
class kwLine {
public:
    ///@brief $R=Xcos+Ysin$
    float           theta;
    float           radius;
    ///@brief $Y=MX+B$
    float           slope;
    float           intercept;

    //-------------------------------------
    ///@brief Contructor
    kwLine();
    kwLine(float _T, float _R);

    ///@brief Transfer two line expression
    void MB2TR();
    void TR2MB();

    ///@brief Operator
    ///@brief Operator = : Assign line to line
    kwLine &operator=(kwLine srcLine);
    ///@brief Operator = : Assign point and (0,0) to line
    kwLine &operator=(kwPoint srcPoint);
    ///@brief Operator = : Assign pointpair to line
    kwLine &operator=(kwPointPair srcPointPair);
    ///@brief Operator ==
    bool operator==(kwLine srcLine);
    ///@brief Operator !=
    bool operator!=(kwLine srcLine);

    /// Function

    ///@brief Set the parameter, to determine a Line
    void SetTR(float _T, float _R);
    void SetMB(float _M, float _B);
    void Set(kwPoint p1, kwPoint p2);
    void Set(float slope, kwPoint p1);

    ///@brief Compute the distance from point to line
    float DistanceTo(kwPoint pt);
    ///@brief return Y of point with X on the line
    float EvaluatAtX(float Xvalue);
    ///@brief return X of point with Y on the line
    float EvaluatAtY(float Yvalue);
    ///@brief Dtermine whether the line is NULL
    bool IsNull();
    ///@brief IsVertical
    bool IsVertical();
    ///@brief Set parameter to 0
    void Clear();
    ///@brief Compute two lines intersection
    kwPoint LineIntersection(kwLine Line);
    ///@brief Compute the regression line
    static kwLine Fitting(kwPoint sample[], int Num);
};

#endif // KWLINE_H
