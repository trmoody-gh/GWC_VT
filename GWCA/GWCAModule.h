#pragma once

#include <vector>

namespace GWCA {

	class BaseModule {
		friend class ApiMgr;

	public:
		virtual ~BaseModule() {}

	protected:
		BaseModule();

	private:
		virtual void OnDestruct() {};
	};

	template <class T>
	class Module : BaseModule {
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
	T* Module<T>::instance_ = nullptr;
}

