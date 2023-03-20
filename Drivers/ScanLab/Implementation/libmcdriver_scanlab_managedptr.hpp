/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

*/


#ifndef __LIBMCDRIVER_SCANLAB_MANAGEDPTR
#define __LIBMCDRIVER_SCANLAB_MANAGEDPTR

#include "libmcdriver_scanlab_interfaces.hpp"

// Include custom headers here.
#include <map>

namespace LibMCDriver_ScanLab {
namespace Impl {


	template<class T>
	class act_managed_ptr
	{
	private:
		T* ptr = nullptr;		

	public:
		act_managed_ptr() : ptr(nullptr)
		{
		}

		act_managed_ptr(T* ptr) : ptr(ptr)
		{
			if (ptr != nullptr)
				ptr->IncRefCount();
		}

		act_managed_ptr(const act_managed_ptr& obj) // copy constructor
		{
			this->ptr = obj.ptr; // share the underlying pointer
			if (this->ptr != nullptr)
				this->ptr->IncRefCount();

		}

		act_managed_ptr& operator=(const act_managed_ptr& obj) // copy assignment
		{
			__cleanup__(); // cleanup any existing data

			// Assign incoming object's data to this object
			this->ptr = obj.ptr; // share the underlying pointer
			if (this->ptr != nullptr)
				this->ptr->IncRefCount();
		}

		/*** Move Semantics ***/
		act_managed_ptr(act_managed_ptr&& dyingObj) // move constructor
		{
			this->ptr = dyingObj.ptr; 
			dyingObj.ptr = nullptr;
		}

		act_managed_ptr& operator=(act_managed_ptr&& dyingObj) // move assignment
		{
			__cleanup__(); // cleanup any existing data

			this->ptr = dyingObj.ptr;
			dyingObj.ptr = nullptr;

			return *this;
		}

		T* get() const
		{
			return this->ptr;
		}

		T* operator->() const
		{
			return this->ptr;
		}

		T& operator*() const
		{
			return this->ptr;
		}

		~act_managed_ptr() // destructor
		{
			__cleanup__();
		}

	private:
		void __cleanup__()
		{
			if (this->ptr != nullptr)
				this->ptr->DecRefCount();
			this->ptr = nullptr;
		}
	};
	

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#endif // __LIBMCDRIVER_SCANLAB_MANAGEDPTR
