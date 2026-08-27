#include "kwReaderQR_Eye.h"

//-------------------------------------
kwReaderQR_Eye::kwReaderQR_Eye()
{
    this->Clear();
}

//-------------------------------------
kwReaderQR_Eye::kwReaderQR_Eye(kwReaderQR_Eye &copyfrom)
{
    // we don't need to copy the value one by one, since there is no variable is pointer
    // (if there is some variable is pointer, then when we use distructor to free the membase, we might free that space twice)
    memcpy( this, &copyfrom, sizeof(kwReaderQR_Eye));
}

//-------------------------------------
kwReaderQR_Eye::~kwReaderQR_Eye() {}

//-------------------------------------
void kwReaderQR_Eye::Clear()
{
    this->ptCenter.x = this->ptCenter.y = this->eyeWidth_H = this->eyeWidth_V = ranking = kw_FMAX;
    this->beReversed = false;
}

//-------------------------------------
bool kwReaderQR_Eye::IsNull() {
    if ( this->ptCenter.IsNULL() || eyeWidth_H == kw_FMAX || eyeWidth_V == kw_FMAX)
        return true;
    else
        return false;
}

//-------------------------------------
kwReaderQR_Eye& kwReaderQR_Eye::operator=( const kwReaderQR_Eye &fiFrom )
{
    if ( this != &fiFrom ){
        memcpy( this, &fiFrom, sizeof(kwReaderQR_Eye) );
    }
    return (*this);
}



//===========================================================================
kwReaderQR_EyeSet::kwReaderQR_EyeSet()
{
    Eye0.Clear();
    Eye1.Clear();
    Eye2.Clear();
    Rank = kw_FMAX;
}

//-------------------------------------
kwReaderQR_EyeSet::kwReaderQR_EyeSet( kwReaderQR_EyeSet &copyfrom )
{
    memcpy( this, &copyfrom, sizeof(kwReaderQR_EyeSet));
}

//-------------------------------------
kwReaderQR_EyeSet::~kwReaderQR_EyeSet() {}

//-------------------------------------
void kwReaderQR_EyeSet::SetEye(kwReaderQR_Eye _Eye0, kwReaderQR_Eye _Eye1,
                                     kwReaderQR_Eye _Eye2) {
  Eye0 = _Eye0;
  Eye1 = _Eye1;
  Eye2 = _Eye2;
}

//-------------------------------------
void kwReaderQR_EyeSet::operator()(kwReaderQR_Eye _Eye0, kwReaderQR_Eye _Eye1,
                                      kwReaderQR_Eye _Eye2) {
  Eye0 = _Eye0;
  Eye1 = _Eye1;
  Eye2 = _Eye2;
}

//-------------------------------------
void kwReaderQR_EyeSet::kwReaderQR_EyeSet::Clear() {
  Eye0.Clear();
  Eye1.Clear();
  Eye2.Clear();
  Rank = kw_FMAX;
}

//-------------------------------------
bool kwReaderQR_EyeSet::kwReaderQR_EyeSet::IsNull() {
  if (Eye0.IsNull() || Eye1.IsNull() || Eye2.IsNull())
    return true;
  else
    return false;
}
