using System;
using System.Text;
using System.Runtime.InteropServices;

namespace LibAMCF {


	namespace Internal {


		public class LibAMCFWrapper
		{
			[DllImport("libamcf.dll", EntryPoint = "libamcf_operationresult_waitfor", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 OperationResult_WaitFor (IntPtr Handle, UInt32 ATimeOut, out Byte AOperationFinished);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_operationresult_ensuresuccess", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 OperationResult_EnsureSuccess (IntPtr Handle);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_operationresult_inprogress", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 OperationResult_InProgress (IntPtr Handle, out Byte AOperationIsInProgress);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_operationresult_success", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 OperationResult_Success (IntPtr Handle, out Byte AOperationSuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_operationresult_geterrormessage", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 OperationResult_GetErrorMessage (IntPtr Handle, UInt32 sizeErrorMessage, out UInt32 neededErrorMessage, IntPtr dataErrorMessage);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_datastream_getuuid", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 DataStream_GetUUID (IntPtr Handle, UInt32 sizeUUID, out UInt32 neededUUID, IntPtr dataUUID);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_datastream_getcontextuuid", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 DataStream_GetContextUUID (IntPtr Handle, UInt32 sizeContextUUID, out UInt32 neededContextUUID, IntPtr dataContextUUID);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_datastream_getname", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 DataStream_GetName (IntPtr Handle, UInt32 sizeName, out UInt32 neededName, IntPtr dataName);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_datastream_getmimetype", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 DataStream_GetMimeType (IntPtr Handle, UInt32 sizeMimeType, out UInt32 neededMimeType, IntPtr dataMimeType);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_datastream_getsha256", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 DataStream_GetSHA256 (IntPtr Handle, UInt32 sizeSHA256, out UInt32 neededSHA256, IntPtr dataSHA256);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_datastream_getsize", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 DataStream_GetSize (IntPtr Handle, out UInt64 AStreamSize);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_datastream_gettimestamp", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 DataStream_GetTimestamp (IntPtr Handle, UInt32 sizeTimestamp, out UInt32 neededTimestamp, IntPtr dataTimestamp);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_getname", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_GetName (IntPtr Handle, UInt32 sizeName, out UInt32 neededName, IntPtr dataName);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_getmimetype", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_GetMimeType (IntPtr Handle, UInt32 sizeMimeType, out UInt32 neededMimeType, IntPtr dataMimeType);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_getusagecontext", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_GetUsageContext (IntPtr Handle, UInt32 sizeUsageContext, out UInt32 neededUsageContext, IntPtr dataUsageContext);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_uploaddata", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_UploadData (IntPtr Handle, UInt64 sizeData, IntPtr dataData, UInt32 AChunkSize, out IntPtr ASuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_uploadfile", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_UploadFile (IntPtr Handle, byte[] AFileName, UInt32 AChunkSize, out IntPtr ASuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_beginchunking", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_BeginChunking (IntPtr Handle, UInt64 ADataSize, out IntPtr ASuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_uploadchunk", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_UploadChunk (IntPtr Handle, UInt64 sizeData, IntPtr dataData, out IntPtr ASuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_finishchunking", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_FinishChunking (IntPtr Handle, out IntPtr ASuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_getstatus", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_GetStatus (IntPtr Handle, out UInt64 AUploadSize, out UInt64 AFinishedSize, out UInt64 AInProgressSize, out Byte AFinished);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_streamupload_getdatastream", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 StreamUpload_GetDataStream (IntPtr Handle, out IntPtr ADataStream);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_getbaseurl", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_GetBaseURL (IntPtr Handle, UInt32 sizeBaseURL, out UInt32 neededBaseURL, IntPtr dataBaseURL);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_settimeouts", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_SetTimeouts (IntPtr Handle, UInt32 ATimeout, UInt32 ARetryCount);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_gettimeout", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_GetTimeout (IntPtr Handle, out UInt32 ATimeout);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_getretrycount", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_GetRetryCount (IntPtr Handle, out UInt32 ARetryCount);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_authenticatewithpassword", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_AuthenticateWithPassword (IntPtr Handle, byte[] AUserName, byte[] APassword, out IntPtr ASuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_isauthenticated", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_IsAuthenticated (IntPtr Handle, out Byte AIsAuthenticated);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_refreshauthentication", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_RefreshAuthentication (IntPtr Handle, out IntPtr ASuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_ping", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_Ping (IntPtr Handle, out IntPtr ASuccess);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_getauthtoken", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_GetAuthToken (IntPtr Handle, UInt32 sizeToken, out UInt32 neededToken, IntPtr dataToken);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_connection_createupload", CallingConvention=CallingConvention.Cdecl)]
			public unsafe extern static Int32 Connection_CreateUpload (IntPtr Handle, byte[] AName, byte[] AMimeType, byte[] AUsageContext, out IntPtr AInstance);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_getversion", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)]
			public extern static Int32 GetVersion (out UInt32 AMajor, out UInt32 AMinor, out UInt32 AMicro);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_getlasterror", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)]
			public extern static Int32 GetLastError (IntPtr AInstance, UInt32 sizeErrorMessage, out UInt32 neededErrorMessage, IntPtr dataErrorMessage, out Byte AHasError);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_releaseinstance", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)]
			public extern static Int32 ReleaseInstance (IntPtr AInstance);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_acquireinstance", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)]
			public extern static Int32 AcquireInstance (IntPtr AInstance);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_injectcomponent", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)]
			public extern static Int32 InjectComponent (byte[] ANameSpace, UInt64 ASymbolAddressMethod);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_getsymbollookupmethod", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)]
			public extern static Int32 GetSymbolLookupMethod (out UInt64 ASymbolLookupMethod);

			[DllImport("libamcf.dll", EntryPoint = "libamcf_createconnection", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)]
			public extern static Int32 CreateConnection (byte[] ABaseURL, out IntPtr AInstance);

			public static void ThrowError(IntPtr Handle, Int32 errorCode)
			{
				String sMessage = "LibAMCF Error";
				if (Handle != IntPtr.Zero) {
					UInt32 sizeMessage = 0;
					UInt32 neededMessage = 0;
					Byte hasLastError = 0;
					Int32 resultCode1 = GetLastError (Handle, sizeMessage, out neededMessage, IntPtr.Zero, out hasLastError);
					if ((resultCode1 == 0) && (hasLastError != 0)) {
						sizeMessage = neededMessage;
						byte[] bytesMessage = new byte[sizeMessage];

						GCHandle dataMessage = GCHandle.Alloc(bytesMessage, GCHandleType.Pinned);
						Int32 resultCode2 = GetLastError(Handle, sizeMessage, out neededMessage, dataMessage.AddrOfPinnedObject(), out hasLastError);
						dataMessage.Free();

						if ((resultCode2 == 0) && (hasLastError != 0)) {
							sMessage = sMessage + ": " + Encoding.UTF8.GetString(bytesMessage).TrimEnd(char.MinValue);
						}
					}
				}

				throw new Exception(sMessage + "(# " + errorCode + ")");
			}

		}
	}


	class CBase 
	{
		protected IntPtr Handle;

		public CBase (IntPtr NewHandle)
		{
			Handle = NewHandle;
		}

		~CBase ()
		{
			if (Handle != IntPtr.Zero) {
				Internal.LibAMCFWrapper.ReleaseInstance (Handle);
				Handle = IntPtr.Zero;
			}
		}

		protected void CheckError (Int32 errorCode)
		{
			if (errorCode != 0) {
				Internal.LibAMCFWrapper.ThrowError (Handle, errorCode);
			}
		}

		public IntPtr GetHandle ()
		{
			return Handle;
		}

	}

	class COperationResult : CBase
	{
		public COperationResult (IntPtr NewHandle) : base (NewHandle)
		{
		}

		public bool WaitFor (UInt32 ATimeOut)
		{
			Byte resultOperationFinished = 0;

			CheckError(Internal.LibAMCFWrapper.OperationResult_WaitFor (Handle, ATimeOut, out resultOperationFinished));
			return (resultOperationFinished != 0);
		}

		public void EnsureSuccess ()
		{

			CheckError(Internal.LibAMCFWrapper.OperationResult_EnsureSuccess (Handle));
		}

		public bool InProgress ()
		{
			Byte resultOperationIsInProgress = 0;

			CheckError(Internal.LibAMCFWrapper.OperationResult_InProgress (Handle, out resultOperationIsInProgress));
			return (resultOperationIsInProgress != 0);
		}

		public bool Success ()
		{
			Byte resultOperationSuccess = 0;

			CheckError(Internal.LibAMCFWrapper.OperationResult_Success (Handle, out resultOperationSuccess));
			return (resultOperationSuccess != 0);
		}

		public String GetErrorMessage ()
		{
			UInt32 sizeErrorMessage = 0;
			UInt32 neededErrorMessage = 0;
			CheckError(Internal.LibAMCFWrapper.OperationResult_GetErrorMessage (Handle, sizeErrorMessage, out neededErrorMessage, IntPtr.Zero));
			sizeErrorMessage = neededErrorMessage;
			byte[] bytesErrorMessage = new byte[sizeErrorMessage];
			GCHandle dataErrorMessage = GCHandle.Alloc(bytesErrorMessage, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.OperationResult_GetErrorMessage (Handle, sizeErrorMessage, out neededErrorMessage, dataErrorMessage.AddrOfPinnedObject()));
			dataErrorMessage.Free();
			return Encoding.UTF8.GetString(bytesErrorMessage).TrimEnd(char.MinValue);
		}

	}

	class CDataStream : CBase
	{
		public CDataStream (IntPtr NewHandle) : base (NewHandle)
		{
		}

		public String GetUUID ()
		{
			UInt32 sizeUUID = 0;
			UInt32 neededUUID = 0;
			CheckError(Internal.LibAMCFWrapper.DataStream_GetUUID (Handle, sizeUUID, out neededUUID, IntPtr.Zero));
			sizeUUID = neededUUID;
			byte[] bytesUUID = new byte[sizeUUID];
			GCHandle dataUUID = GCHandle.Alloc(bytesUUID, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.DataStream_GetUUID (Handle, sizeUUID, out neededUUID, dataUUID.AddrOfPinnedObject()));
			dataUUID.Free();
			return Encoding.UTF8.GetString(bytesUUID).TrimEnd(char.MinValue);
		}

		public String GetContextUUID ()
		{
			UInt32 sizeContextUUID = 0;
			UInt32 neededContextUUID = 0;
			CheckError(Internal.LibAMCFWrapper.DataStream_GetContextUUID (Handle, sizeContextUUID, out neededContextUUID, IntPtr.Zero));
			sizeContextUUID = neededContextUUID;
			byte[] bytesContextUUID = new byte[sizeContextUUID];
			GCHandle dataContextUUID = GCHandle.Alloc(bytesContextUUID, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.DataStream_GetContextUUID (Handle, sizeContextUUID, out neededContextUUID, dataContextUUID.AddrOfPinnedObject()));
			dataContextUUID.Free();
			return Encoding.UTF8.GetString(bytesContextUUID).TrimEnd(char.MinValue);
		}

		public String GetName ()
		{
			UInt32 sizeName = 0;
			UInt32 neededName = 0;
			CheckError(Internal.LibAMCFWrapper.DataStream_GetName (Handle, sizeName, out neededName, IntPtr.Zero));
			sizeName = neededName;
			byte[] bytesName = new byte[sizeName];
			GCHandle dataName = GCHandle.Alloc(bytesName, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.DataStream_GetName (Handle, sizeName, out neededName, dataName.AddrOfPinnedObject()));
			dataName.Free();
			return Encoding.UTF8.GetString(bytesName).TrimEnd(char.MinValue);
		}

		public String GetMimeType ()
		{
			UInt32 sizeMimeType = 0;
			UInt32 neededMimeType = 0;
			CheckError(Internal.LibAMCFWrapper.DataStream_GetMimeType (Handle, sizeMimeType, out neededMimeType, IntPtr.Zero));
			sizeMimeType = neededMimeType;
			byte[] bytesMimeType = new byte[sizeMimeType];
			GCHandle dataMimeType = GCHandle.Alloc(bytesMimeType, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.DataStream_GetMimeType (Handle, sizeMimeType, out neededMimeType, dataMimeType.AddrOfPinnedObject()));
			dataMimeType.Free();
			return Encoding.UTF8.GetString(bytesMimeType).TrimEnd(char.MinValue);
		}

		public String GetSHA256 ()
		{
			UInt32 sizeSHA256 = 0;
			UInt32 neededSHA256 = 0;
			CheckError(Internal.LibAMCFWrapper.DataStream_GetSHA256 (Handle, sizeSHA256, out neededSHA256, IntPtr.Zero));
			sizeSHA256 = neededSHA256;
			byte[] bytesSHA256 = new byte[sizeSHA256];
			GCHandle dataSHA256 = GCHandle.Alloc(bytesSHA256, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.DataStream_GetSHA256 (Handle, sizeSHA256, out neededSHA256, dataSHA256.AddrOfPinnedObject()));
			dataSHA256.Free();
			return Encoding.UTF8.GetString(bytesSHA256).TrimEnd(char.MinValue);
		}

		public UInt64 GetSize ()
		{
			UInt64 resultStreamSize = 0;

			CheckError(Internal.LibAMCFWrapper.DataStream_GetSize (Handle, out resultStreamSize));
			return resultStreamSize;
		}

		public String GetTimestamp ()
		{
			UInt32 sizeTimestamp = 0;
			UInt32 neededTimestamp = 0;
			CheckError(Internal.LibAMCFWrapper.DataStream_GetTimestamp (Handle, sizeTimestamp, out neededTimestamp, IntPtr.Zero));
			sizeTimestamp = neededTimestamp;
			byte[] bytesTimestamp = new byte[sizeTimestamp];
			GCHandle dataTimestamp = GCHandle.Alloc(bytesTimestamp, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.DataStream_GetTimestamp (Handle, sizeTimestamp, out neededTimestamp, dataTimestamp.AddrOfPinnedObject()));
			dataTimestamp.Free();
			return Encoding.UTF8.GetString(bytesTimestamp).TrimEnd(char.MinValue);
		}

	}

	class CStreamUpload : CBase
	{
		public CStreamUpload (IntPtr NewHandle) : base (NewHandle)
		{
		}

		public String GetName ()
		{
			UInt32 sizeName = 0;
			UInt32 neededName = 0;
			CheckError(Internal.LibAMCFWrapper.StreamUpload_GetName (Handle, sizeName, out neededName, IntPtr.Zero));
			sizeName = neededName;
			byte[] bytesName = new byte[sizeName];
			GCHandle dataName = GCHandle.Alloc(bytesName, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.StreamUpload_GetName (Handle, sizeName, out neededName, dataName.AddrOfPinnedObject()));
			dataName.Free();
			return Encoding.UTF8.GetString(bytesName).TrimEnd(char.MinValue);
		}

		public String GetMimeType ()
		{
			UInt32 sizeMimeType = 0;
			UInt32 neededMimeType = 0;
			CheckError(Internal.LibAMCFWrapper.StreamUpload_GetMimeType (Handle, sizeMimeType, out neededMimeType, IntPtr.Zero));
			sizeMimeType = neededMimeType;
			byte[] bytesMimeType = new byte[sizeMimeType];
			GCHandle dataMimeType = GCHandle.Alloc(bytesMimeType, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.StreamUpload_GetMimeType (Handle, sizeMimeType, out neededMimeType, dataMimeType.AddrOfPinnedObject()));
			dataMimeType.Free();
			return Encoding.UTF8.GetString(bytesMimeType).TrimEnd(char.MinValue);
		}

		public String GetUsageContext ()
		{
			UInt32 sizeUsageContext = 0;
			UInt32 neededUsageContext = 0;
			CheckError(Internal.LibAMCFWrapper.StreamUpload_GetUsageContext (Handle, sizeUsageContext, out neededUsageContext, IntPtr.Zero));
			sizeUsageContext = neededUsageContext;
			byte[] bytesUsageContext = new byte[sizeUsageContext];
			GCHandle dataUsageContext = GCHandle.Alloc(bytesUsageContext, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.StreamUpload_GetUsageContext (Handle, sizeUsageContext, out neededUsageContext, dataUsageContext.AddrOfPinnedObject()));
			dataUsageContext.Free();
			return Encoding.UTF8.GetString(bytesUsageContext).TrimEnd(char.MinValue);
		}

		public COperationResult UploadData (Byte[] AData, UInt32 AChunkSize)
		{
			GCHandle dataData = GCHandle.Alloc(AData, GCHandleType.Pinned);
			IntPtr newSuccess = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.StreamUpload_UploadData (Handle, (UInt64) AData.Length, dataData.AddrOfPinnedObject(), AChunkSize, out newSuccess));
			dataData.Free ();
			return new COperationResult (newSuccess );
		}

		public COperationResult UploadFile (String AFileName, UInt32 AChunkSize)
		{
			byte[] byteFileName = Encoding.UTF8.GetBytes(AFileName + char.MinValue);
			IntPtr newSuccess = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.StreamUpload_UploadFile (Handle, byteFileName, AChunkSize, out newSuccess));
			return new COperationResult (newSuccess );
		}

		public COperationResult BeginChunking (UInt64 ADataSize)
		{
			IntPtr newSuccess = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.StreamUpload_BeginChunking (Handle, ADataSize, out newSuccess));
			return new COperationResult (newSuccess );
		}

		public COperationResult UploadChunk (Byte[] AData)
		{
			GCHandle dataData = GCHandle.Alloc(AData, GCHandleType.Pinned);
			IntPtr newSuccess = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.StreamUpload_UploadChunk (Handle, (UInt64) AData.Length, dataData.AddrOfPinnedObject(), out newSuccess));
			dataData.Free ();
			return new COperationResult (newSuccess );
		}

		public COperationResult FinishChunking ()
		{
			IntPtr newSuccess = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.StreamUpload_FinishChunking (Handle, out newSuccess));
			return new COperationResult (newSuccess );
		}

