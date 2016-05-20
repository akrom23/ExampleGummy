/*
** gummy52
**	Windows only
*/

#ifndef _UTIL_H
#define _UTIL_H

// Needed for this specific program
#include "stdafx.h"

typedef signed __int64 int64;
typedef int int32;
typedef short int16;
typedef char int8;

typedef unsigned __int64 uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

#define MAX32BIT 2147483647
#define MAX16BIT 32767

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <memory>
#include <random>

#include <assert.h>
#include <time.h>

#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib")

// C Connector
#include <mysql.h>

#include "StlBuffer.h"
#include "QueryObjects.h"
#include "DbField.h"
#include "Database.h"
#include "SocketMgr.h"
#include "TcpListener.h"
#include "Packet.h"

#define FORMAT_STRING_ARGS(format, output, len)	\
{												\
	va_list ap;									\
	char szQuery[len];							\
	va_start(ap, format);						\
	vsprintf(szQuery, format, ap);				\
	va_end(ap);									\
	output = szQuery;							\
}

namespace Util
{
	static bool timePassed(const time_t tTime)
	{
		return time(nullptr) >= tTime;
	}

	static bool stringsEqualInsensitive(const std::string& a, const std::string& b)
	{
		size_t sz = a.size();

		if (b.size() != sz)
			return false;

		for (size_t i = 0; i < sz; ++i)
		{
			if (tolower(a[i]) != tolower(b[i]))
				return false;
		}

		return true;
	}
	
	static bool stringContainsOtherStringInsensitive(std::string masterStr, std::string str)
	{
		std::transform(masterStr.begin(), masterStr.end(), masterStr.begin(), tolower);
		std::transform(str.begin(), str.end(), str.begin(), tolower);
		return masterStr.find(str) != std::string::npos;
	}

	// Returns whether or not the vector contains an object
	template<class T>
	static bool vectorHas(const std::vector<T>& v, T obj)
	{
		return find(v.begin(), v.end(), obj) != v.end();
	}

	// Returns whether or not the vector contains an object
	template<class T>
	static void removeAllFromVector(std::vector<T>& v, T obj)
	{
		auto itr = v.begin();

		while (itr != v.end())
		{
			if (*itr == obj)
			{
				itr = v.erase(itr);
			}
			else
			{
				++itr;
			}
		}
	}
	
	template<class T>
	static void fillVectorFromStringOfNumbers(std::string str, std::vector<T> &_vector)
	{
		std::istringstream iss(str);	
			
		while (iss.good())
		{
			T intVal = 0;
			iss >> intVal;

			if (intVal)
				_vector.push_back(intVal);
		} 
	}

	static void strReplaceAll(std::string &str, const std::string& from, const std::string& to) 
	{
		size_t start_pos = 0;

		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
    }

	static std::string readTextFile(const std::string addr)
	{
		std::string result;

		std::ifstream file(addr, std::ios::binary);

		if (file.is_open()) 
		{
			std::ostringstream os;
			os << file.rdbuf();
			result = os.str();
		}
		else 
		{
			printf("Util::readTextFile - File %s not found.\n", addr.c_str());
		}

		return result;
	}	

	// Loads data into a StlBuffer
	static bool readFile(const std::string addr, STLBuffer& result)
	{
		if (result.getSize())
		{
			printf("Util::readFile paramter result needed to be empty... ");
			result.clear();
			printf("we cleared it for you.\n");
		}

		FILE* pFile = nullptr;
		fopen_s(&pFile, addr.c_str(), "rb");

		if (pFile == nullptr) 
			return false;

		// Obtain file size and allocate memory to contain the whole file
		fseek(pFile, 0, SEEK_END);
		result.resize(ftell(pFile));
		rewind(pFile);

		// Copy the file into the buffer:
		size_t readSize = fread((char*)result.getContents(), 1, result.getSize(), pFile);

		// This should never be true
		if (readSize != result.getSize()) 
		{
			// If this somehow happens then wipe the result
			fputs("Util::readFile fread error\n", stderr);
			result.clear();
		}

		fclose(pFile);
		return readSize == result.getSize();
	}

