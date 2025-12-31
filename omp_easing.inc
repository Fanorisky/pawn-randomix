#if defined _omp_easing_functions_included
	#endinput
#endif
#define _omp_easing_functions_included

#include <open.mp>

#define EASE_IN_SINE 0
#define EASE_OUT_SINE 1
#define EASE_IN_OUT_SINE 2
#define EASE_IN_QUAD 3
#define EASE_OUT_QUAD 4
#define EASE_IN_OUT_QUAD 5
#define EASE_IN_CUBIC 6
#define EASE_OUT_CUBIC 7
#define EASE_IN_OUT_CUBIC 8
#define EASE_IN_QUART 9
#define EASE_OUT_QUART 10
#define EASE_IN_OUT_QUART 11
#define EASE_IN_QUINT 12
#define EASE_OUT_QUINT 13
#define EASE_IN_OUT_QUINT 14
#define EASE_IN_EXPO 15
#define EASE_OUT_EXPO 16
#define EASE_IN_OUT_EXPO 17
#define EASE_IN_CIRC 18
#define EASE_OUT_CIRC 19
#define EASE_IN_OUT_CIRC 20
#define EASE_IN_BACK 21
#define EASE_OUT_BACK 22
#define EASE_IN_OUT_BACK 23
#define EASE_IN_ELASTIC 24
#define EASE_OUT_ELASTIC 25
#define EASE_IN_OUT_ELASTIC 26
#define EASE_IN_BOUNCE 27
#define EASE_OUT_BOUNCE 28
#define EASE_IN_OUT_BOUNCE 29
#define EASE_NONE 30

enum eAnimatorTypes 
{
	ANIMATOR_POSITION,
	ANIMATOR_LETTER_SIZE,
	ANIMATOR_TEXT_SIZE,
	ANIMATOR_FULL_SIZE,
	ANIMATOR_COLOR,
	ANIMATOR_BOX_COLOR,
	ANIMATOR_BACKGROUND_COLOR
}

forward OnAnimatorFinish(playerid, animatorid, textdrawid, type);

// Easing utility functions
native Float:GetEasingValue(Float:t, easeType);
native Float:Lerp(Float:start, Float:end, Float:t);
native LerpColor(color1, color2, Float:t);

// PlayerTextDraw animation functions
native PlayerText_MoveTo(playerid, PlayerText:textdraw, Float:x, Float:y, duration, easeType, bool:silent = false);
native PlayerText_MoveToX(playerid, PlayerText:textdraw, Float:x, duration, easeType, bool:silent = false);
native PlayerText_MoveToY(playerid, PlayerText:textdraw, Float:y, duration, easeType, bool:silent = false);
native PlayerText_MoveLetterSize(playerid, PlayerText:textdraw, Float:y, duration, easeType, bool:silent = false);
native PlayerText_MoveTextSizeX(playerid, PlayerText:textdraw, Float:x, duration, easeType, bool:silent = false);
native PlayerText_MoveTextSizeY(playerid, PlayerText:textdraw, Float:y, duration, easeType, bool:silent = false);
native PlayerText_MoveSize(playerid, PlayerText:textdraw, Float:x, Float:y, duration, easeType, bool:silent = false);
native PlayerText_InterpolateColor(playerid, PlayerText:textdraw, color, duration, easeType, bool:silent = false);
native PlayerText_InterpolateBoxColor(playerid, PlayerText:textdraw, color, duration, easeType, bool:silent = false);
native PlayerText_InterpolateBGColor(playerid, PlayerText:textdraw, color, duration, easeType, bool:silent = false);
native PlayerText_PlaceOnTop(playerid, PlayerText:textdraw);
native PlayerText_StopAnimation(animator_id);
native IsAnimationActive(animator_id);
native GetActiveAnimationsCount();
native GetAnimationStats(&totalCreated, &peakConcurrent, &totalCallbacks);

// Alias for British English spelling
#define PlayerText_InterpolateColour PlayerText_InterpolateColor
#define PlayerText_InterpolateBoxColour PlayerText_InterpolateBoxColor
#define PlayerText_InterpolateBGColour PlayerText_InterpolateBGColor

