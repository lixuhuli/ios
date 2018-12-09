
#ifndef __ui_tween_h__
#define __ui_tween_h__

#pragma once

class CTween
{
public:
    enum Type
    {
        LINEAR = 0,        // Linear.
        EASE_OUT,      // Fast in, slow out (default).
        EASE_IN,       // Slow in, fast out.
        EASE_IN_OUT,   // Slow in and out, fast in the middle.
        FAST_IN_OUT,   // Fast in and out, slow in the middle.
        EASE_OUT_SNAP, // Fast in, slow out, snap to final value.
        ZERO,          // Returns a value of 0 always.
		EASE_IN_QUART,
		EASE_OUT_QUART,
		EASE_IN_OUT_QUART,
		EASE_IN_EXPO,
		EASE_OUT_EXPO,
		EASE_IN_OUT_EXPO,
		EASE_OUT_BOUNCE,
		EASE_IN_BACK,
		EASE_OUT_BACK,
		EASE_IN_OUT_BACK,
		ELASTIC,
		SWING_FROM_TO,
		SWING_FROM,
		SWING_TO,
		BOUNCE,
		BOUNCE_PAST,
		EASE_FROM_TO,
		EASE_FROM,
		EASE_TO,
		MIRROR,
		FLICKER,
		WOBBLE,
		PULSE,
		BLINK,
		SPRING
    };

    static double CalculateValue(Type type, double state);
//     static double ValueBetween(double value, double start, double target);
//     static int    ValueBetween(double value, int start, int target);
// 	static POINT  ValueBetween(double value, POINT ptBegin, POINT ptEnd);

private:
    CTween();
    ~CTween();
};

#endif //__ui_base_tween_h__