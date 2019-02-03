#pragma once

#include <thread>
#include <atomic>
#include <future>
#include <optional>
#include <type_traits>

namespace automaton
{
	template <typename _Scheduler, typename _Impl>
	struct PollThread
	{
		using scheduler_type = _Scheduler;
		using clock_type = typename _Scheduler::clock_type;

		PollThread (scheduler_type& sched, bool autostart = false)
		:	_scheduler {sched},
			_polfuture {}
		{
			if (autostart)
				start ();
		}	

		void start()
		{
			_polfuture = std::async([this]()
			{
				_internal_execute ();				
			});
		}


		auto stop ()
		{
			_quit = true;
		}

		auto join () -> std::optional<std::exception_ptr>
		{
			try
			{
				_polfuture.get ();
			}
			catch (...)
			{
				return std::current_exception ();
			}
			return std::nullopt;
		}

		~PollThread()
		{
			stop ();
			join ();
		}

		scheduler_type& scheduler () const
		{
			return _scheduler;
		}

	protected:
		bool init (scheduler_type&)
		{
			return false;
		}

		bool loop (scheduler_type&)
		{
			return false;
		}

		void exit (scheduler_type&)
		{}

	private:
		void _internal_execute ()
		{
			auto& _impl = static_cast<_Impl&>(*this);
			if (_impl.init (_scheduler))
			{
				while (!_quit && _impl.loop (_scheduler));
				_impl.exit(_scheduler);
			}
		}

	private:
		scheduler_type& _scheduler;
		std::atomic_bool _quit{ false };
		std::future<void> _polfuture;
	};
	

}