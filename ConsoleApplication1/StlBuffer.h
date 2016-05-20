/*
** StlBuffer
**	Uses the STL container 'vector' as a buffer, as oppose to using an array for a buffer.
*/

#ifndef _STLBUFFER_H
#define _STLBUFFER_H

#include "stdafx.h"

class STLBuffer
{
	public:
		STLBuffer() : 
			m_stReadPos(0), 
			m_stWritePos(0)
		{}

		STLBuffer(const size_t reserveSize) : 
			m_stReadPos(0),
			m_stWritePos(0)
		{
			m_vData.reserve(reserveSize);
			m_stReadPos = 0;
			m_stWritePos = getSize();
		}

		STLBuffer(const STLBuffer &buf) : 
			m_stReadPos(buf.m_stReadPos), m_stWritePos(buf.m_stWritePos), m_vData(buf.m_vData)
		{}

		virtual ~STLBuffer()
		{
			//
		}

		void resize(const size_t stSize)
		{
			m_vData.resize(stSize);
			m_stReadPos = 0;
			m_stWritePos = getSize();
		}

		void clear()
		{
			m_vData.clear();
			m_stReadPos = 0;
			m_stWritePos = 0;
		}

		void append(const std::string& str)
		{
			append((uint8 const*)str.c_str(), str.size() + 1);
		}

		void append(const uint8* src, size_t cnt)
		{
			if (!cnt)
				return;

			assert(getSize() < MAX32BIT);

			if (m_vData.size() < m_stWritePos + cnt)
				m_vData.resize(m_stWritePos + cnt);

			memcpy(&m_vData[m_stWritePos], src, cnt);
			m_stWritePos += cnt;
		}

		void append(const STLBuffer& buffer)
		{
			if (buffer.getWritePosition())
				append(buffer.getContents(), buffer.getWritePosition());
		}

		bool isEmpty() const 
		{ 
			return m_vData.empty(); 
		}

		uint8 const* getContents() const
		{
			return &m_vData[0];
		}

		// In regards to how much is left to read
		size_t getRemainingSize() const
		{
			if (m_vData.size() && m_stReadPos < m_vData.size())
				return m_vData.size() - m_stReadPos;

			return 0;
		}

		size_t getSize() const 
		{ 
			return m_vData.size();
		}

		size_t getReadPosition() const
		{
			return m_stReadPos;
		}

		size_t getReadPosition(size_t rpos_)
		{
			m_stReadPos = rpos_;
			return m_stReadPos;
		}

		size_t getWritePosition() const
		{
			return m_stWritePos;
		}

		size_t getWritePosition(size_t wpos_)
		{
			m_stWritePos = wpos_;
			return m_stWritePos;
		}

		template<class T>
		STLBuffer &operator<<(T value)
		{
			append<T>(value);
			return *this;
		}

		STLBuffer &operator<<(const std::string &value)
		{
			append((uint8 const*)value.c_str(), value.length());
			append((uint8)0);
			return *this;
		}

		STLBuffer &operator<<(const char* str)
		{
			append((uint8 const*)str, str ? strlen(str) : 0);
			append((uint8)0);
			return *this;
		}

		template<class T>
		STLBuffer &operator>>(T& value)
		{
			if (getRemainingSize() < sizeof(T))
				throw StlBufferException(m_stWritePos, getSize());

			value = read<T>();
			return *this;
		}

		STLBuffer &operator>>(std::string& value)
		{
			value.clear();

			while (getReadPosition() < getSize() && getRemainingSize())
			{
				char c = read<char>();

				if (c == 0)
					break;

				value += c;
			}

			return *this;
		}

	public:
		class StlBufferException
		{
			public:
				StlBufferException(const size_t accessPos, const size_t actualSize)
				{
					printf("StlBufferException - accessPos = %zu, actualSize = %zu\n", accessPos, actualSize);
				}
		};

	protected:
		size_t m_stReadPos;
		size_t m_stWritePos;

		std::vector<uint8> m_vData;

	private:
		// Limited for internal use because can "append" any unexpected type (like pointer and etc) with hard detection problem
		template <typename T> 
		void append(T value)
		{
			append((uint8 *)&value, sizeof(value));
		}

		template <typename T>
		T read()
		{
			T r = read<T>(m_stReadPos);
			m_stReadPos += sizeof(T);
			return r;
		}

		template <typename T>
		T read(size_t pos) const
		{
			if (pos + sizeof(T) > getSize())
				throw StlBufferException(pos + sizeof(T), getSize());

			T val = *((T const*)&m_vData[pos]);
			return val;
		}
};

#endif


