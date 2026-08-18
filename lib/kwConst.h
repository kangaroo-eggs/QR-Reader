#ifndef KWCONST_H
#define KWCONST_H

/////////////////////////////////////////////////////////////////////////////////
/// \brief define Often used constant
///
typedef unsigned char kwPixel;
const float         kw_Pi               = 3.14259265;
const float         kw_NaturalBase      = 2.71828183;

const float         kw_FMAX             = 3.40282347E+38F;
const float         kw_FMIN_POSITIVE    = 1.17549435E-38F;
// 一般還是用 32 bits 的 int, even OS is 64 bits
const int           kw_INT32_MAX        = 2147483647;
const int           kw_INT32_MIN        = -2147483647;
const unsigned int  kw_UIN32_MAX        = 4294967295;
const unsigned int  kw_UIN32_MIN        = 0;

#endif // KWCONST_H
