
CTestEnvironment
====================================================================================================


.. cpp:class:: LibMCEnv::CTestEnvironment : public CBase 

	




	.. cpp:function:: void WriteTestOutput(const std::string & sOutputName, const CInputVector<LibMCEnv_uint8> & DataBuffer)

		Write output file to test directory.

		:param sOutputName: Output file name. Only alphanumeric characters, point, underscore and hypen are allowed. 
		:param DataBuffer: Test data to write into output directory. 


.. cpp:type:: std::shared_ptr<CTestEnvironment> LibMCEnv::PTestEnvironment

	Shared pointer to CTestEnvironment to easily allow reference counting.

