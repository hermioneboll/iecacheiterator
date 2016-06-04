#include "stdafx.h"
#include <windows.h>
#include <WinInet.h>
#include <iterator>

class cache_iter : public std::iterator < std::input_iterator_tag, INTERNET_CACHE_ENTRY_INFO >
{
	HANDLE m_hCacheEntry;
	LPINTERNET_CACHE_ENTRY_INFO m_lpCacheEntryInfo;
    unsigned int MCACHE_ENTRY_SIZE=4096;

public:
	cache_iter() : m_hCacheEntry(0), m_lpCacheEntryInfo(0)
	{
		DWORD dwInitCacheEntrySize = MCACHE_ENTRY_SIZE;
		m_lpCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)(new char[dwInitCacheEntrySize]);
		m_lpCacheEntryInfo->dwStructSize = dwInitCacheEntrySize;
		m_hCacheEntry = ::FindFirstUrlCacheEntry(NULL, m_lpCacheEntryInfo, &dwInitCacheEntrySize);

		if (0==m_hCacheEntry)
		{
			delete[] m_lpCacheEntryInfo;
			switch (::GetLastError())
			{
			case ERROR_NO_MORE_ITEMS:
				break;
			case  ERROR_INSUFFICIENT_BUFFER:
				m_lpCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)(new char[dwInitCacheEntrySize]);
				m_lpCacheEntryInfo->dwStructSize = dwInitCacheEntrySize;
				m_hCacheEntry = ::FindFirstUrlCacheEntry(NULL, m_lpCacheEntryInfo, &dwInitCacheEntrySize);
				if (0 == m_hCacheEntry)
					delete[] m_lpCacheEntryInfo;
				break;
			default:
				::FindCloseUrlCache(m_hCacheEntry);
				m_hCacheEntry = 0;
			}
		}
	}
	cache_iter(LPINTERNET_CACHE_ENTRY_INFO mlp) : m_hCacheEntry(0), m_lpCacheEntryInfo(mlp)
	{}
	cache_iter(cache_iter & citer) :m_hCacheEntry(citer.m_hCacheEntry), m_lpCacheEntryInfo(citer.m_lpCacheEntryInfo)
	{}
	cache_iter& operator++()
	{
		DWORD dwInitCacheEntrySize = 4096;
		m_lpCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)(new char[dwInitCacheEntrySize]);
		m_lpCacheEntryInfo->dwStructSize = dwInitCacheEntrySize;
		if (!::FindNextUrlCacheEntry(m_hCacheEntry, m_lpCacheEntryInfo, &dwInitCacheEntrySize))
		{
			delete[] m_lpCacheEntryInfo;
			switch (::GetLastError())
			{
			case ERROR_NO_MORE_ITEMS:
				::FindCloseUrlCache(m_hCacheEntry);
				m_hCacheEntry = 0;
				break;
			case ERROR_INSUFFICIENT_BUFFER:
				m_lpCacheEntryInfo = (LPINTERNET_CACHE_ENTRY_INFO)(new char[dwInitCacheEntrySize]);
				m_lpCacheEntryInfo->dwStructSize = dwInitCacheEntrySize;
				if (!::FindCloseUrlCache(m_hCacheEntry))
				{
					delete[] m_lpCacheEntryInfo;
					::FindCloseUrlCache(m_hCacheEntry);
					m_hCacheEntry = 0;
				}
				break;
			default:
				::FindCloseUrlCache(m_hCacheEntry);
				m_hCacheEntry = 0;
			}
		}
		return *this;
	}
	cache_iter operator++(int)
	{
		cache_iter temp = *this;
		++(*this);
		return temp;
	}
	bool operator==(const cache_iter& citer) const
	{
		return m_hCacheEntry == citer.m_hCacheEntry;
	}
	bool operator!=(const cache_iter& citer) const
	{
		return !(*this == citer);
	}
	INTERNET_CACHE_ENTRY_INFO operator*()
	{
		return *m_lpCacheEntryInfo;
	}
};
