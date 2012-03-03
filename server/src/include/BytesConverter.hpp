#ifndef _BYTES_CONVERTER_
#define _BYTES_CONVERTER_

#include <string>
#include <string.h>
#include <sstream>
using namespace std;

union BCUnion
{
	char bytes[4];
	int integer;
	short shortInteger;
	bool boolean;
};

const int INT_SIZE = 4;
const int SHORT_SIZE = 2;
const int CHAR_SIZE = 1;
const int BOOL_SIZE = 1;
const int FLOAT_SIZE = 4;

/**
 * These functions converts bytes sequences into different formats.
 * Like int, short, char, bool and string.
 * Also functions set pointer in char sequence gived to them on
 * next bytes accordingly with a geting value type.
 * And there are functions that do not change some pointers in array.
 */
int BCgetInt(const char *);
int BCgetInt(const char *, int&);
short BCgetShort(const char *);
short BCgetShort(const char *, int&);
string BCgetUTF(const char *);
string BCgetUTF(const char *, int&);
bool BCgetBoolean(const char *);
bool BCgetBoolean(const char *, int&);
char BCgetChar(const char *);
char BCgetChar(const char *, int&);
string BCitos(int i);

#endif /* _BYTES_CONVERTER_ */
