#pragma once

#include "pollthread.hpp"
#include "dgram.hpp"

namespace automaton
{

	template <typename _Scheduler>
	struct TempProbe : PollThread<_Scheduler, TempProbe<_Scheduler>>
	{
		using PollThread<_Scheduler, TempProbe<_Scheduler>>::PollThread;
		friend struct PollThread<_Scheduler, TempProbe<_Scheduler>>;

		using scheduler_type = typename _Scheduler;
		using callback_type = std::function<void (std::uint64_t id, std::int32_t value, float multiplier)>;

		template <typename _Callback>
		void callback (_Callback&& cbk)
		{
			_callback = std::forward<_Callback> (cbk);
		}

		TempProbe (_Scheduler& sched, bool autostart = false)
		:	PollThread<_Scheduler, TempProbe<_Scheduler>> (sched, autostart)
		{}

	protected:

		bool init (scheduler_type& sched)
		{
			_listener.bind ({ 10000, address::anyaddres () });
			return true;
		}

		bool loop (scheduler_type& sched)
		{
			full_address faddr;
			Packet packet;
			auto size = _listener.recv (faddr, &packet, sizeof(packet));
			if (!_callback)
				return true;
			if (size != sizeof (packet))
				return true;
			if (packet.magic != 'SENS')
				return true;
			if (packet.type != 0xD518B20)
				return true;
			sched.post ([packet, this] (auto&& sched, auto delay) {
				_callback (packet.addr, packet.value, 1.0 / 128.0);
			});

			return true;
		}

		void exit (scheduler_type& sched)
		{
			_listener.close ();
		}


	private:

	#pragma pack(push, 1)
		struct Packet
		{
			std::uint32_t magic;
			std::uint32_t type;
			std::uint64_t addr;
			std::int32_t value;
		};
	#pragma pack(pop)
		dgram::Socket _listener;
		callback_type _callback;
	};
}