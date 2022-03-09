

class CBase 
{

	constructor (Wrapper, Handle)
	{
		this.Wrapper = Wrapper;
		this.Handle = Handle;
	}
	
	release ()
	{
		if (this.Wrapper) {
			if (this.Handle) {
				this.Handle = 0;
			}
		}
	}

}


class COperationResult extends CBase
{
	
	constructor (Wrapper, Handle)
	{
		super (Wrapper, Handle);
	}
	
	
}



class CConnection extends CBase
{
	
	constructor (Wrapper, Handle)
	{
		super (Wrapper, Handle);
	}
	
	GetBaseURL ()
	{
		var errorCode = 0;
		var resultBaseURL = "";
		var lenBaseURL = 0;
		var lenptrBaseURL = this.Wrapper._allocUint32 ();		
		
		errorCode = this.Wrapper.Module.ccall (
				'libamcf_connection_getbaseurl',	
				'number',	
				['number', 'number', 'number', 'number'],
				[this.Handle, 0, lenptrBaseURL, 0]	
			);

		
		if (errorCode === 0) {
			lenBaseURL = this.Wrapper._readUint32 (lenptrBaseURL);					
		} else {
			this.Wrapper._dealloc (lenptrBaseURL);
			this.Wrapper._handleErrorCode (this, errorCode);
		}		
				
		var ptrBaseURL = this.Wrapper._allocArray (lenBaseURL + 1);
		
		errorCode = this.Wrapper.Module.ccall (
				'libamcf_connection_getbaseurl',	
				'number',	
				['number', 'number', 'number', 'number'],
				[this.Handle, lenBaseURL + 1, lenptrBaseURL, ptrBaseURL]	
			);
		
		if (errorCode === 0) {
			resultBaseURL = this.Wrapper.Module.UTF8ToString (ptrBaseURL);
		}		
		
		this.Wrapper._dealloc (lenptrBaseURL);
		this.Wrapper._dealloc (ptrBaseURL);
		
		this.Wrapper._handleErrorCode (this, errorCode);
		
		return resultBaseURL;
	}
	
	
	GetTimeout ()
	{
		var resultTimeOut = 0;
		var resultPtrTimeout = this.Wrapper._allocUint32 ();
		
		var errorCode = this.Wrapper.Module.ccall (
				'libamcf_connection_gettimeout',	
				'number',	
				['number', 'number'],
				[this.Handle, resultPtrTimeout]	
			);
			
		if (errorCode === 0) {
			resultTimeOut = this.Wrapper._readUint32 (resultPtrTimeout);		
		}
		this.Wrapper._dealloc (resultPtrTimeout);
		
		this.Wrapper._handleErrorCode (this, errorCode);
		
		return resultTimeOut;
	}
	
	
	GetRetryCount ()
	{
		var resultRetryCount = 0;
		var resultPtrRetryCount = this.Wrapper._allocUint32 ();
		
		var errorCode = this.Wrapper.Module.ccall (
				'libamcf_connection_getretrycount',	
				'number',	
				['number', 'number'],
				[this.Handle, resultPtrRetryCount]	
			);
			
		if (errorCode === 0) {
			resultRetryCount = this.Wrapper._readUint32 (resultPtrRetryCount);		
		}
		this.Wrapper._dealloc (resultPtrRetryCount);
		
		this.Wrapper._handleErrorCode (this, errorCode);
		
		return resultRetryCount;
	}
	
	Ping ()
	{
		var resultHandle = 0;
		var resultptrHandle = this.Wrapper._allocHandle ();		
		var errorCode = this.Wrapper.Module.ccall (
				'libamcf_connection_ping',	
				'number',	
				['number', 'number'],	
				[this.Handle, resultptrHandle]	
			);		
			
		resultHandle = this.Wrapper._readHandle (resultptrHandle);
		this.Wrapper._dealloc(resultptrHandle);
						
		this.Wrapper._handleErrorCode (this, errorCode);
						
		return new COperationResult (this.Wrapper, resultHandle);
	}
	
	AuthenticateWithPassword (UserName, Password) 
	{
		var resultHandle = 0;
		var resultptrHandle = this.Wrapper._allocHandle ();		
		var errorCode = this.Wrapper.Module.ccall (
				'libamcf_connection_authenticatewithpassword',	
				'number',
				['number', 'string', 'string', 'number'],	
				[this.Handle, UserName, Password, resultptrHandle]	
			);		
			
		resultHandle = this.Wrapper._readHandle (resultptrHandle);
		this.Wrapper._dealloc(resultptrHandle);
						
		this.Wrapper._handleErrorCode (this, errorCode);
						
		return new COperationResult (this.Wrapper, resultHandle);
	}

}



