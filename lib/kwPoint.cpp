#include "kwPoint.h"

//-------------------------------------
void kwPoint::SetNull()
{
  x = kw_FMAX;
  y = kw_FMAX;
}

//-------------------------------------
float kwPoint::Norm2()
{
    return sqrtf(this->x*this->x + this->y*this->y);
}

//-------------------------------------
kwPoint& kwPoint::operator=( kwPoint srcPoint )
{
    this->x = srcPoint.x;
    this->y = srcPoint.y;
    return (*this);
}

//-------------------------------------
kwPoint &kwPoint::operator+=(kwPoint add) {
  this->x += add.x;
  this->y += add.y;
  return (*this);
}

//-------------------------------------
kwPoint &kwPoint::operator-=(kwPoint sub) {
  this->x -= sub.x;
  this->y -= sub.y;
  return (*this);
}

//-------------------------------------
bool kwPoint::operator==(kwPoint target) const{
  if (this->x == target.x && this->y == target.y)
    return true;
  else
    return false;
}

//-------------------------------------
bool kwPoint::operator!=(kwPoint target) const {
    return !((*this) == target);
}

//-------------------------------------
kwPoint kwPoint::operator+(kwPoint that) {
  kwPoint temp;
  temp.x = (this->x) + that.x;
  temp.y = (this->y) + that.y;
  return temp;
}

//-------------------------------------
kwPoint kwPoint::operator-(kwPoint that) {
  kwPoint temp;
  temp.x = (this->x) - that.x;
  temp.y = (this->y) - that.y;
  return temp;
}

//-------------------------------------
kwPoint kwPoint::operator*(float scalar) {
  kwPoint ans;
  ans.x = this->x * scalar;
  ans.y = this->y * scalar;
  return ans;
}

//-------------------------------------
kwPoint kwPoint::operator/(float scalar) const {
  kwPoint ans;
  ans.x = this->x / scalar;
  ans.y = this->y / scalar;
  return ans;
}

//-------------------------------------
float kwPoint::operator*(kwPoint others) {
    float ans;
    ans = (this->x * others.x) + (this->y * others.y);
    return ans;
}

//-------------------------------------
float kwPoint::operator%(kwPoint that) {
    float ans;
    ans = ((this->x * that.y) - (this->y * that.x));
    return ans;
}

//-------------------------------------------------------------------------------------------------
float kwPoint::operator||(kwPoint that) {
    float ans;
    ans = ((this->x * that.y) - (this->y * that.x));
    return ans;
}

//-------------------------------------
kwPoint kwPoint::MidPoint(kwPoint &operand) const{
  kwPoint temp(0, 0);
  temp.x = (this->x + operand.x) / 2;
  temp.y = (this->y + operand.y) / 2;
  return temp;
}

//-------------------------------------
float kwPoint::DistanceTo(kwPoint ptPoint) {
  float ans;
  ans = (this->x - ptPoint.x) * (this->x - ptPoint.x) + (this->y - ptPoint.y) * (this->y - ptPoint.y);
  ans = sqrt(ans);
  return ans;
}

//-------------------------------------
void kwPoint::Swap(kwPoint &srcPoint) {
  kwPoint tmp;
  tmp = (*this);
  (*this) = srcPoint;
  srcPoint = tmp;
}

//-------------------------------------
kwPoint kwPoint::Project(kwPoint &that) {
    float scalar;
    kwPoint ans;
    float norm2 = that.Norm2();
    if (norm2 == 0)
        return kwPoint(0, 0);
    scalar = (*this * that) / (norm2 * norm2);
    ans = (that * scalar);
    return ans;
}


//===========================================================================
kwPointInt kwPointInt::Set( int vx, int vy ){
    return kwPointInt(vx, vy);
}

//-------------------------------------
float kwPointInt::Norm2() const{
    return sqrtf(this->x*this->x + this->y*this->y);
}

//-------------------------------------
void kwPointInt::Swap( kwPointInt& srcPoint ){
    kwPointInt tmp(*this);
    (*this) = srcPoint;
    srcPoint = tmp;
}

//-------------------------------------
kwPointInt& kwPointInt::operator=( int ival )
{
    this->x = this->y = ival;
    return (*this);
}

//-------------------------------------
kwPointInt& kwPointInt::operator=( kwPointInt srcPoint )
{
    this->x = srcPoint.x;
    this->y = srcPoint.y;
    return (*this);
}


//===========================================================================
kwPointPair::kwPointPair() {
  p1.x = kw_FMAX;
  p1.y = kw_FMAX;
  p2.x = kw_FMAX;
  p2.y = kw_FMAX;
}

//-------------------------------------
kwPointPair::kwPointPair(kwPoint _p1, kwPoint _p2) {
  p1 = _p1;
  p2 = _p2;
}

//-------------------------------------
kwPointPair::kwPointPair(float _p1x, float _p1y, float _p2x, float _p2y) {
  p1.x = _p1x;
  p1.y = _p1y;
  p2.x = _p2x;
  p2.y = _p2y;
}

//-------------------------------------
bool kwPointPair::IsNull() { return (p1.IsNULL() || p2.IsNULL()); }

//-------------------------------------
void kwPointPair::SetNull() {
  p1.SetNull();
  p2.SetNull();
}

//-------------------------------------
void kwPointPair::Clear() {
  p1.Clear();
  p2.Clear();
}

//-------------------------------------
bool kwPointPair::IsZero() {
  if (p1.IsZero() && p2.IsZero())
    return true;
  else
    return false;
}

//-------------------------------------
kwPointPair kwPointPair::operator()(kwPoint _p1, kwPoint _p2) {
  p1 = _p1;
  p2 = _p2;
  return (*this);
}

