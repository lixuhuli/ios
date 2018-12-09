#include "StdAfx.h"
#include "tween.h"

#include <float.h>
#define _USE_MATH_DEFINES
#include <math.h>


double sinusoidal(double t)
{
	return  (-cos(t*M_PI)/2) + 0.5;
}
// static
double CTween::CalculateValue(CTween::Type type, double state)
{
    switch(type)
    {
    case EASE_IN:
        return pow(state, 2);

    case EASE_IN_OUT:
        if(state < 0.5)
        {
            return pow(state * 2, 2) / 2.0;
        }
        return 1.0 - (pow((state - 1.0) * 2, 2) / 2.0);

    case FAST_IN_OUT:
        return (pow(state - 0.5, 3) + 0.125) / 0.25;

    case LINEAR:
        return state;

    case EASE_OUT_SNAP:
        state = 0.95 * (1.0 - pow(1.0 - state, 2)) + 0.05;
        break;

    case EASE_OUT:
        return 1.0 - pow(1.0 - state, 2);

    case ZERO:
        return 0;
	case EASE_IN_QUART:
		return pow(state, 4);
	case EASE_OUT_QUART:
		return -(pow((state - 1), 4) - 1);
	case EASE_IN_OUT_QUART:
		{
			if((state/=0.5) < 1) return 0.5 * pow(state, 4);
			return -0.5 * ((state -= 2)*pow(state, 3) - 2);
		}
	case EASE_IN_EXPO:
		return (state == 0) ? 0 : pow(2, 10 * (state-1));
	case EASE_OUT_EXPO:
		return (state == 1) ? 1 : -pow(2, -10 * state) + 1;
	case EASE_IN_OUT_EXPO:
		{
			if(state == 0) return 0;
			if(state == 1) return 1;
			if((state/=0.5) < 1) return 0.5 * pow(2, 10*(state-1));
			return 0.5 * (-pow(2, -10 * (--state)) + 2);
		}
	case EASE_OUT_BOUNCE:
		{
			if(state < (1/2.75)) return (7.5625*state*state);
			else if(state < (2/2.75)) return (7.5625 * (state-=(1.5/2.75))*state + .75);
			else if (state < (2.5/2.75))
				return (7.5625*(state-=(2.25/2.75))*state + .9375);
			else
				return (7.5625*(state-=(2.625/2.75))*state + .984375);
		}
	case EASE_IN_BACK:
		{
			double s = 1.70158;
			return (state)*state*((s+1)*state - s);
		}
	case EASE_OUT_BACK:
		{
			double s = 1.70158;
			return (state=state-1)*state*((s+1)*state + s) + 1;
		}
	case EASE_IN_OUT_BACK:
		{
			double s = 1.70158;
			if((state/=0.5) < 1) {
				return 0.5*(state*state*(((s*=(1.525))+1)*state -s));
			}
			return (0.5*((state-=2)*state*(((s*=(1.525))+1)*state +s) +2));
		}
	case ELASTIC:
		return -1 * pow(4,-8*state) * sin((state*6-1)*(2*M_PI)/2) + 1;
	case SWING_FROM_TO:
		{
			double s = 1.70158;
			return ((state/=0.5) < 1) ? 0.5*(state*state*(((s*=(1.525))+1)*state - s)) : 0.5*((state-=2)*state*(((s*=(1.525))+1)*state + s) + 2);
		}
	case SWING_FROM:
		{
			double s = 1.70158;
			return state*state*((s+1)*state - s);
		}
	case SWING_TO:
		{
			double s = 1.70158;
			return (state-=1)*state*((s+1)*state + s) + 1;
		}
	case BOUNCE:
		{
			if (state < (1/2.75)) {
				return (7.5625*state*state);
			} else if (state < (2/2.75)) {
				return (7.5625*(state-=(1.5/2.75))*state + .75);
			} else if (state < (2.5/2.75)) {
				return (7.5625*(state-=(2.25/2.75))*state + .9375);
			} else {
				return (7.5625*(state-=(2.625/2.75))*state + .984375);
			}
		}
	case BOUNCE_PAST:
		{
			if (state < (1/2.75)) {
				return (7.5625*state*state);
			} else if (state < (2/2.75)) {
				return 2 - (7.5625*(state-=(1.5/2.75))*state + .75);
			} else if (state < (2.5/2.75)) {
				return 2 - (7.5625*(state-=(2.25/2.75))*state + .9375);
			} else {
				return 2 - (7.5625*(state-=(2.625/2.75))*state + .984375);
			}
		}
	case EASE_FROM_TO:
		{
			if ((state/=0.5) < 1) return 0.5*pow(state,4);
			return -0.5 * ((state-=2)*pow(state,3) - 2);
		}
	case EASE_FROM:
		{
			return pow(state,4);
		}
	case EASE_TO:
		{
			return pow(state,0.25);
		}
	case MIRROR:
		{
			if(state<0.5)
				return sinusoidal(state*2);
			else
				return sinusoidal(1-(state-0.5)*2);
		}
	case FLICKER:
		{
			srand(GetTickCount());
			state = state + (rand()-0.5)/5;
			return sinusoidal(state < 0 ? 0 : state > 1 ? 1 : state);
		}
	case WOBBLE:
		{
			return (-cos(state*M_PI*(9*state))/2) + 0.5;
		}
	case PULSE:
		{
			return (-cos((state*5-.5)*2)*M_PI)/2 + .5;
		}
	case BLINK:
		{
			return ((int)(state*5)) % 2;
		}
	case SPRING:
		{
			return 1 - (cos(state * 4.5 * M_PI) * exp(-state * 6));
		}
    }

    //NOTREACHED();
    return state;
}

