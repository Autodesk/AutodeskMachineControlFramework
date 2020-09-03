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


using System;
using System.Runtime.InteropServices;
using S7.Net;

namespace LibS7Net
{

	/*************************************************************************************************************************
	 Class implementation of CCalculator 
	**************************************************************************************************************************/

	[ComVisible(true)]
	[Guid(ContractGuids.CLSID_IPLC)]
	public class CPLC : CBase, IPLC
	{

		Plc ? S7;
		UInt32 LastErrorCode;
		String LastErrorMessage;
		
		public CPLC ()			
        {
			LastErrorCode = 0;
			LastErrorMessage = "";
		}


		void IPLC.Connect(UInt32 CPUType, String IP, UInt32 Rack, UInt32 Slot, UInt32 ReadTimeOut, UInt32 WriteTimeOut)
		{

			S7 = null;
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{

				S7 = new Plc((CpuType)CPUType, IP, (short)Rack, (short)Slot);

				S7.ReadTimeout = (int)ReadTimeOut;
				S7.WriteTimeout = (int)WriteTimeOut;

				S7.Open();

			} 
			catch (PlcException EPLC)
            {
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32) EPLC.ErrorCode;

				throw EPLC;

			}
			catch (Exception E)
            {
				LastErrorMessage = E.ToString ();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;

				throw E;
            }


		}

		void IPLC.Disconnect()
        {
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{
				S7 = null;

				if (S7 != null)
				{
					S7.Close();
					S7 = null;
				}
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;

				throw EPLC;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;

				throw E;
			}
		}


		void IPLC.WriteBytes(UInt32 DB, UInt32 Offset, [In, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UI1)] byte[] bArray)
		{
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{
				if (S7 != null)
				{
					S7.WriteBytes(DataType.DataBlock, (int)DB, (int)Offset, bArray);
				}
				else
				{
					throw new LibS7Exception(ErrorCodes.NOTCONNECTED);

				}
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;
				throw EPLC;

			}
			catch (LibS7Exception ES7)
			{
				LastErrorMessage = ES7.ToString();
				LastErrorCode = ES7.ErrorCode;
				throw ES7;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;
				throw E;
			}

		}

		IPLCReadData IPLC.ReadBytes(UInt32 DB, UInt32 Offset, UInt32 Count)		
		{

			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{

				if (S7 != null)
				{
					byte[] Bytes = S7.ReadBytes(DataType.DataBlock, (int)DB, (int)Offset, (int)Count);
					return new CPLCReadData(Bytes);
				}
				else
				{
					throw new LibS7Exception(ErrorCodes.NOTCONNECTED);

				}
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;
				throw EPLC;
			}
			catch (LibS7Exception ES7)
			{
				LastErrorMessage = ES7.ToString();
				LastErrorCode = ES7.ErrorCode;
				throw ES7;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;
				throw E;
			}


		}


		void IPLC.GetLastError(out UInt32 outLastErrorCode, out String outLastErrorMessage)
        {
			outLastErrorCode = LastErrorCode;
			outLastErrorMessage = LastErrorMessage;
		}

		bool IPLC.IsConnected()
        {
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{
				if (S7 != null)
				{
					return S7.IsConnected;
				}

				return false;
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;
				throw EPLC;
			}
			catch (LibS7Exception ES7)
			{
				LastErrorMessage = ES7.ToString();
				LastErrorCode = ES7.ErrorCode;
				throw ES7;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;
				throw E;
			}

		}

		bool IPLC.IsAvailable()
        {
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{
				if (S7 != null)
				{
					return S7.IsConnected;
				}

				return false;
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;
				throw EPLC;
			}
			catch (LibS7Exception ES7)
			{
				LastErrorMessage = ES7.ToString();
				LastErrorCode = ES7.ErrorCode;
				throw ES7;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;
				throw E;
			}

		}

		UInt32 IPLC.GetSlot()
		{
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{
				if (S7 != null)
				{
					return (UInt32)S7.Slot;
				}

				throw new LibS7Exception(ErrorCodes.NOTCONNECTED);
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;
				throw EPLC;
			}
			catch (LibS7Exception ES7)
			{
				LastErrorMessage = ES7.ToString();
				LastErrorCode = ES7.ErrorCode;
				throw ES7;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;
				throw E;
			}

		}

		UInt32 IPLC.GetRack()
		{
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{
				if (S7 != null)
				{
					return (UInt32)S7.Rack;
				}

				throw new LibS7Exception(ErrorCodes.NOTCONNECTED);
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;
				throw EPLC;
			}
			catch (LibS7Exception ES7)
			{
				LastErrorMessage = ES7.ToString();
				LastErrorCode = ES7.ErrorCode;
				throw ES7;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;
				throw E;
			}

		}

		UInt32 IPLC.GetCPUType()
        {
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{
				if (S7 != null)
				{
					return (UInt32)S7.CPU;
				}

				throw new LibS7Exception(ErrorCodes.NOTCONNECTED);
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;
				throw EPLC;
			}
			catch (LibS7Exception ES7)
			{
				LastErrorMessage = ES7.ToString();
				LastErrorCode = ES7.ErrorCode;
				throw ES7;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;
				throw E;
			}
		}

		String IPLC.GetIPAddress()
        {
			LastErrorCode = 0;
			LastErrorMessage = "";

			try
			{
				if (S7 != null)
				{
					return S7.IP;
				}

				throw new LibS7Exception(ErrorCodes.NOTCONNECTED);
			}
			catch (PlcException EPLC)
			{
				LastErrorMessage = EPLC.ToString();
				LastErrorCode = (UInt32)EPLC.ErrorCode;
				throw EPLC;
			}
			catch (LibS7Exception ES7)
			{
				LastErrorMessage = ES7.ToString();
				LastErrorCode = ES7.ErrorCode;
				throw ES7;
			}
			catch (Exception E)
			{
				LastErrorMessage = E.ToString();
				LastErrorCode = ErrorCodes.GENERICEXCEPTION;
				throw E;
			}
		}


	}

}