//-------------------------------------
kwPointPair kwPointPair::operator()(float _p1x, float _p1y, float _p2x, float _p2y) {
  p1.x = _p1x;
  p1.y = _p1y;
  p2.x = _p2x;
  p2.y = _p2y;
  return (*this);
}

//-------------------------------------
kwPointPair &kwPointPair::operator=(kwPointPair &srcPointPair) {
  p1 = srcPointPair.p1;
  p2 = srcPointPair.p2;
  return (*this);
}

//-------------------------------------
kwPointPair &kwPointPair::operator=(kwPoint srcPoint) {
  p1.x = 0;
  p1.y = 0;
  p2 = srcPoint;
  return (*this);
}

//-------------------------------------
bool kwPointPair::operator==(kwPointPair operand2) {
  if (p1 == operand2.p1 && p2 == operand2.p2) {
    return true;
  }
  else {
    return false;
  }
}

//-------------------------------------
bool kwPointPair::operator!=(kwPointPair operand2) {
  if (*this == operand2)
    return false;
  else
    return true;
}

//-------------------------------------
kwPointPair kwPointPair::operator+(kwPoint TranslateVector) {
  kwPointPair temp;
  temp.p1 = (this->p1) + TranslateVector;
  temp.p2 = (this->p2) + TranslateVector;
  return temp;
}

//-------------------------------------
kwPointPair kwPointPair::operator-(kwPoint TranslateVector) {
  kwPointPair temp;
  temp.p1 = (this->p1) - TranslateVector;
  temp.p2 = (this->p2) - TranslateVector;
  return temp;
}

//-------------------------------------
kwPointPair kwPointPair::operator*(float scale) {
  kwPointPair temp;
  temp.p1 = this->p1 * scale;
  temp.p2 = this->p2 * scale;
  return temp;
}

//-------------------------------------
float kwPointPair::operator*(kwPointPair oprand2) {
  float ans;
  kwPoint vector1, vector2;
  vector1 = oprand2.p1 - p1;
  vector2 = oprand2.p2 - p2;
  ans = vector1 * vector2;
  return ans;
}

//-------------------------------------
float kwPointPair::operator||(kwPointPair operand2) {
  kwPoint vectorA, vectorB;
  float ans;
  vectorA = this->MakeVector();
  vectorB = operand2.MakeVector();
  ans = ( vectorA || vectorB );
  return ans;
}

//-------------------------------------
kwPoint kwPointPair::operator~() {
  kwPoint vector;
  vector = this->MakeVector();
  float norm2 = vector.Norm2();
  if (norm2 == 0)
    return kwPoint(0, 0);
  vector = vector / (norm2);
  return vector;
}

//-------------------------------------
float kwPointPair::operator^(kwPointPair operand2) {
    kwPoint vectorA, vectorB;
    float ans;
    vectorA = this->MakeVector();
    vectorB = operand2.MakeVector();
    float denom = vectorA.Norm2() * vectorB.Norm2();
    if (denom == 0)
        return 0;
    ans = (vectorA * vectorB) / denom;
    return ans;
}

//-------------------------------------
kwPointPair kwPointPair::operator-() const {
    kwPointPair ans;
    ans.p1 = this->p2;
    ans.p2 = this->p1;
    return ans;
}

//-------------------------------------
float kwPointPair::EvaluateAtX(float Xvalue) {
    if ((Xvalue - p1.x) * (Xvalue - p2.x) > 0)
        return kw_FMAX;
    else {
        float ans, m, n;
        m = Xvalue - p1.x;
        n = p2.x - Xvalue;
        ans = (n * p1.y + m * p2.y) / (m + n);
        return ans;
    }
}

//-------------------------------------
float kwPointPair::EvaluateAtY(float Yvalue) {
    if ((Yvalue - p1.y) * (Yvalue - p2.y) > 0)
        return kw_FMAX;
    else {
        float ans, m, n;
        m = Yvalue - p1.y;
        n = p2.y - Yvalue;
        ans = (n * p1.x + m * p2.x) / (m + n);
        return ans;
    }
}


//-------------------------------------
kwPoint kwPointPair::MakeVector() {
    kwPoint Vector;
    Vector = p2 - p1;
    return Vector;
}

//-------------------------------------
float kwPointPair::Length() {
    float ans;
    ans = p2.DistanceTo(p1);
    return ans;
}

//-------------------------------------
void kwPointPair::swap(kwPointPair &srcPointPair) {
    p1.Swap(srcPointPair.p1);
    p2.Swap(srcPointPair.p2);
}

//-------------------------------------
kwPoint kwPointPair::MidPoint() {
    kwPoint Mid;
    Mid.x = (p1.x + p2.x) * 0.5;
    Mid.y = (p1.y + p2.y) * 0.5;
    return Mid;
}


//===========================================================================
kwPointPairNode::kwPointPairNode() {
    p1(kw_FMAX, kw_FMAX);
    p2(kw_FMAX, kw_FMAX);
    Previous = nullptr;
    Next = nullptr;
    Rank = kw_FMAX;
}

//-------------------------------------
kwPointPairNode::kwPointPairNode(float x1, float y1, float x2, float y2, float _rank) {
    p1(x1, y1);
    p2(x2, y2);
    Rank = _rank;
}

//-------------------------------------
kwPointPairNode &kwPointPairNode::operator=(kwPointPairNode &that) {
    p1 = that.p1;
    p2 = that.p2;
    Previous = that.Previous;
    Next = that.Next;
    Rank = that.Rank;
    return (*this);
}