class CWrapper {
	
	constructor (Module)
	{
		this.Module = Module;			
	}
	
	_allocUint32 ()
	{
		return this.Module._malloc(4);
	}

	_allocArray (arraySize)
	{
		return this.Module._malloc(arraySize);
	}

	_readUint32 (Ptr)
	{
		return this.Module.getValue (Ptr, "i32");
	}

	_allocHandle ()
	{
		return this._allocUint32 ();
	}

	_readHandle (Ptr)
	{
		return this._readUint32 (Ptr);
	}

	_dealloc (Handle)
	{
		return this.Module._free(Handle);
	}
	
	_handleErrorCode (Instance, ErrorCode)
	{
		if (ErrorCode) {
			
			var ErrorInfo = { ErrorMessage: "", HasError: false};
			
			if (Instance) {
				ErrorInfo = this.GetLastError (Instance);
			}
			
			if (ErrorInfo.HasError) {
				throw "LibAMCF Error #" + ErrorCode + ": " + ErrorInfo.ErrorMessage;
				
			} else {			
				throw "Generic LibAMCF Error #" + ErrorCode;
			}
		}
	}
	
	
	GetVersion ()
	{

		var resultObject = {};
		var resultPtrMajorVersion = this._allocUint32 ();
		var resultPtrMinorVersion = this._allocUint32 ();
		var resultPtrMicroVersion = this._allocUint32 ();
		var errorCode = this.Module.ccall (
				'libamcf_getversion',	
				'number',	
				['number', 'number', 'number'],
				[resultPtrMajorVersion, resultPtrMinorVersion, resultPtrMicroVersion]	
			);		

		if (errorCode === 0) {
			resultObject = {
				MajorVersion: this._readUint32 (resultPtrMajorVersion),
				MinorVersion: this._readUint32 (resultPtrMinorVersion),
				MicroVersion: this._readUint32 (resultPtrMicroVersion),	
			};				
		}
		
		this._dealloc (resultPtrMajorVersion);
		this._dealloc (resultPtrMinorVersion);
		this._dealloc (resultPtrMicroVersion);
						
		this._handleErrorCode (null, errorCode);
		
		return resultObject;
	}
	
	GetLastError (Instance)
	{
		var errorCode = 0;
		var resultErrorMessage = "";
		var resultHasError = false;
		var lenErrorMessage = 0;
		var lenptrErrorMessage = this._allocUint32 ();		
		var ptrHasError = this._allocUint32 ();		
		
		errorCode = this.Module.ccall (
				'libamcf_getlasterror',	
				'number',	
				['number', 'number', 'number', 'number', 'number'],
				[Instance.Handle, 0, lenptrErrorMessage, 0, ptrHasError]	
			);

		
		if (errorCode === 0) {
			lenErrorMessage = this._readUint32 (lenptrErrorMessage);					
		} else {
			this._dealloc (lenptrErrorMessage);
			this._dealloc (ptrHasError);
			lenptrErrorMessage = 0;
			ptrHasError = 0;
			this._handleErrorCode (null, errorCode);
		}		
				
		var ptrErrorMessage = this._allocArray (lenErrorMessage + 1);
		
		errorCode = this.Module.ccall (
				'libamcf_getlasterror',	
				'number',	
				['number', 'number', 'number', 'number', 'number'],
				[Instance.Handle, lenErrorMessage + 1, lenptrErrorMessage, ptrErrorMessage, ptrHasError]	
			);
		
		if (errorCode === 0) {
			resultErrorMessage = this.Module.UTF8ToString (ptrErrorMessage);
		}		
		
		resultHasError = this._readUint32 (ptrHasError) != 0;
		
		this._dealloc (lenptrErrorMessage);
		this._dealloc (ptrErrorMessage);
		this._dealloc (ptrHasError);
		
		this._handleErrorCode (this, errorCode);
		
		return { ErrorMessage: resultErrorMessage, HasError: resultHasError };
	}
	
	
	CreateConnection (baseURL)
	{
		
		var resultHandle = 0;
		var resultPtrHandle = this._allocHandle ();		
		var errorCode = this.Module.ccall (
				'libamcf_createconnection',	
				'number',	
				['string', 'number'],	
				[baseURL, resultPtrHandle]	
			);		
			
		resultHandle = this._readHandle (resultPtrHandle);
		this._dealloc(resultPtrHandle);
						
		this._handleErrorCode (null, errorCode);
						
		return new CConnection (this, resultHandle);
		
	}
	
	
};