//static
// double CTween::ValueBetween(double value, double start, double target)
// {
//     return start + (target - start) * value;
// }
// 
// //static
// int CTween::ValueBetween(double value, int start, int target)
// {
//     if(start == target)
//     {
//         return start;
//     }
//     double delta = static_cast<double>(target - start);
//     if(delta < 0)
//     {
//         delta--;
//     }
//     else
//     {
//         delta++;
//     }
//     return start + static_cast<int>(value * _nextafter(delta, 0));
// }
// 
// POINT CTween::ValueBetween(double value, POINT ptBegin, POINT ptEnd)
// {
// 	//计算三阶贝塞尔曲线
// 
// 	POINT ptCtrl1 = {0};
// 	POINT ptCtrl2 = {0};
// 
// 	ptCtrl1.x = ptBegin.x + (ptBegin.x - 50)/2;
// 	ptCtrl1.y = ptBegin.y + (ptBegin.y - 50)/2;
// 
// 	ptCtrl2.x = ptCtrl1.x + 100;
// 	ptCtrl2.y = ptCtrl1.y + 100;
// 
// 	float ax, bx, cx;
// 	float ay, by, cy;
// 	ax = bx = cx = 0;
// 	ay = by = cy = 0;
// 
// 	float tSquared = 0;
// 	float tCubed = 0;
// 	POINT   result = {0};
// 
// 	/*計算多項式係數*/
// 
// 	cx = 3.0 * (ptCtrl1.x - ptBegin.x);
// 	bx = 3.0 * (ptCtrl2.x - ptCtrl1.x) - cx;
// 	ax = ptEnd.x - ptBegin.x - cx - bx;
// 
// 	cy = 3.0 * (ptCtrl1.y - ptBegin.y);
// 	by = 3.0 * (ptCtrl2.y - ptCtrl1.y) - cy;
// 	ay = ptEnd.y - ptBegin.y - cy - by;
// 
// 	/*計算位於參數值t的曲線點*/
// 
// 	tSquared = value * value;
// 	tCubed = tSquared * value;
// 
// 	result.x = (ax * tCubed) + (bx * tSquared) + (cx * value) + ptBegin.x;
// 	result.y = (ay * tCubed) + (by * tSquared) + (cy * value) + ptBegin.y;
// 
// 	return result;
// }