
CIterator
====================================================================================================


.. cpp:class:: LibMCEnv::CIterator : public CBase 

	




	.. cpp:function:: bool MoveNext()

		Iterates to the next object in the list.

		:returns: Iterates to the next object in the list.


	.. cpp:function:: bool MovePrevious()

		Iterates to the previous object in the list.

		:returns: Iterates to the previous object in the list.


	.. cpp:function:: PBase GetCurrent()

		Returns the object the iterator points at.

		:returns: returns the object instance.


	.. cpp:function:: PIterator Clone()

		Creates a new object iterator with the same object list.

		:returns: returns the cloned Iterator instance


	.. cpp:function:: LibMCEnv_uint64 Count()

		Returns the number of resoucres the iterator captures.

		:returns: returns the number of objects the iterator captures.


.. cpp:type:: std::shared_ptr<CIterator> LibMCEnv::PIterator

	Shared pointer to CIterator to easily allow reference counting.

