#include <common/fixed.h>
#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable:4996)

void fixed_lua_number2str(char *result, int number)
{
	fixed f(true, number);
	sprintf(result, "%s", f.asString());
}

int fixed_lua_str2number(const char *s, char **endptr)
{
	strtod(s, endptr); // Increment endptr
	fixed f(s);
	return (int) f.getInternalData();
}

int fixed_lua_number2int(int n)
{
	fixed f(true, n);
	return f.asInt();
}

int fixed_luai_nummul(int a, int b)
{
	fixed fa(true, a);
	fixed fb(true, b);
	fixed result = fa * fb;
	return (int) result.getInternalData();
}

int fixed_luai_numdiv(int a, int b)
{
	fixed fa(true, a);
	fixed fb(true, b);
	fixed result = fa / fb;
	return (int) result.getInternalData();
}

int fixed_luai_nummod(int a, int b)
{
	fixed fa(true, a);
	fixed fb(true, b);
	fixed result = fa % fb;
	return (int) result.getInternalData();
}

int fixed_luai_numpow(int a, int b)
{
	fixed fa(true, a);
	fixed fb(true, b);
	fixed result = fa.pow(fb);
	return (int) result.getInternalData();
}
