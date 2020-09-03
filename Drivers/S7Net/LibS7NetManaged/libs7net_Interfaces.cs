/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the Autodesk Inc. nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


using S7.Net;
using System;
using System.Runtime.InteropServices;

namespace LibS7Net
{

	/*************************************************************************************************************************
	 Error Codes and Exception definition of LibS7Net
	**************************************************************************************************************************/

	public class ErrorCodes {
		public const UInt32 NOTIMPLEMENTED = 1001;
		public const UInt32 INVALIDPARAM = 1002;
		public const UInt32 INVALIDCAST = 1003;
		public const UInt32 BUFFERTOOSMALL = 1004;
		public const UInt32 GENERICEXCEPTION = 1005;
		public const UInt32 COULDNOTLOADLIBRARY = 1006;
		public const UInt32 COULDNOTFINDLIBRARYEXPORT = 1007;
		public const UInt32 INCOMPATIBLEBINARYVERSION = 1008;
		public const UInt32 NOTCONNECTED = 1009;
	}

	[Serializable()]
	public class LibS7Exception : System.Exception
	{
		public UInt32 ErrorCode;

		public static string ErrorCodeToString (UInt32 errorCode)
		{
			switch (errorCode)
			{
				case ErrorCodes.NOTIMPLEMENTED: return "functionality not implemented";
				case ErrorCodes.INVALIDPARAM: return "an invalid parameter was passed";
				case ErrorCodes.INVALIDCAST: return "a type cast failed";
				case ErrorCodes.BUFFERTOOSMALL: return "a provided buffer is too small";
				case ErrorCodes.GENERICEXCEPTION: return "a generic exception occurred";
				case ErrorCodes.COULDNOTLOADLIBRARY: return "the library could not be loaded";
				case ErrorCodes.COULDNOTFINDLIBRARYEXPORT: return "a required exported symbol could not be found in the library";
				case ErrorCodes.INCOMPATIBLEBINARYVERSION: return "the version of the binary interface does not match the bindings interface";
				case ErrorCodes.NOTCONNECTED: return "PLC is not connected";
				default: return String.Format("Unknown Error #{0}", errorCode);
			}
		}
		
		public LibS7Exception() : base() { ErrorCode = ErrorCodes.GENERICEXCEPTION; }
		public LibS7Exception(UInt32 errorCode) : base(ErrorCodeToString (errorCode)) { ErrorCode = errorCode; }
		public LibS7Exception(string message) : base(message) { ErrorCode = ErrorCodes.GENERICEXCEPTION;  }
		public LibS7Exception(string message, System.Exception inner) : base(message, inner) { ErrorCode = ErrorCodes.GENERICEXCEPTION; }
		protected LibS7Exception(System.Runtime.Serialization.SerializationInfo info,
			 System.Runtime.Serialization.StreamingContext context) : base(info, context) { ErrorCode = ErrorCodes.GENERICEXCEPTION; }
	}

	/*************************************************************************************************************************
	 COM GUID definitions of LibPrimes 
	**************************************************************************************************************************/

	internal sealed class ContractGuids
	{
		public const string CLSID_IBase = "1C0B49DC-9BE4-4157-97B4-79225AAD7907";
		public const string IID_IBase = "C27EFFB2-951B-4AF9-BB47-FAEABBDE5598";
		public const string CLSID_IPLC = "0CDEECF2-4252-4A1A-AE01-C98A9B02C78F";
		public const string IID_IPLC = "C7ADB662-C099-41C1-831D-0F11BAA29640";
		public const string CLSID_IPLCReadData = "CB767BA1-E821-4849-8B81-9852F5053384";
		public const string IID_IPLCReadData = "1C670D16-177D-44B2-B9E5-B264520178D6";
		public const string CLSID_IWrapper = "EB056EA7-29D5-480E-A351-C95A544DCC95";
		public const string IID_IWrapper = "10C868F6-6FCA-47FB-B7AA-0160193BA204";
	}


	/*************************************************************************************************************************
	 COM interface of IBase 
	**************************************************************************************************************************/

	[ComVisible(true)]
	[Guid(ContractGuids.IID_IBase)]
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IBase 
	{

	}


	/*************************************************************************************************************************
	COM interface of IPLCReadData 
	**************************************************************************************************************************/

	[ComVisible(true)]
	[Guid(ContractGuids.IID_IPLCReadData)]
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IPLCReadData : IBase
    {
		public UInt32 GetSize();

		[return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UI1)]
		public byte[] GetData();


	}

	/*************************************************************************************************************************
	 COM interface of IPLC 
	**************************************************************************************************************************/

	[ComVisible(true)]
	[Guid(ContractGuids.IID_IPLC)]
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IPLC : IBase
	{

		public void Connect (UInt32 CPUType, String IP, UInt32 Rack, UInt32 Slot, UInt32 ReadTimeOut, UInt32 WriteTimeOut);

		public void Disconnect();

		public void GetLastError(out UInt32 outLastErrorCode, out String outLastErrorMessage);
		
		public void WriteBytes (UInt32 DB, UInt32 Offset, [In, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UI1)] byte[] bArray);

		public IPLCReadData ReadBytes(UInt32 DB, UInt32 Offset, UInt32 Count);

		public bool IsConnected();

		public bool IsAvailable();

		public UInt32 GetSlot();

		public UInt32 GetRack();

		public UInt32 GetCPUType();

		public String GetIPAddress();
	}


	/*************************************************************************************************************************
	 COM interface of IWrapper 
	**************************************************************************************************************************/

	[ComVisible(true)]
	[Guid(ContractGuids.IID_IWrapper)]
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IWrapper
	{
		public IPLC CreatePLC();
	}

}

