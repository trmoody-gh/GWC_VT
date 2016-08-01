#pragma once

#include <list>
#include <functional>
#include <future>
#include <vector>

#include "GWCAManager.h"
#include "Hooker.h"

namespace GWCA {

	// Shoutouts to DarthTon @ unknowncheats.me for this class.

	class GameThreadMgr : public GWCAManager<GameThreadMgr> {
		friend class GWCAManager<GameThreadMgr>;
		friend class Api;

	private:
		class BaseTask {
		public:
			virtual void run() = 0;
		};

		template<typename T>
		class Task : BaseTask {
		public:
			Task(std::packaged_task<T()>&& task) : task_(std::move(task)) {}
			void run() override { task_(); }

		private:
			std::packaged_task<T()> task_;
		};


		GameThreadMgr();
		void RestoreHooks() override;

		std::vector<std::function<void(void)> > calls_;
		std::vector<std::function<void(void)> > calls_permanent_;
		std::list<std::unique_ptr<BaseTask>> tasks_;
		static CRITICAL_SECTION criticalsection_;
		bool render_state_;
		Hook hk_game_thread_;

	public:

		// For use only in gameloop hook.
		void __stdcall CallFunctions();

		// Add function to gameloop queue, only use if you know what you're doing.
		template<typename F, typename... ArgTypes>
		void Enqueue(F&& Func, ArgTypes&&... Args)
		{
			EnterCriticalSection(&criticalsection_);
			calls_.emplace_back(std::bind(std::forward<F>(Func), std::forward<ArgTypes>(Args)...));
			LeaveCriticalSection(&criticalsection_);
		}

		template<typename F, typename... ArgTypes>
		void AddPermanentCall(F&& Func, ArgTypes&&... Args)
		{
			EnterCriticalSection(&criticalsection_);
			calls_permanent_.emplace_back(std::bind(std::forward<F>(Func), std::forward<ArgTypes>(Args)...));
			LeaveCriticalSection(&criticalsection_);
		}

		template<typename T>
		void EnqueueTask(std::packaged_task<T()>&& task)
		{
			EnterCriticalSection(&criticalsection_);
			auto pointer = std::unique_ptr<BaseTask>(new Task<T>(std::move(task)));
			tasks_.push_back(std::move(pointer));
			LeaveCriticalSection(&criticalsection_);
		}

		static void gameLoopHook();
		static void renderHook();
		void ToggleRenderHook();
	};
}
