#include "RGBColor.h"


namespace XULWin {


RGBColor::RGBColor() :
    mAlpha(0),
    mRed(0),
    mGreen(0),
    mBlue(0)
{
}


RGBColor::RGBColor(int inRed, int inGreen, int inBlue) :
    mAlpha(255),
    mRed(inRed),
    mGreen(inGreen),
    mBlue(inBlue)

{
}


RGBColor::RGBColor(int inAlpha, int inRed, int inGreen, int inBlue) :
    mAlpha(inAlpha),
    mRed(inRed),
    mGreen(inGreen),
    mBlue(inBlue)
{
}


int RGBColor::red() const
{
    return mRed;
}


int RGBColor::green() const
{
    return mGreen;
}


int RGBColor::blue() const
{
    return mBlue;
}


int RGBColor::alpha() const
{
    return mAlpha;
}


} // namespace XULWin