		public void GetStatus (out UInt64 AUploadSize, out UInt64 AFinishedSize, out UInt64 AInProgressSize, out bool AFinished)
		{
			Byte resultFinished = 0;

			CheckError(Internal.LibAMCFWrapper.StreamUpload_GetStatus (Handle, out AUploadSize, out AFinishedSize, out AInProgressSize, out resultFinished));
			AFinished = (resultFinished != 0);
		}

		public CDataStream GetDataStream ()
		{
			IntPtr newDataStream = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.StreamUpload_GetDataStream (Handle, out newDataStream));
			return new CDataStream (newDataStream );
		}

	}

	class CConnection : CBase
	{
		public CConnection (IntPtr NewHandle) : base (NewHandle)
		{
		}

		public String GetBaseURL ()
		{
			UInt32 sizeBaseURL = 0;
			UInt32 neededBaseURL = 0;
			CheckError(Internal.LibAMCFWrapper.Connection_GetBaseURL (Handle, sizeBaseURL, out neededBaseURL, IntPtr.Zero));
			sizeBaseURL = neededBaseURL;
			byte[] bytesBaseURL = new byte[sizeBaseURL];
			GCHandle dataBaseURL = GCHandle.Alloc(bytesBaseURL, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.Connection_GetBaseURL (Handle, sizeBaseURL, out neededBaseURL, dataBaseURL.AddrOfPinnedObject()));
			dataBaseURL.Free();
			return Encoding.UTF8.GetString(bytesBaseURL).TrimEnd(char.MinValue);
		}

