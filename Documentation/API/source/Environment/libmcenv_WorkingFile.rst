
CWorkingFile
====================================================================================================


.. cpp:class:: LibMCEnv::CWorkingFile : public CBase 

	Working File Access Object




	.. cpp:function:: std::string GetAbsoluteFileName()

		Retrieves absolute file name of the working file

		:returns: global path of the file


	.. cpp:function:: LibMCEnv_uint64 GetSize()

		Returns the size of temporary file.

		:returns: file size


	.. cpp:function:: std::string CalculateSHA2()

		Calculates the SHA256 checksum of the file.

		:returns: sha256 checksum


	.. cpp:function:: PWorkingFileExecution ExecuteFile()

		Executes the temporary file, if it is an executable.

		:returns: execution object


	.. cpp:function:: bool IsManaged()

		Returns if the file is managed.

		:returns: returns if the file is managed.


	.. cpp:function:: void MakeManaged()

		Makes the file managed if it is not managed yet.



	.. cpp:function:: bool FileExists()

		Returns if the file exists on disk.

		:returns: returns if the file exists.


	.. cpp:function:: bool DeleteFromDisk()

		Deletes the temporary file.

		:returns: returns if deletion was successful or file did not exist in the first place.


.. cpp:type:: std::shared_ptr<CWorkingFile> LibMCEnv::PWorkingFile

	Shared pointer to CWorkingFile to easily allow reference counting.

