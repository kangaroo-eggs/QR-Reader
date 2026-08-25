#include <memory.h>
#include <iostream>

#include "kwMatrix.h"
#include "kwConst.h"

using namespace std;

kwMatrix::kwMatrix(int nRowNum, int nColNum, bool clear)
{
    if ( nRowNum <= 0 || nColNum <= 0 ){
        cout << "invalid rows or cols, set rows = cols = 1" << endl;
        this->rows = this->cols = 1;
    }else
        this->rows = nRowNum, this->cols = nColNum;
    this->numOfElements = this->rows * this->cols;

    this->membase = new float[this->numOfElements];
    this->memend = this->membase + this->numOfElements - 1;
    this->elematrix = new float*[this->rows];
    if (clear == true)
        memset( this->membase, 0, this->numOfElements * sizeof(float) );   // 所有的 0 都一樣，不論 float or int, 不用打 0.f
    float* tmp = this->membase;
    for( int r = 0; r < this->rows; ++r, tmp += this->cols )
        this->elematrix[r] = tmp;
}

//-------------------------------------------------
kwMatrix::kwMatrix(const kwMatrix& copyfrom) // copy constructor
{
    if( copyfrom.membase == 0 ){
        cout << "can't assign a Null matrix, return a Null matrix" << endl;
        this->membase = 0;
        return;
    }
    if ( copyfrom.rows <= 0 || copyfrom.cols <= 0 ){
        cout << "invalid rows or cols, return a Null matrix" << endl;
        this->membase = 0;
        return;
    }
    this->rows = copyfrom.rows, this->cols = copyfrom.cols, this->numOfElements = copyfrom.numOfElements;
    this->membase = new float[this->numOfElements];
    this->memend = this->membase + this->numOfElements - 1; // 實際上是加 (this->numOfElements - 1)*4 byte
    this->elematrix = new float*[this->rows];
    float* tmp = this->membase;
    for (int r = 0; r < this->rows; ++r, tmp += this->cols)
        this->elematrix[r] = tmp;
    memcpy(this->membase, copyfrom.membase, this->numOfElements * sizeof(float) );
}

//-------------------------------------------------
kwMatrix::~kwMatrix()
{
    if(this->membase != 0)
        delete [] membase;
    if(this->elematrix != 0)
        delete [] elematrix;
}

//-------------------------------------------------
bool kwMatrix::IsNull()
{
    return this->membase == 0 ? true : false;
}

//-------------------------------------------------
bool kwMatrix::IsZero()
{
    for ( float* data = this->membase; data <= this->memend; ++data ){
        if( *data != 0)
            return false;
    }
    return true;
}

//-------------------------------------------------
bool kwMatrix::IsSymmetric()
{
    if( this->rows != this->cols )
        return false;
    if( this->rows == 1 && this->cols == 1 )
        return  true;

    for( int r = 0; r < this->rows; ++r ){
        for( int c = 0; c < this->cols; ++c ){
            if( this->elematrix[r][c] != this->elematrix[c][r] )
                return false;
        }
    }
    return true;
}

//-------------------------------------------------
bool kwMatrix::IsUpperTriangular()
{
    for(int r = 0; r < this->rows; ++r){
        for(int c = 0; c < r; ++c){
            if(this->elematrix[r][c] != 0)
                return  false;
        }
    }
    return true;
}

//-------------------------------------------------
bool kwMatrix::IsLowerTriangular()
{
    for(int r = 0; r < this->rows; ++r){
        for(int c = r+1; c < this->cols; ++c){
            if(this->elematrix[r][c] != 0)
                return  false;
        }
    }
    return true;
}

//-------------------------------------------------
void kwMatrix::SetAllZero()
{
    memset(this->membase, 0, this->numOfElements * sizeof(float));
}

//-------------------------------------------------
void kwMatrix::SetAllValue(float Val)
{
    for( int i = 0; i < this->numOfElements; ++i)  //不能用 memset，因為 memset 是一個 byte(8 bits) 一個 byte 去清，而 float 是 32 bits
        this->membase[i] = Val;
}

//-------------------------------------------------
void kwMatrix::SetIdentity()
{
    if(this->rows != this->cols){
        cout << "rows != cols, can't turn this matrix into an identity matrix" << endl;
        return;
    }
    memset(this->membase, 0, this->numOfElements * sizeof(float));
    for(int i = 0; i < this->rows; ++i)
        this->elematrix[i][i] = 1.f;
}

//-------------------------------------------------
void kwMatrix::SetEye(float DiagVal)
{
    if(this->rows != this->cols){
        cout << "rows != cols, can't turn this matrix into an eye" << endl;
        return;
    }
    memset(this->membase, 0, this->numOfElements * sizeof(float));
    for(int i = 0; i < this->rows; ++i)
        this->elematrix[i][i] = DiagVal;
}