		public void SetTimeouts (UInt32 ATimeout, UInt32 ARetryCount)
		{

			CheckError(Internal.LibAMCFWrapper.Connection_SetTimeouts (Handle, ATimeout, ARetryCount));
		}

		public UInt32 GetTimeout ()
		{
			UInt32 resultTimeout = 0;

			CheckError(Internal.LibAMCFWrapper.Connection_GetTimeout (Handle, out resultTimeout));
			return resultTimeout;
		}

		public UInt32 GetRetryCount ()
		{
			UInt32 resultRetryCount = 0;

			CheckError(Internal.LibAMCFWrapper.Connection_GetRetryCount (Handle, out resultRetryCount));
			return resultRetryCount;
		}

		public COperationResult AuthenticateWithPassword (String AUserName, String APassword)
		{
			byte[] byteUserName = Encoding.UTF8.GetBytes(AUserName + char.MinValue);
			byte[] bytePassword = Encoding.UTF8.GetBytes(APassword + char.MinValue);
			IntPtr newSuccess = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.Connection_AuthenticateWithPassword (Handle, byteUserName, bytePassword, out newSuccess));
			return new COperationResult (newSuccess );
		}

		public bool IsAuthenticated ()
		{
			Byte resultIsAuthenticated = 0;

			CheckError(Internal.LibAMCFWrapper.Connection_IsAuthenticated (Handle, out resultIsAuthenticated));
			return (resultIsAuthenticated != 0);
		}

