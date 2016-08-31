#ifndef FLOATUTIL_H
#define FLOATUTIL_H

//a simple negative test for floats this MIGHT be platform specific
//If the platform used the IEEE standard for floats then it should work but
//only the computer gods can tell for certain.
namespace uLPrivate{union floatTester{int i; float f;};}
inline bool isNegFloat(float test)
{
	uLPrivate::floatTester ft;
	ft.f=test;
	return ((ft.i)<0);
}

inline bool isNegZero(float test)
{
	uLPrivate::floatTester ft;
	ft.f=test;
	return (((ft.i&0x7fffffff)==0)&&isNegFloat(test));
}

inline static float negZero()
{
	float out=0;
	return (out=-out);
}

#endif // FLOATUTIL_H
