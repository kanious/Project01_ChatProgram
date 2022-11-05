#ifndef _STATICFUNC_H_
#define _STATICFUNC_H_

template <typename T>
void SafeDelete(T& pointer)
{
	if (nullptr != pointer)
	{
		delete pointer;
		pointer = nullptr;
	}
}

#endif //_STATICFUNC_H_