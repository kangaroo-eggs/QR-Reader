#ifndef KWPOINT_H
#define KWPOINT_H

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "kwConst.h"
#include "main.h"

class kwPoint;
class kwPointInt;
class kwPointPair;

/////////////////////////////////////////////////////////////////////////////////
/// \brief The kwPoint class
///
class kwPoint
{
public:
    float           x;
    float           y;

    //-------------------------------------
    /// @brief Initialize kwPoint as \f$(0.f,0.f)\f$.
    kwPoint() : x(0.f), y(0.f) {}   // "：" 表示簡化版的 constructor 寫法，意思是在之前會先把值指定給 x, y，{} 中依然可以做其他事

    /// @brief Initialize kwPoint as \f$(x,y)\f$.
    kwPoint(float _x, float _y) : x(_x), y(_y) {}

    /// @brief Assign coordinate \f$(x,y)\f$ to this point.
    kwPoint                 Set( float vx, float vy );

    /// @brief Assign this point to a null point, i.e. \f$(\textrm{kw_FLT_MAX}, \textrm{kw_FLT_MAX})\f$.
    inline void             Clear() { this->x = kw_FMAX; this->y = kw_FMAX; }

    void                    SetNull();

    /// @brief Check if the point is null or not.
    inline bool             IsNULL()
                            { return ((x==kw_FMAX) || (y==kw_FMAX)); }

    /// @brief Check if the point is zero.
    inline bool             IsZero()
                            { return ((x==0) && (y==0)); }

    //-------------------------------------
    /// @brief Calculate the middle point between this point and the operand.
    kwPoint                 MidPoint( kwPoint &operand ) const;

    /// @brief Rotate this point with respect to the origin for 90 degrees clockwise.
    inline kwPoint          Rotate90DegClockwise()
                            {   return kwPoint{ this->y, -(this->x) };   }  // 沒有名字的 class object

    /// @brief Rotate this point with respect to the origin for 90 degrees counterclockwise.
    kwPoint                 Rotate90DegCounterClockwise()
                            {   return kwPoint{ -(this->y), this->x };   }

    /// @brief Calculate the length of this point to the origin, i.e. the length of the vector.
    float                   Norm2(void);

    /*! Check if this point is within the triangle enclosed by point A, B, and C.
        @param kwPoint A, B, C
        @return true or false
        @image html kwPoint_IsWithinTriangle.png */
    bool                    IsWithinTriangle( kwPoint &A, kwPoint &B, kwPoint &C ) const;

    /// @brief Swap this point coordinate with a kwPoint.
    void                    Swap( kwPoint &srcPoint );

    ///@brief
    kwPoint                 Project(kwPoint &that);
    //-------------------------------------------------
    /// @brief Assign coordinate \f$(x,y)\f$ to this point, same as Set(x,y).
    inline kwPoint          operator()( float _x, float _y )
                            {   this->x = _x;   this->y = _y;   return (*this);   }

    /// @brief Assign coordinate \f$(fVal,fVal)\f$ to this point.
    kwPoint&                operator=( float fVal );

    /// @brief Assignment for kwPoint.
    kwPoint&                operator=( kwPoint srcPoint );

    /// @brief Convert this kwPointPair into a kwPoint, similar to MakeVector().
    kwPoint&                operator=( kwPointPair srcPointPair );

    /// @brief \f$(x,y)\leftarrow(x,y)+(u,v)\f$.
    kwPoint&                operator+=( kwPoint addend );

    /// @brief \f$(x,y)\leftarrow(x,y)-(u,v)\f$.
    kwPoint&                operator-=( kwPoint subtrahend );

    /// @brief \f$(x,y)\leftarrow(x,y)*fFactor\f$.
    kwPoint&                operator*=( float fFactor );

    /// @brief \f$(x,y)\leftarrow(x,y)/fFactor\f$.
    kwPoint&                operator/=( float fFactor );

    /// @brief Test of logical comparision -- equal.
    bool                    operator==( kwPoint operand2 ) const;

    /// @brief Test of logical comparision -- not equal.
    bool                    operator!=( kwPoint operand2 ) const;

    /// @brief Test of logical comparision -- equal.
    bool                    operator==( float fVal ) const;

    /// @brief Test of logical comparision -- not equal.
    bool                    operator!=( float fVal ) const;

    /// @brief Calculate vector addition with the operand (kwPoint).
    kwPoint                 operator+( kwPoint operand ) ;

    /// @brief Scalar addition: \f$(x,y)\rightarrow(x+fShift,y+fShift)\f$.
    kwPoint                 operator+( float fShift ) ;

    /// @brief Calculate vector subtraction with the operand (kwPoint).
    kwPoint                 operator-( kwPoint subtrahend ) ;