	/* Return a random number in the range min..max; (max-min) must be smaller than 32768. */
	static int32 irand(const int32 min, const int32 max);

	/* Return a random number in the range min..max (inclusive). For reliable results, the difference
	* between max and min should be less than RAND32_MAX. */
	static uint32 urand(const uint32 min, const uint32 max);

	/* Return a random number in the range min..max (inclusive). For reliable results, the difference
	* between max and min should be less than max. */
	static float frand(const float min, const float max);

	/* Return a random number in the range 0 .. RAND32_MAX. */
	static int32 rand32();

	/* Return a random double from 0.0 to 1.0 (exclusive). Floats support only 7 valid decimal digits.
	 * A double supports up to 15 valid decimal digits and is used internally (RAND32_MAX has 10 digits).
	 * With an FPU, there is usually no difference in performance between float and double. */
	static double rand_norm();

	/* Return a random double from 0.0 to 99.9999999999999. Floats support only 7 valid decimal digits.
	 * A double supports up to 15 valid decimal digits and is used internaly (RAND32_MAX has 10 digits).
	 * With an FPU, there is usually no difference in performance between float and double. */
	static double rand_chance();

	/* Return true if a random roll fits in the specified chance (range 0-100). */
	static bool roll_chance_f(const float chance);

	/* Return true if a random roll fits in the specified chance (range 0-100). */
	static bool roll_chance_i(const int32 chance);

	template<class T>
	int32 inline stringFindAndReplace(T& source, const T& find, const T& replace)
	{
		int32 num = 0;

		typename T::size_type fLen = find.size();
		typename T::size_type rLen = replace.size();

		for (T::size_type pos=0; (pos=source.find(find, pos))!=T::npos; pos+=rLen)
		{
			num++;
			source.replace(pos, fLen, replace);
		}

		return num;
	}

	// Vector must not be empty
	template<class T>
	static T randVecObj(const std::vector<T>& v)
	{
		ASSERT(!v.empty());
		return v[Util::urand(0, v.size() - 1)];
	}

	// Will send the data over sock until completion. (WinSock32)
	//	Will return the amount sent or SOCKET_ERROR if there was an error.
	static int32 sendBytes(SOCKET sock, char* data, int32 size)
	{
		int32 result = size;

		// Send the msg until complete
		for (uint32 amountSent = 0, maxSend = size, amountLeft = size; amountSent < maxSend && result > 0;)
		{
			result = send(sock, data + amountSent, amountLeft, 0); 

			if (result == SOCKET_ERROR)
				return SOCKET_ERROR;

			amountLeft -= result;
			amountSent += result;
		}

		return size;
	}
	
	// Will receive the data over sock until completion. (WinSock32)
	//	Will return the amount received or SOCKET_ERROR if we lost connection or if there was an error
	static int32 recvBytes(SOCKET sock, char* buffer, int32 size)
	{        
		int32 result = 0;

		while (result < size)
		{
			int32 ret = recv(sock, buffer + result, size - result, 0);

			// If the connection has been gracefully closed, the return value is zero.
			if (ret <= 0)
				return SOCKET_ERROR;

			if (ret)
				result += ret;
		}

		if (result != size)
			printf("Util::recvBytes mis matched\n");

		return result;
	}
};

// Allows + operator to function with vector
template <typename T>
std::vector<T> operator+(const std::vector<T>& vec1, const std::vector<T>& vec2)
{
	std::vector<T> result;
	result.reserve(vec1.size() + vec2.size());
	result.insert(result.end(), vec1.begin(), vec1.end());
	result.insert(result.end(), vec2.begin(), vec2.end());
	return result;
}

// Allows += operator to function with vector
template <typename T>
std::vector<T> &operator+=(std::vector<T>& vec1, const std::vector<T>& vec2)
{
	vec1.reserve(vec1.size() + vec2.size());
	vec1.insert(vec1.end(), vec2.begin(), vec2.end());
	return vec1;                               
}

#endif