//-------------------------------------------------
bool kwMatrix::SetSize(int nRowNum, int nColNum, bool clear)
{
    if ( nRowNum <= 0 || nColNum <= 0 ){
        cout << "invalid value of rows or cols" << endl;
        return false;
    }
    if ( nRowNum == this->rows && nColNum == this->cols ){  // size 不變
        if (clear)
            memset( this->membase, 0, this->numOfElements * sizeof(float) );
    }
    else{
        this->rows = nRowNum;
        this->cols = nColNum;
        this->numOfElements = this->rows * this->cols;

        if ( this->membase != 0 )   // matrix is not NULL
            delete [] this->membase;
        this->membase = new float[this->numOfElements];
        this->memend = this->membase + this->numOfElements - 1;

        if ( this->elematrix != 0 )
            delete [] this->elematrix;
        this->elematrix = new float*[this->rows];

        if (clear)
            memset( this->membase, 0, this->numOfElements * sizeof(float) );

        float* tmp = this->membase;
        for( int i = 0; i < this->rows; ++i, tmp += this->cols )
            this->elematrix[i] = tmp;
    }
    return true;
}

//-------------------------------------------------
bool kwMatrix::SetSize(kwMatrix& ref, bool clear)
{
    return kwMatrix::SetSize(ref.rows, ref.cols, clear);
}

//-------------------------------------------------
kwMatrix kwMatrix::Transpose()
{
    kwMatrix result(this->cols, this->rows, false);
    for (int r = 0; r < this->rows; ++r)
        for(int c = 0; c < this->cols; ++c)
            result.elematrix[c][r] = this->elematrix[r][c];
    return result;
}

//-------------------------------------------------
float kwMatrix::SumOfSquares()
{
    float sum = 0;
    for (int r = 0; r < this->rows; ++r)
        for(int c = 0; c < this->cols; ++c)
            sum += this->elematrix[r][c]*this->elematrix[r][c];
    return  sum;
}

//-------------------------------------------------
void kwMatrix::Clear()
{
    if ( this->membase != 0 )
        delete [] this->membase;
    if ( this->elematrix != 0 )
        delete [] this->elematrix;
    this->membase = 0;  // main purpose
    this->elematrix = 0;
    this->rows = this->cols = 1;
    this->numOfElements = 1;
}

//-------------------------------------------------
//operator

//-------------------------------------------------
float kwMatrix::operator()( int nRowIndex, int nColIndex )
{
    return this->elematrix[nRowIndex][nColIndex];
}


//-------------------------------------------------
kwMatrix& kwMatrix::operator=( const kwMatrix& CopyFrom )
{
    if( CopyFrom.membase == 0 ){
        cout << "can't assign a Null matrix, return a Null matrix" << endl;
        this->membase = 0;
        return *this;
    }
    if ( CopyFrom.rows <= 0 || CopyFrom.cols <= 0 ){
        cout << "invalid rows or cols, return a Null matrix" << endl;
        this->membase = 0;
        return *this;
    }

    if ( this != &CopyFrom )
    {
        if ( this->rows != CopyFrom.rows || this->cols != CopyFrom.cols )
        {
            if ( this->rows != CopyFrom.rows )
            {
                delete [] this->elematrix;
                this->rows = CopyFrom.rows;
                this->elematrix = new float*[ this->rows ]; // create a new elematrix with different size
            }

            this->cols = CopyFrom.cols;
            this->numOfElements = CopyFrom.numOfElements;

            delete [] this->membase;
            this->membase = new float[ this->numOfElements ];
            this->memend = this->membase + this->numOfElements - 1;

            float* tmp = this->membase;
            for ( int r=0; r < this->rows; ++r, tmp += this->cols )
                this->elematrix[r] = tmp;
        }
        memcpy( this->membase, CopyFrom.membase, this->numOfElements * sizeof(float) );
    }
    return *this;
}

//-------------------------------------------------
kwMatrix kwMatrix::operator+( kwMatrix& CopyFrom )
{
    if(CopyFrom.rows != this->rows || CopyFrom.cols != this->cols){
        cout << "wrong size, can't add two matrix, return a Null matrix" << endl;
        kwMatrix result(1, 1, false);
        result.Clear();
        return  result;
    }

    kwMatrix result(this->rows, this->cols, false);
    for (int r = 0; r < this->rows; ++r)
        for(int c = 0; c < this->cols; ++c)
            result.elematrix[r][c] = this->elematrix[r][c] + CopyFrom.elematrix[r][c];
//    cout << "result membase:" << result.membase << endl;
    // case: (A+B)
    // (A+B).membase != result.membase, 可以得到結論, result 在離開 function 時會被 destruct 掉, 但是不會影響到 (A+B)
    // case: C = A+B
    // C.membase != (A+B).membase != result.membase, by 上面結論, 當然也不會影響到 C
//    cout << result.cols << "," << result.cols << "," << result.membase << endl;

    return result;
}