    /// @brief Scalar subtraction: \f$(x,y)\rightarrow(x-fShift,y-fShift)\f$.
    kwPoint                 operator-( float fShift ) ;

    /// @brief Calculate scalar product (inner product) with the operand (kwPoint).
    float                   operator*( kwPoint operand ) ;

    /*! Calculate cross product (outer product) with the operand.
        @param kwPoint operand
        @return a scalar;  > 0, if this vector sweeps to the operand vector clockwise, otherwise, < 0. */
    float                   operator%( kwPoint operand ) ;

    /// @brief calculate determinent
    float                   operator||(kwPoint that);

    /// @brief Scalar multiplication.
    kwPoint                 operator*( float multiplier ) ;

    /// @brief Scalar division.
    kwPoint                 operator/( float divisor ) const;

    /// @brief Normalize to Unit Vector.
    kwPoint                 operator~(void) const;

    /// @brief Calculate Angle(in radians) between two Vectors.
    float                   operator^( kwPoint operand ) const;

    /// @brief Negate a vector, \f$(x,y)\rightarrow(-x,-y)\f$.
    kwPoint                 operator-(void) const;

    //-------------------------------------------------
    /*! Convert an ordered pair into an origin-based vector (for kwPoint).
        @param kwPoint P1 and P2
        @return kwPoint (a vector)
        @image html kwPoint_MakeVector.png */
    kwPoint                 MakeVector( kwPoint P1, kwPoint P2 );

    /*! Convert an ordered pair into an origin-based unit vector (for kwPoint).
        @param kwPoint P1 and P2
        @return kwPoint (a unit vector)
        @image html kwPoint_MakeUnitVector.png */
    kwPoint                 MakeUnitVector( kwPoint P1, kwPoint P2 );

    /*! Convert a point pair into an origin-based unit vector (for kwPointPair).
        @param kwPointPair
        @return kwPoint (a unit vector) */
    kwPoint                 MakeUnitVector( kwPointPair ptpPair );

    /// @brief Calculate distance from this point to the operand, same as DistanceTo().
    float                   operator|( kwPoint operand ) const;

    /*! Calculate distance from this point to the operand (for kwPoint).
    The distance between this point \f$(x_1,y_1)\f$ and \f$(x_2,y_2)\f$ is calculated with the formula \f$\sqrt{(x_1-x_2)^2+(y_1-y_2)^2}\f$.
    @param operand - a kwPoint
    @return distance (in float) between the two points */
    float                   DistanceTo( kwPoint ptPoint );

    /*! Calculate distance from this point to a line specified by a kwPointPair.
    The shortest distance between this point and the line specified.
    @param operand - a kwPointPair
    @return distance (in float) */
    float                   DistanceTo( kwPointPair ptpPointPair );

    /*! Project a vector(vVector) to a reference vector(vReference) w/ the result direction as the reference.
        @param kwPoint vReference - a point(a vector) as a reference direction to project
        @return a vector w/ direction as vReference and length as the projection
        @image html kwPoint_Projection.PNG */
    inline static kwPoint   Projection( kwPoint vVector, kwPoint vReference )
                            {    return vReference*((vVector*vReference)/(vReference*vReference));      }

    /// @brief Static function of calculation of distance from one kwPoint to another.
    static float            Distance( kwPoint ptPoint1, kwPoint ptPoint2 );
    // 靜態成員函式，代表它是「與 class 相關連」，而不是「與物件相關連」的變數。它獨立配置記憶體，獨立於 class 的任何物件而存在，注意到當它存取 class member 時，只能存取靜態成員變數


    /// @brief Static function of calculation of distance from one kwPoint to a line kwPointPair.
    static float            Distance( kwPoint ptPoint, kwPointPair ptpPointPair );
};



/////////////////////////////////////////////////////////////////////////////////
/// \brief The kwPointInt class, for opencv img used (integer coordinate)
///
class kwPointInt
{
public:
    int           x;
    int           y;

    //-------------------------------------
    /// @brief Initialize kwPointInt as \f$(0.f,0.f)\f$.
    kwPointInt() : x(0), y(0) {}
    /// @brief Initialize kwPointInt as \f$(x,y)\f$.
    kwPointInt(int _x, int _y) : x(_x), y(_y) {}

    /// @brief Assign coordinate \f$(x,y)\f$ to this point.
    kwPointInt              Set( int vx, int vy );

    /// @brief Assign this point to a null point, i.e. \f$(\textrm{kw_FLT_MAX}, \textrm{kw_FLT_MAX})\f$.
    inline void             Clear()
                            { this->x = kw_INT32_MAX; this->y = kw_INT32_MAX; }

    /// @brief Calculate the length of this point to the origin, i.e. the length of the vector.
    float                   Norm2(void) const;