		public COperationResult RefreshAuthentication ()
		{
			IntPtr newSuccess = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.Connection_RefreshAuthentication (Handle, out newSuccess));
			return new COperationResult (newSuccess );
		}

		public COperationResult Ping ()
		{
			IntPtr newSuccess = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.Connection_Ping (Handle, out newSuccess));
			return new COperationResult (newSuccess );
		}

		public String GetAuthToken ()
		{
			UInt32 sizeToken = 0;
			UInt32 neededToken = 0;
			CheckError(Internal.LibAMCFWrapper.Connection_GetAuthToken (Handle, sizeToken, out neededToken, IntPtr.Zero));
			sizeToken = neededToken;
			byte[] bytesToken = new byte[sizeToken];
			GCHandle dataToken = GCHandle.Alloc(bytesToken, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.Connection_GetAuthToken (Handle, sizeToken, out neededToken, dataToken.AddrOfPinnedObject()));
			dataToken.Free();
			return Encoding.UTF8.GetString(bytesToken).TrimEnd(char.MinValue);
		}

		public CStreamUpload CreateUpload (String AName, String AMimeType, String AUsageContext)
		{
			byte[] byteName = Encoding.UTF8.GetBytes(AName + char.MinValue);
			byte[] byteMimeType = Encoding.UTF8.GetBytes(AMimeType + char.MinValue);
			byte[] byteUsageContext = Encoding.UTF8.GetBytes(AUsageContext + char.MinValue);
			IntPtr newInstance = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.Connection_CreateUpload (Handle, byteName, byteMimeType, byteUsageContext, out newInstance));
			return new CStreamUpload (newInstance );
		}

	}

	class Wrapper
	{
		private static void CheckError (Int32 errorCode)
		{
			if (errorCode != 0) {
				Internal.LibAMCFWrapper.ThrowError (IntPtr.Zero, errorCode);
			}
		}

		public static void GetVersion (out UInt32 AMajor, out UInt32 AMinor, out UInt32 AMicro)
		{

			CheckError(Internal.LibAMCFWrapper.GetVersion (out AMajor, out AMinor, out AMicro));
		}

		public static bool GetLastError (CBase AInstance, out String AErrorMessage)
		{
			Byte resultHasError = 0;
			UInt32 sizeErrorMessage = 0;
			UInt32 neededErrorMessage = 0;
			CheckError(Internal.LibAMCFWrapper.GetLastError (AInstance.GetHandle(), sizeErrorMessage, out neededErrorMessage, IntPtr.Zero, out resultHasError));
			sizeErrorMessage = neededErrorMessage;
			byte[] bytesErrorMessage = new byte[sizeErrorMessage];
			GCHandle dataErrorMessage = GCHandle.Alloc(bytesErrorMessage, GCHandleType.Pinned);

			CheckError(Internal.LibAMCFWrapper.GetLastError (AInstance.GetHandle(), sizeErrorMessage, out neededErrorMessage, dataErrorMessage.AddrOfPinnedObject(), out resultHasError));
			dataErrorMessage.Free();
			AErrorMessage = Encoding.UTF8.GetString(bytesErrorMessage).TrimEnd(char.MinValue);
			return (resultHasError != 0);
		}

		public static void ReleaseInstance (CBase AInstance)
		{

			CheckError(Internal.LibAMCFWrapper.ReleaseInstance (AInstance.GetHandle()));
		}

		public static void AcquireInstance (CBase AInstance)
		{

			CheckError(Internal.LibAMCFWrapper.AcquireInstance (AInstance.GetHandle()));
		}

		public static void InjectComponent (String ANameSpace, UInt64 ASymbolAddressMethod)
		{
		throw new Exception("Component injection is not supported in CSharp.");
		}

		public static UInt64 GetSymbolLookupMethod ()
		{
			UInt64 resultSymbolLookupMethod = 0;

			CheckError(Internal.LibAMCFWrapper.GetSymbolLookupMethod (out resultSymbolLookupMethod));
			return resultSymbolLookupMethod;
		}

		public static CConnection CreateConnection (String ABaseURL)
		{
			byte[] byteBaseURL = Encoding.UTF8.GetBytes(ABaseURL + char.MinValue);
			IntPtr newInstance = IntPtr.Zero;

			CheckError(Internal.LibAMCFWrapper.CreateConnection (byteBaseURL, out newInstance));
			return new CConnection (newInstance );
		}

	}

}