//-------------------------------------------------
kwMatrix kwMatrix::operator-( kwMatrix& CopyFrom )
{
    if(CopyFrom.rows != this->rows || CopyFrom.cols != this->cols){
        cout << "wrong size, can't sub two matrix, return a Null matrix" << endl;
        kwMatrix result(1, 1, false);
        result.Clear();
        return  result;
    }

    kwMatrix result(this->rows, this->cols, false);
    for (int r = 0; r < this->rows; ++r)
        for(int c = 0; c < this->cols; ++c)
            result.elematrix[r][c] = this->elematrix[r][c] - CopyFrom.elematrix[r][c];

    return result;
}

//-------------------------------------------------
kwMatrix kwMatrix::operator*( kwMatrix& multi )
{
    if(this->cols != multi.rows){
        cout << "wrong size, can't multi two matrix, return a Null matrix" << endl;
        kwMatrix result(1, 1, false);
        result.Clear();
        return  result;
    }
    kwMatrix result( this->rows, multi.cols, true );
    // O(n^3)可被優化
    for (int r = 0; r < this->rows; ++r)
        for(int c = 0; c < multi.cols; ++c)
            for (int k = 0; k < this->cols; ++k)    // this->cols = multi.rows
                result.elematrix[r][c] += this->elematrix[r][k] * multi.elematrix[k][c];
//    cout << result.cols << "," << result.cols << "," << result.membase << endl;

    return result;
}

//-------------------------------------------------
kwMatrix kwMatrix::operator*( float multiple )
{
    kwMatrix result(*this); // full copy
    for (int r = 0; r < this->rows; ++r)
        for(int c = 0; c < this->cols; ++c)
            result.elematrix[r][c] *= multiple;
    return result;
}

//-------------------------------------------------
kwMatrix kwMatrix::operator/( kwMatrix& Divisor )
{
    kwMatrix   result( this->rows, Divisor.rows );
    kwMatrix   inversed( Divisor.cols, Divisor.rows );
    inversed = !Divisor;    // size: Divisor.cols * Divisor.rows
    result = (*this) * inversed;
    return result;
}

//-------------------------------------------------
kwMatrix kwMatrix::operator/( float Divisor )
{
    kwMatrix result(1, 1, false);
    if ( Divisor != 0 )
    {
        result = (*this);
        for ( int r = 0; r < this->rows; ++r )
            for ( int c = 0; c < this->cols; ++c )
                result.elematrix[r][c] /= Divisor;
    }
    else {
        cout << "0 divisor, return a Null matrix" << endl;
        result.Clear();
    }
    return result;
}

//-------------------------------------------------
kwMatrix kwMatrix::operator%(kwMatrix &HardamardMul)
{
    if( this->rows != HardamardMul.rows || this->cols !=  HardamardMul.cols ){
        cout << "wrong size, can't do elem-wise mult, , return a Null matrix" << endl;
        kwMatrix result(1, 1, false);
        result.Clear();
        return  result;
    }
    kwMatrix result(*this);
    for(int r = 0; r < this->rows; ++r)
        for(int c = 0; c < this->cols; ++c)
            result.elematrix[r][c] *= HardamardMul.elematrix[r][c];
    return result;
}

