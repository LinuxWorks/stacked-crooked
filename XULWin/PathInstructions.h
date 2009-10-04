#ifndef PATHINSTRUCTIONS_H_INCLUDED
#define PATHINSTRUCTIONS_H_INCLUDED


#include "Points.h"
#include <utility>


namespace XULWin
{

    class PathInstruction
    {
    public:
        enum Type
        {
            MoveTo,                         // M
            LineTo,                         // L
            HorizontalLineTo,               // H
            VerticalLineTo,                 // V
            CurveTo,                        // C
            SmoothCurveTo,                  // S
            QuadraticBelzierCurve,          // Q
            SmoothQuadraticBelzierCurveTo,  // T
            EllipticalArc,                  // A
            ClosePath                       // Z
        };

        enum Positioning
        {
            Relative,
            Absolute
        };
        
        PathInstruction(Type inType, Positioning inPositioning, const Points & inPoints);

        Type type() const;

        size_t numPoints() const;

        const Point & getPoint(size_t inIdx) const;

        Positioning positioning() const;

    private:
        Type mType;
        Points mPoints;
        Positioning mPositioning;
    };

    typedef std::vector<PathInstruction> PathInstructions;

} // namespace XULWin


#endif // PATHINSTRUCTIONS_H_INCLUDED
