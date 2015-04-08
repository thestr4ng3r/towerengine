
#ifndef _COMPARABLE_H
#define _COMPARABLE_H


template <class T> class tComparable
{
	private:
		T value;

	public:
		T GetCompareValue(void)		{ return value; }
		void SetCompareValue(T v)	{ value = v; }
};

#endif