//-------------------------------------------------
kwMatrix kwMatrix::operator!() const
{
    kwMatrix NotInverMatrix(1, 1, false);
    NotInverMatrix.Clear();
    if (this->rows != this->cols){
        cout << "not a square, don't have inverse matrix, return a Null matrix" << endl;
        return NotInverMatrix;
    }

    kwMatrix GaussMatrix(*this);
    kwMatrix Identity(this->rows, this->cols, false);
    Identity.SetIdentity();

    int RowIndex;
    float Type2Mult, Type3Mult;
    // compute reduced row echelon form
    for (int r = 0; r < GaussMatrix.rows; ++r) {
        RowIndex = r;
        while (GaussMatrix.elematrix[RowIndex][r] == 0) {
            // final row all=0, no pivot found
            if (RowIndex == GaussMatrix.rows - 1){
                cout << "there exist one row is zero, return a Null matrix" << endl;
                return NotInverMatrix;
            }
            RowIndex++;
            // switch row
            if (GaussMatrix.elematrix[RowIndex][r] != 0) {
                GaussMatrix.Type1_RowOperation(r, RowIndex);
                Identity.Type1_RowOperation(r, RowIndex);
                break;
            }
        }

        Type2Mult = 1 / GaussMatrix.elematrix[r][r];
        GaussMatrix.Type2_RowOPeration(r, Type2Mult);
        Identity.Type2_RowOPeration(r, Type2Mult);

        // eliminate pivot
        for (int i = r + 1; i < GaussMatrix.rows; ++i) {
            Type3Mult = -GaussMatrix.elematrix[i][r];
            GaussMatrix.Type3_RowOperation(r, Type3Mult, i);
            Identity.Type3_RowOperation(r, Type3Mult, i);
        }
    }
    if (GaussMatrix.elematrix[GaussMatrix.rows - 1][GaussMatrix.cols - 1] == 0){
        cout << "not all rows are linear indep, return a Null matrix" << endl;
        return NotInverMatrix;
    }
    // backward substistution
    Type2Mult = 1 / GaussMatrix.elematrix[GaussMatrix.rows - 1][GaussMatrix.cols - 1];
    GaussMatrix.Type2_RowOPeration(rows - 1, Type2Mult);
    Identity.Type2_RowOPeration(rows - 1, Type2Mult);

    for (int i = GaussMatrix.rows - 1; 0 <= i; --i) {
        for (int j = i - 1; 0 <= j; --j) {
          Type3Mult = -GaussMatrix.elematrix[j][i];
          GaussMatrix.Type3_RowOperation(i, Type3Mult, j);
          Identity.Type3_RowOperation(i, Type3Mult, j);
        }
    }

//    for (int i = 0 ; i < GaussMatrix.rows; ++i){
//        for (int j = 0; j < GaussMatrix.cols; ++j)
//            cout << Identity.elematrix[i][j] << ",";
//        cout << "======="<< endl;
//    }
//    cout << Identity.elematrix[0][0] << ' ' << Identity.elematrix[0][1] << ' ' << Identity.elematrix[0][2] << endl;
//    cout << Identity.elematrix[1][0] << ' ' << Identity.elematrix[1][1] << ' ' << Identity.elematrix[1][2] << endl;
//    cout << Identity.elematrix[2][0] << ' ' << Identity.elematrix[2][1] << ' ' << Identity.elematrix[2][2] << endl;

    return Identity;
}

//-------------------------------------------------
void kwMatrix::Type1_RowOperation(int row1, int row2)
{
    for (int c = 0; c < this->cols; ++c){
        float tmp = this->elematrix[row1][c];
        this->elematrix[row1][c] = this->elematrix[row2][c];
        this->elematrix[row2][c] = tmp;
    }

//    if we use following 作法, i.e. only change the pointer, not the memory content. Then once we use memcpy in operator=, it will cause the wrong assignment
//    float* ptr = this->elematrix[row1];
//    this->elematrix[row1] = this->elematrix[row2];
//    this->elematrix[row2] = ptr;
}

//-------------------------------------------------
void kwMatrix::Type2_RowOPeration(int row, float mult)
{
    for (int c = 0; c < this->cols; ++c)
        this->elematrix[row][c] *= mult;
}

//-------------------------------------------------
void kwMatrix::Type3_RowOperation(int row1, float mult, int row2)
{
    for (int c = 0; c < this->cols; ++c)
        this->elematrix[row2][c] += this->elematrix[row1][c]*mult;
}

//-------------------------------------------------
float kwMatrix::Determinant()
{
    if(this->rows != this->cols){
        cout << "not a square matrix, DNE determinant" << endl;
        return 0;
    }
    kwMatrix row_ech = Row_Echlon_Form();
    if (row_ech.IsNull()){
        cout << "this matrix has determinant = 0" << endl;
        return 0;
    }

    float det = 1.f;
    for (int i = 0; i < this->rows; ++i)
        det *= row_ech.elematrix[i][i];
    return det;
}

//-------------------------------------------------
kwMatrix kwMatrix::Row_Echlon_Form()
{
    kwMatrix row_echlon(*this);
    int RowIndex;
    float Type3Mult;
    for (int r = 0; r < row_echlon.rows; ++r) {
      RowIndex = r;
      while (row_echlon.elematrix[RowIndex][r] == 0) {
        if (RowIndex == this->rows - 1){    // final row all=0
            cout << "there exist one row is zero, return a Null matrix" << endl;
            row_echlon.Clear();
            return row_echlon;
        }
        RowIndex++;
        // switch row
        if (row_echlon.elematrix[RowIndex][r] != 0) {
          row_echlon.Type1_RowOperation(r, RowIndex);
          break;
        }
      }
      // eliminate pivot (outside while loop)
      for (int i = r + 1; i < rows; ++i) {
        Type3Mult = -row_echlon.elematrix[i][r]/row_echlon.elematrix[r][r];
        row_echlon.Type3_RowOperation(r, Type3Mult, i);
      }
    }

    if (row_echlon.elematrix[rows - 1][rows - 1] == 0){
        cout << "there exist one row is zero, return a Null matrix" << endl;
        row_echlon.Clear();
        return row_echlon;
    }

    return row_echlon;
}
