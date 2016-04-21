#pragma once

#include <vector>

namespace GWCA {

	class GWCABaseModule {
		friend class Api;

	public:
		virtual ~GWCABaseModule() {}

	protected:
		GWCABaseModule();

	private:
		virtual void RestoreHooks() {};
	};

	template <class T>
	class GWCAModule : GWCABaseModule {
	public:
		inline static T& Instance() { return *InstancePtr(); }
		static T* InstancePtr() {
			if (instance_ == nullptr) {
				instance_ = new T();
			}
			return instance_;
		}

	private:
		static T* instance_;
	};

	template <class T>
	T* GWCAModule<T>::instance_ = nullptr;
}

