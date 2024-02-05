
CWorkingDirectory
====================================================================================================


.. cpp:class:: LibMCEnv::CWorkingDirectory : public CBase 

	Working Directory Access Object




	.. cpp:function:: bool IsActive()

		Working directory is active.

		:returns: returns true if files can be read and written to the directory.


	.. cpp:function:: std::string GetAbsoluteFilePath()

		Retrieves absolute file path.

		:returns: global path of the directory, including path delimiter.


	.. cpp:function:: PWorkingFile StoreCustomData(const std::string & sFileName, const CInputVector<LibMCEnv_uint8> & DataBufferBuffer)

		Stores a data buffer in a temporary file.

		:param sFileName: filename to store to. Can not include any path delimiters or .. 
		:param DataBufferBuffer: file data to store to. 
		:returns: working file instance.


	.. cpp:function:: PWorkingFile StoreCustomString(const std::string & sFileName, const std::string & sDataString)

		Stores a string in a temporary file.

		:param sFileName: filename to store to. Can not include any path delimiters or .. 
		:param sDataString: file data to store to. 
		:returns: working file instance.


	.. cpp:function:: PWorkingFile StoreDriverData(const std::string & sFileName, const std::string & sIdentifier)

		Stores attached driver data in a temporary file.

		:param sFileName: filename to store to. Can not include any path delimiters or .. 
		:param sIdentifier: identifier of the binary data in the driver package. 
		:returns: working file instance.


	.. cpp:function:: bool CleanUp()

		Deletes all managed files in the directory and the directory. No storing is possible after a cleanup.

		:returns: returns if deletion was successful.


	.. cpp:function:: PWorkingFile AddManagedFile(const std::string & sFileName)

		Adds a managed filename in the directory (i.e. this file will be deleted at CleanUp). Subdirectories are not allowed.

		:param sFileName: Filename to manage. The file does not need to exist yet. 
		:returns: working file instance.


	.. cpp:function:: bool HasUnmanagedFiles()

		Returns if the working directory has unmanaged files. A clean implementation will never deal with unmanaged files.

		:returns: returns if there are unmanaged files.


	.. cpp:function:: PWorkingFileIterator RetrieveUnmanagedFiles()

		Returns a list of unmanaged files.

		:returns: working file iterator instance.


	.. cpp:function:: PWorkingFileIterator RetrieveManagedFiles()

		Returns a list of managed files.

		:returns: working file iterator instance.


	.. cpp:function:: PWorkingFileIterator RetrieveAllFiles()

		Returns a list of all files in the directory.

		:returns: working file iterator instance.


.. cpp:type:: std::shared_ptr<CWorkingDirectory> LibMCEnv::PWorkingDirectory

	Shared pointer to CWorkingDirectory to easily allow reference counting.

