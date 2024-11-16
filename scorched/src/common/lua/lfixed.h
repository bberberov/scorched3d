#ifndef lfixed_h
#define lfixed_h

void fixed_lua_number2str(char *output, int n);
int fixed_lua_str2number(const char *s, char **endptr);
int fixed_lua_number2int(int n);

int fixed_luai_nummul(int a, int b);
int fixed_luai_numdiv(int a, int b);
int fixed_luai_nummod(int a, int b);
int fixed_luai_numpow(int a, int b);

#endif