// These functions are provided for backward compatibility
stock Float:easeInSine(Float:t) return GetEasingValue(t, EASE_IN_SINE);
stock Float:easeOutSine(Float:t) return GetEasingValue(t, EASE_OUT_SINE);
stock Float:easeInOutSine(Float:t) return GetEasingValue(t, EASE_IN_OUT_SINE);
stock Float:easeInQuad(Float:t) return GetEasingValue(t, EASE_IN_QUAD);
stock Float:easeOutQuad(Float:t) return GetEasingValue(t, EASE_OUT_QUAD);
stock Float:easeInOutQuad(Float:t) return GetEasingValue(t, EASE_IN_OUT_QUAD);
stock Float:easeInCubic(Float:t) return GetEasingValue(t, EASE_IN_CUBIC);
stock Float:easeOutCubic(Float:t) return GetEasingValue(t, EASE_OUT_CUBIC);
stock Float:easeInOutCubic(Float:t) return GetEasingValue(t, EASE_IN_OUT_CUBIC);
stock Float:easeInQuart(Float:t) return GetEasingValue(t, EASE_IN_QUART);
stock Float:easeOutQuart(Float:t) return GetEasingValue(t, EASE_OUT_QUART);
stock Float:easeInOutQuart(Float:t) return GetEasingValue(t, EASE_IN_OUT_QUART);
stock Float:easeInQuint(Float:t) return GetEasingValue(t, EASE_IN_QUINT);
stock Float:easeOutQuint(Float:t) return GetEasingValue(t, EASE_OUT_QUINT);
stock Float:easeInOutQuint(Float:t) return GetEasingValue(t, EASE_IN_OUT_QUINT);
stock Float:easeInExpo(Float:t) return GetEasingValue(t, EASE_IN_EXPO);
stock Float:easeOutExpo(Float:t) return GetEasingValue(t, EASE_OUT_EXPO);
stock Float:easeInOutExpo(Float:t) return GetEasingValue(t, EASE_IN_OUT_EXPO);
stock Float:easeInCirc(Float:t) return GetEasingValue(t, EASE_IN_CIRC);
stock Float:easeOutCirc(Float:t) return GetEasingValue(t, EASE_OUT_CIRC);
stock Float:easeInOutCirc(Float:t) return GetEasingValue(t, EASE_IN_OUT_CIRC);
stock Float:easeInBack(Float:t) return GetEasingValue(t, EASE_IN_BACK);
stock Float:easeOutBack(Float:t) return GetEasingValue(t, EASE_OUT_BACK);
stock Float:easeInOutBack(Float:t) return GetEasingValue(t, EASE_IN_OUT_BACK);
stock Float:easeInElastic(Float:t) return GetEasingValue(t, EASE_IN_ELASTIC);
stock Float:easeOutElastic(Float:t) return GetEasingValue(t, EASE_OUT_ELASTIC);
stock Float:easeInOutElastic(Float:t) return GetEasingValue(t, EASE_IN_OUT_ELASTIC);
stock Float:easeInBounce(Float:t) return GetEasingValue(t, EASE_IN_BOUNCE);
stock Float:easeOutBounce(Float:t) return GetEasingValue(t, EASE_OUT_BOUNCE);
stock Float:easeInOutBounce(Float:t) return GetEasingValue(t, EASE_IN_OUT_BOUNCE);
stock Float:easeNone(Float:t) return GetEasingValue(t, EASE_NONE);

// helper function to get color from RGBA
stock GetColorFromRGBA(red, green, blue, alpha = 255)
{
    return ((red & 0xFF) << 24) | ((green & 0xFF) << 16) | ((blue & 0xFF) << 8) | (alpha & 0xFF);
}

stock GetRGBAFromColor(color, &red, &green, &blue, &alpha)
{
    red = (color >> 24) & 0xFF; green = (color >> 16) & 0xFF; blue  = (color >> 8)  & 0xFF; alpha = color & 0xFF;
}
