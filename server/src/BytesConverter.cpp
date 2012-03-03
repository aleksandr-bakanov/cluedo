#include "include/BytesConverter.hpp"

int BCgetInt(const char * s)
{
	BCUnion u;
	memcpy(u.bytes, s, INT_SIZE);
	return u.integer;
}

int BCgetInt(const char * s, int& recvPos)
{
	BCUnion u;
	memcpy(u.bytes, s + recvPos, INT_SIZE);
	recvPos += INT_SIZE;
	return u.integer;
}

short BCgetShort(const char * s)
{
	BCUnion u;
	memcpy(u.bytes, s, INT_SIZE);
	return u.shortInteger;
}

short BCgetShort(const char * s, int& recvPos)
{
	BCUnion u;
	memcpy(u.bytes, s + recvPos, INT_SIZE);
	recvPos += SHORT_SIZE;
	return u.shortInteger;
}

string BCgetUTF(const char * s)
{
	short strSize (BCgetShort(s));
	string result (s + SHORT_SIZE, strSize);
	return result;
}

string BCgetUTF(const char * s, int& recvPos)
{
	short strSize (BCgetShort(s + recvPos));
	string result (s + recvPos + SHORT_SIZE, strSize);
	recvPos += strSize + SHORT_SIZE;
	return result;
}

bool BCgetBoolean(const char * s)
{
	bool b = (bool)(*s);
	return b;
}

bool BCgetBoolean(const char * s, int& recvPos)
{
	bool b = (bool)(*(s + recvPos));
	recvPos += BOOL_SIZE;
	return b;
}

string BCitos(int i)
{
	char * c = new char[INT_SIZE * 4];
	ostringstream sin;
	sin << i;
	string s = sin.str();
	delete [] c;
	c = NULL;
	return s;
}

char BCgetChar(const char * s)
{
	char c = *s;
	return c;
}

char BCgetChar(const char * s, int& recvPos)
{
	char c = *(s + recvPos);
	recvPos += CHAR_SIZE;
	return c;
}
