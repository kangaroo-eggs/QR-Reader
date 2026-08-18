#ifndef KWMATRIX_H
#define KWMATRIX_H

#include "kwConst.h"

/////////////////////////////////////////////////////////////////////////////////
/// @brief
///
// define Null matrix: membase = 0
class kwMatrix
{
public:

    int             rows;
    int             cols;
    int             numOfElements;

    float           *membase;
    float           *memend;
    float*          *elematrix;

    //-------------------------------------
    // 這裡要注意，在傳參數的時候，如果是透過 call by value，會複製一份新的 kwMatrix，然後在副程式結束時，會啟動 distructor，
    // 這樣會造成，原本存放 data 的地方被 free 掉，所以傳參數的時候要用 call by reference
    kwMatrix( int nRowNum = 1, int nColNum = 1, bool clear = true );    // clear only means set the matrix's value to 0
    kwMatrix( const kwMatrix &CopyFrom );   // copy full matrix, not just the pointer
    ~kwMatrix();
    bool            IsNull();  // check if this matrix is Null
    bool            IsZero();  // check if this matrix's all elem are 0
    bool            IsSymmetric();
    bool            IsUpperTriangular();   // i>j 要是 0
    bool            IsLowerTriangular();   // i<j 要是 0
    void            SetAllZero();
    void            SetAllValue( float Val );
    /// @brief set the square matrix into identity
    void            SetIdentity();
    void            SetEye( float DiagonalVal = 1.f );    // 1.f 表示將 1 表示成 float
    bool            SetSize( int nRowNum, int nColNum, bool clear = true );
    bool            SetSize( kwMatrix &Ref, bool clear = true );

    float           GetElement( int nRowIndex, int nColIndex );
    void            SetElement( int nRowIndex, int nColIndex, float val );
    /// @brief set a matrix into Null
    void            Clear();

    //-------------------------------------
    float           SumOfSquares();
    kwMatrix        Transpose();

    //-------------------------------------
    /// @brief equiv to GetElement()
    float           operator()( int nRowIndex, int nColIndex );
    /// @brief equiv to SetElement()
    float           operator()( int nRowIndex, int nColIndex, float SetValue );

    /// Operator =
    // return by reference(Note that return by reference can't return local var or const), in this case, will return itself
    // const is needed
    kwMatrix&       operator=( const kwMatrix &CopyFrom);

    kwMatrix        operator+( kwMatrix &Addend );
    kwMatrix        operator+( float etAddend);

    kwMatrix        operator-( kwMatrix &Subtrahend );
    kwMatrix        operator-( float etSubtrahend);

    /// @brief Operator * -- Normal matrix multiplication
    kwMatrix        operator*( kwMatrix &Multiplier);
    kwMatrix        operator*( float etMultiplier);

    /// @brief Operator /
    kwMatrix        operator/( kwMatrix &Divisor);
    kwMatrix        operator/( float etDivisor);

    /// @brief Operator % -- Hadamard Product <entry-wise multiplications>
    kwMatrix        operator%( kwMatrix &HardamardMul);

    /// Operator ! (Inverse)
    kwMatrix        operator!() const; // const 表示此 member function 不能改到 this 的 data，是一個 read only function
    kwMatrix        Inverse() const;

    //-------------------------------------
    /// @brief interchange two given row
    void            Type1_RowOperation(int row1, int row2);
    /// @brief multi one row
    void            Type2_RowOPeration(int row, float mult);
    /// @brief mult first row and add to 2nd row
    void            Type3_RowOperation(int row1, float mult, int row2);
    float           Determinant();
    /// @brief return the row echlon form of this matrix
    kwMatrix        Row_Echlon_Form();
};

#endif // KWMATRIX_H
