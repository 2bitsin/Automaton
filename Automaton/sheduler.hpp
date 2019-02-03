#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <vector>
#include <type_traits>
#include <condition_variable>
#include <cassert>

#include "clock.hpp"


namespace automaton
{
	namespace detail
	{
		template <typename _Scheduler>
		struct job
		{
			using scheduler_type = _Scheduler;
			using clock_type = typename _Scheduler::clock_type;
			using duration_type = typename clock_type::duration_type;
			using time_type = typename clock_type::time_type;
			using callable = void (scheduler_type&, duration_type);

			job (time_type when,
				std::function<callable> what,
				duration_type repeat = 0)
			:	when (when),
				what (std::move (what)),
				repeat (repeat)
			{}

			time_type when;
			std::function<callable> what;
			duration_type repeat{ 0 };
			std::atomic_bool cancelled{ false };

			struct handle
			{
				handle (job& j)
				:	_job (j) 
				{
					assert (&_job != nullptr);
				}

				void cancel ()
				{
					_job.cancelled = true;
				}

			private:
				job& _job;
			};
		};


		template <typename T, typename... U>
		auto pop (std::priority_queue<T, U...>& q)
		{
			auto tmp = const_cast<T&&>(q.top ());
			q.pop ();
			return tmp;
		}

	}


	template <typename _Clock = Clock>
	struct Scheduler
	{
		using clock_type = _Clock;
		using duration_type = typename _Clock::duration_type;
		using time_type = typename _Clock::time_type;

		Scheduler(_Clock& clock)
		:	_owningthread(std::this_thread::get_id()),
			_clock(clock)
		{}

		bool next ()
		{
			using namespace std::chrono;
			assert (std::this_thread::get_id () == _owningthread);

			if (_newbulletin)
			{
				if (_bulletinlock.try_lock ())
				{
					while (!_bulletinboard.empty ())
					{
						_jobqueue.emplace (std::move (_bulletinboard.back ()));
						_bulletinboard.pop_back ();
					}
				}
				_newbulletin = false;
				_bulletinlock.unlock ();
			}

			if (_jobqueue.empty ())
				return false;

			auto top = detail::pop (_jobqueue);

			if (top->cancelled)
				return !_jobqueue.empty ();

			std::unique_lock ulock{ _bulletinlock };

			if (_clock.wait_until (top->when, _pobox, ulock) == std::cv_status::timeout)
			{
				auto latency = duration_cast<duration_type>(_clock.now () - top->when);
				top->what (*this, latency);
				if (top->repeat > 0ms)
				{
					top->when = top->when + top->repeat;
					_jobqueue.emplace (std::move (top));
				}
				return !_jobqueue.empty ();
			}

			_jobqueue.emplace (std::move (top));
			return true;
		}

		template <typename _Task, typename ... _DurationArgs>
		auto post (time_type when, std::chrono::duration<_DurationArgs...> interval, _Task&& what)
		{
			using namespace std::chrono;
			auto _interval = duration_cast<duration_type>(interval);
			auto _the_job = std::make_unique<job>(
				when, std::forward<_Task> (what), _interval);
			handle promise{ *_the_job };
			if (std::this_thread::get_id () != _owningthread)
			{
				std::lock_guard _{ _bulletinlock };
				_bulletinboard.emplace_back (std::move(_the_job));
				_newbulletin = true;
				_pobox.notify_one ();
				return promise;
			}			
			_jobqueue.emplace (std::move(_the_job));
			return promise;
		}

		template <typename _Task>
		auto post (typename _Clock::time_type when, _Task&& what)
		{
			return post (when, 0ms, std::forward<_Task>(what));
		}

		template <typename _Task, typename ... _DurationArgs>
		auto post (std::chrono::duration<_DurationArgs...> interval, _Task&& what)
		{
			return post (_clock.now(), interval, std::forward<_Task> (what));
		}

		template <typename _Task>
		auto post (_Task&& what)
		{
			using namespace std::chrono_literals;
			return post (_clock.now(), 0ms, std::forward<_Task> (what));
		}

		auto wait ()
		{
			if (_shouldquit.load())
				return false;
			assert (std::this_thread::get_id () == _owningthread);
			std::unique_lock ulock{ _bulletinlock };
			_pobox.wait (ulock, [this] () {
				return !_bulletinboard.empty () || _shouldquit.load();
			});
			return !_shouldquit.load();
		}

		auto quit ()
		{
			_shouldquit = true;
			_pobox.notify_all ();
		}

		auto&& clock () const 
		{ 
			return _clock; 
		}

	protected:
		using job = detail::job<Scheduler>;		

		struct compare
		{
			bool operator () (
				const std::unique_ptr<job>& a,
				const std::unique_ptr<job>& b)
				const
			{
				return a->when > b->when;
			}
		};

		std::thread::id _owningthread;
		_Clock& _clock;
		std::priority_queue<std::unique_ptr<job>, std::vector<std::unique_ptr<job>>, compare> _jobqueue;
		std::atomic_bool _newbulletin{ false };
		std::vector<std::unique_ptr<job>> _bulletinboard;
		std::mutex _bulletinlock;
		std::condition_variable _pobox;
		std::atomic_bool _shouldquit{ false };
	public:
		using handle = typename job::handle;

	};
}