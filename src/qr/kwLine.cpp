#include "kwLine.h"
#include <math.h>

//-------------------------------------
kwLine::kwLine() {
  theta = kw_FMAX;
  radius = kw_FMAX;
  slope = kw_FMAX;
  intercept = kw_FMAX;
}
//-------------------------------------
kwLine::kwLine(float _T, float _R) {
  theta = _T;
  radius = _R;
}

//-------------------------------------
bool kwLine::IsVertical()
{
    if (slope > 500 || slope < -500)
        return true;
    else
        return false;
}

//-------------------------------------
void kwLine::MB2TR()
{
  radius = (intercept / sqrt((1 * 1 + slope * slope)));

  if (slope == 0) {
    theta = kw_Pi / 2;
  } else {
    theta = atan(-1 / slope);
  }
  if (radius < 0) {
    radius = -radius;
    theta = -theta;
  }
}

//-------------------------------------
void kwLine::TR2MB()
{
  float sinT = sin(theta);
  if (sinT == 0) {
    slope = kw_FMAX;
    intercept = kw_FMAX;
  } else {
    slope = -1 / tan(theta);
    intercept = radius / sinT;
  }
}

//-------------------------------------
void kwLine::Set(kwPoint p1, kwPoint p2)
{
    if (p2.x == p1.x) {
        slope = kw_FMAX;
        intercept = kw_FMAX;
        theta = (p1.x > 0) ? 0 : kw_Pi;
        radius = p1.x;
    } else {
        slope = (p2.y - p1.y) / (p2.x - p1.x);
        intercept = (p2.y - p1.y) / (p2.x - p1.x) * (-p1.x) + p1.y;
        this->MB2TR();
    }
}

//-------------------------------------
kwPoint kwLine::LineIntersection(kwLine Line)
{
    float a1, b1, c1;
    float a2, b2, c2;
    kwPoint ans;
    a1 = this->slope;
    a2 = Line.slope;
    b1 = -1;
    b2 = -1;
    c1 = -intercept;
    c2 = -Line.intercept;
    if (a1 < 500 && a1 > -500 && a2 < 500 && a2 > -500) {
        float delta, deltaX, deltaY;
        delta = a1 * b2 - b1 * a2;
        if (delta == 0) {
          return ans;
        }
        deltaX = c1 * b2 - b1 * c2;
        deltaY = a1 * c2 - c1 * a2;
        ans.x = deltaX / delta;
        ans.y = deltaY / delta;
        return ans;
    } else if (this->IsVertical() && !Line.IsVertical()) {
        ans.x = this->radius;
        ans.y = Line.EvaluatAtX(ans.x);
        return ans;
    } else if (!this->IsVertical() && Line.IsVertical()) {
        ans.x = Line.radius;
        ans.y = Line.EvaluatAtX(ans.x);
        return ans;
    } else {
        return ans;
    }
}

//-------------------------------------
float kwLine::EvaluatAtX(float Xvalue) {
    if (slope == kw_FMAX)
      return kw_FMAX;
    else {
      float ans = slope * Xvalue + intercept;
      return ans;
    }
}