    /// @brief Check if the point is null or not.
    inline bool             IsNULL()
                            { return ((x==kw_INT32_MAX) || (y==kw_INT32_MAX)); }

    /// @brief Check if the point is zero.
    inline bool             IsZero()
                            { return ((x==0) && (y==0)); }

    /*! Check if this point is within the triangle enclosed by point A, B, and C.
        @param kwPointInt A, B, C
        @return true or false
        @image html kwPoint_IsWithinTriangle.png */
    bool                    IsWithinTriangle( kwPointInt &A, kwPointInt &B, kwPointInt &C ) const;

    /// @brief Swap this point coordinate with a kwPoint.
    void                    Swap( kwPointInt &srcPoint );

    //-------------------------------------------------
    /// @brief Assign coordinate \f$(x,y)\f$ to this point, same as Set(x,y).
    inline kwPointInt       operator()( int _x, int _y )
                            {   this->x = _x;   this->y = _y;   return (*this);   }

    /// @brief Assign coordinate \f$(fVal,fVal)\f$ to this point.
    kwPointInt&             operator=( int iVal );

    /// @brief Assignment for kwPoint.
    kwPointInt&             operator=( kwPointInt srcPoint );

    /// @brief Convert this kwPointPair into a kwPoint, similar to MakeVector().
    kwPointInt&             operator=( kwPointPair srcPointPair );

};



/////////////////////////////////////////////////////////////////////////////////
/// \brief The kwPointPair class
///
class kwPointPair
{
public:
    kwPoint         p1;
    kwPoint         p2;

    //-------------------------------------------------
    kwPointPair();
    kwPointPair(kwPoint _p1, kwPoint _p2);
    kwPointPair(float _p1x, float _p1y, float _p2x, float _p2y);
    ///@brief Dtermine whether the pointpair is NULL
    bool            IsNull();
    ///@brief Set the pointpair to NULL
    void            SetNull();
    ///@brief Set the pointpair to 0
    void            Clear();
    ///@brief Dtermine whether the point is 0
    bool            IsZero();


    //-------------------------------------------------
    ///@brief Operator () : set the pointpair
    kwPointPair     operator()(kwPoint _p1, kwPoint _p2);
    kwPointPair     operator()(float _p1x, float _p1y, float _p2x, float _p2y);
    ///@brief Operator =
    kwPointPair&    operator=(kwPointPair &srcPointPair);
    ///@brief Operator = : Assign a point as a vector to a pointpair
    kwPointPair&    operator=(kwPoint srcPoint);
    ///@brief Operator ==
    bool            operator==(kwPointPair operand2);
    bool            operator!=(kwPointPair operand2);
    ///@brief Operator +
    kwPointPair     operator+(kwPoint TranslateVector);
    ///@brief Operator -
    kwPointPair     operator-(kwPoint TranslateVector);
    ///@brief Operator * : Scalar multiply
    kwPointPair     operator*(float scale);
    ///@brief Operator * : Inner product
    float           operator*(kwPointPair oprand2);
    ///@brief Operator || : Dterminent if > 0 it is counter clockwise if < 0 it is clockwise
    float           operator||(kwPointPair operand2);
    ///@brief Operator ~ : Make a nunit vector
    kwPoint         operator~();
    ///@brief Operator ^ : Compute the cosine between two points
    float           operator^(kwPointPair operand2);
    ///@brief Operator - : Swap P1 ans P2
    kwPointPair     operator-() const;

    //-------------------------------------------------
    ///@brief return the y of position with x on the pointpair line
    float           EvaluateAtX(float Xvalue);
    ///@brief return the x of position with y on the pointpair line
    float           EvaluateAtY(float Yvalue);
    ///@brief Make a verctor from P1 to P2
    kwPoint         MakeVector();
    ///@brief Compute the length between P1 and P2
    float           Length();
    ///@brief Swap two Pointpair
    void            swap(kwPointPair &srcPointPair);
    ///@brief return the midpoint between p1 ans p2
    kwPoint         MidPoint();
    ///@brief Dtermine whether two line of pointpair have intersection
    bool            IsIntersected(kwPointPair ptpLine1, kwPointPair ptpLine2);
    ///@brief return the intersection of two pointpair line
    kwPoint         LineIntersection(kwPointPair ptpLine1, kwPointPair ptpLine2);


};




/////////////////////////////////////////////////////////////////////////////////
/// \brief The kwPointPairNode class
///
class kwPointPairNode : public kwPointPair
{
public:
    kwPointPairNode         *Previous;
    kwPointPairNode         *Next;
    float                   Rank;

    //-------------------------------------
    ///@brief constructor
    kwPointPairNode();
    kwPointPairNode(float x1, float y1, float x2, float y2, float _rank);
    ///@brief Operator =
    kwPointPairNode&        operator=(kwPointPairNode &that);
};

#endif // KWPOINT_H
