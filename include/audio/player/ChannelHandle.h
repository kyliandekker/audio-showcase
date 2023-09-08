#pragma once

#include <cstdint>

namespace uaudio
{
	namespace player
	{
		constexpr int32_t CHANNEL_NULL_HANDLE = -1;

		struct ChannelHandle
		{
		public:
			ChannelHandle() = default;
			ChannelHandle(const int32_t rhs) { m_Handle = rhs; }
			ChannelHandle(const ChannelHandle& rhs) { m_Handle = rhs; }
			~ChannelHandle() = default;

			ChannelHandle& operator=(const ChannelHandle& rhs) { m_Handle = rhs; return *this; }

			operator int32_t() const
			{
				return m_Handle;
			}

			ChannelHandle& operator=(const int32_t a_Rhs)
			{
				m_Handle = a_Rhs;
				return *this;
			}

			bool IsValid() const
			{
				return m_Handle != CHANNEL_NULL_HANDLE;
			}

		protected:
			int32_t m_Handle = CHANNEL_NULL_HANDLE;
		};
	}
}