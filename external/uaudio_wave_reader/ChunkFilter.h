#pragma once

#include <cstdint>
#include "./Defines.h"

namespace uaudio
{
	namespace wave_reader
	{
		/*
		 * This is a filter for the wave file loader. If you want to only get specific chunks, use this.
		*/
		class ChunkFilter
		{
		public:
			size_t size() const { return m_Size; }

			ChunkFilter()
			{ }

			ChunkFilter(const char* a_Filter, size_t a_Size);

			const char* operator [](size_t i) const;
		private:
			size_t m_Size = 0;
			const char* m_Data = nullptr;
		};
	}
}