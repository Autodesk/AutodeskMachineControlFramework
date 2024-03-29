/*++

Copyright (C) 2023 Autodesk Inc.

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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


This file has been generated by the Automatic Component Toolkit (ACT) version 1.8.0-develop.

Abstract: This is an autogenerated C++-Header file with basic types in
order to allow an easy use of MC Driver cifX

Interface version: 2.0.0

*/

#ifndef __LIBMCDRIVER_CIFX_TYPES_HEADER_CPP
#define __LIBMCDRIVER_CIFX_TYPES_HEADER_CPP


/*************************************************************************************************************************
 Scalar types definition
**************************************************************************************************************************/

#ifdef LIBMCDRIVER_CIFX_USELEGACYINTEGERTYPES

typedef unsigned char LibMCDriver_CifX_uint8;
typedef unsigned short LibMCDriver_CifX_uint16 ;
typedef unsigned int LibMCDriver_CifX_uint32;
typedef unsigned long long LibMCDriver_CifX_uint64;
typedef char LibMCDriver_CifX_int8;
typedef short LibMCDriver_CifX_int16;
typedef int LibMCDriver_CifX_int32;
typedef long long LibMCDriver_CifX_int64;

#else // LIBMCDRIVER_CIFX_USELEGACYINTEGERTYPES

#include <stdint.h>

typedef uint8_t LibMCDriver_CifX_uint8;
typedef uint16_t LibMCDriver_CifX_uint16;
typedef uint32_t LibMCDriver_CifX_uint32;
typedef uint64_t LibMCDriver_CifX_uint64;
typedef int8_t LibMCDriver_CifX_int8;
typedef int16_t LibMCDriver_CifX_int16;
typedef int32_t LibMCDriver_CifX_int32;
typedef int64_t LibMCDriver_CifX_int64 ;

#endif // LIBMCDRIVER_CIFX_USELEGACYINTEGERTYPES

typedef float LibMCDriver_CifX_single;
typedef double LibMCDriver_CifX_double;

/*************************************************************************************************************************
 General type definitions
**************************************************************************************************************************/

typedef LibMCDriver_CifX_int32 LibMCDriver_CifXResult;
typedef void * LibMCDriver_CifXHandle;
typedef void * LibMCDriver_CifX_pvoid;

/*************************************************************************************************************************
 Version for LibMCDriver_CifX
**************************************************************************************************************************/

#define LIBMCDRIVER_CIFX_VERSION_MAJOR 2
#define LIBMCDRIVER_CIFX_VERSION_MINOR 0
#define LIBMCDRIVER_CIFX_VERSION_MICRO 0
#define LIBMCDRIVER_CIFX_VERSION_PRERELEASEINFO ""
#define LIBMCDRIVER_CIFX_VERSION_BUILDINFO ""

/*************************************************************************************************************************
 Error constants for LibMCDriver_CifX
**************************************************************************************************************************/

#define LIBMCDRIVER_CIFX_SUCCESS 0
#define LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED 1 /** functionality not implemented */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM 2 /** an invalid parameter was passed */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDCAST 3 /** a type cast failed */
#define LIBMCDRIVER_CIFX_ERROR_BUFFERTOOSMALL 4 /** a provided buffer is too small */
#define LIBMCDRIVER_CIFX_ERROR_GENERICEXCEPTION 5 /** a generic exception occurred */
#define LIBMCDRIVER_CIFX_ERROR_COULDNOTLOADLIBRARY 6 /** the library could not be loaded */
#define LIBMCDRIVER_CIFX_ERROR_COULDNOTFINDLIBRARYEXPORT 7 /** a required exported symbol could not be found in the library */
#define LIBMCDRIVER_CIFX_ERROR_INCOMPATIBLEBINARYVERSION 8 /** the version of the binary interface does not match the bindings interface */
#define LIBMCDRIVER_CIFX_ERROR_DRIVERERROR 1000 /** a driver error occured */
#define LIBMCDRIVER_CIFX_ERROR_DRIVERNOTINITIALISED 1001 /** the driver is not initialised */
#define LIBMCDRIVER_CIFX_ERROR_DRIVERNOTCONNECTED 1002 /** the driver is not connected */
#define LIBMCDRIVER_CIFX_ERROR_DRIVERTYPENOTFOUND 1003 /** the driver type has not been found */
#define LIBMCDRIVER_CIFX_ERROR_SDKALREADYLOADED 1004 /** SDK already loaded */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDSDKRESOURCE 1005 /** invalid SDK Resource */
#define LIBMCDRIVER_CIFX_ERROR_OUTPUTVALUENOTFOUND 1006 /** output value not found */
#define LIBMCDRIVER_CIFX_ERROR_INPUTVALUENOTFOUND 1007 /** input value not found */
#define LIBMCDRIVER_CIFX_ERROR_VALUENOTFOUND 1008 /** value not found */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDDRIVERPROTOCOL 1009 /** invalid driver protocol */
#define LIBMCDRIVER_CIFX_ERROR_COULDNOTPARSEDRIVERPROTOCOL 1010 /** could not parse driver protocol */
#define LIBMCDRIVER_CIFX_ERROR_NOVERSIONDEFINITION 1011 /** no version definition */
#define LIBMCDRIVER_CIFX_ERROR_NOMAJORVERSION 1012 /** no major version */
#define LIBMCDRIVER_CIFX_ERROR_NOMINORVERSION 1013 /** no minor version */
#define LIBMCDRIVER_CIFX_ERROR_NOPATCHVERSION 1014 /** no patch version */
#define LIBMCDRIVER_CIFX_ERROR_NOBOARDATTRIBUTE 1015 /** no board attribute */
#define LIBMCDRIVER_CIFX_ERROR_NOCHANNELINDEXATTRIBUTE 1016 /** no channel index attribute */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELINDEXATTRIBUTE 1017 /** invalid channel index attribute */
#define LIBMCDRIVER_CIFX_ERROR_NOINPUTIOSIZEATTRIBUTE 1018 /** no input io size attribute */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDINPUTIOSIZEATTRIBUTE 1019 /** invalid input io size attribute */
#define LIBMCDRIVER_CIFX_ERROR_NOOUTPUTIOSIZEATTRIBUTE 1020 /** no output io size attribute */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDOUTPUTIOSIZEATTRIBUTE 1021 /** invalid output io size attribute */
#define LIBMCDRIVER_CIFX_ERROR_MISSINGADDRESSATTRIBUTE 1022 /** missing address attribute */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSATTRIBUTE 1023 /** invalid address attribute */
#define LIBMCDRIVER_CIFX_ERROR_DUPLICATEINPUTIO 1024 /** duplicate input io */
#define LIBMCDRIVER_CIFX_ERROR_DUPLICATEOUTPUTIO 1025 /** duplicate output io */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDHOSTSTATETIMEOUT 1026 /** invalid host state timeout */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDBUSSTATETIMEOUT 1027 /** invalid bus state timeout */
#define LIBMCDRIVER_CIFX_ERROR_CHANNELALREADYOPEN 1028 /** channel already open */
#define LIBMCDRIVER_CIFX_ERROR_UNKNOWNEXCEPTION 1029 /** an unknown exception occured */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREAD 1030 /** invalid address read */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREADBIT 1031 /** invalid address read bit */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSWRITE 1032 /** invalid address write */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSWRITEBIT 1033 /** invalid address write bit */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDBITINDEX 1034 /** invalid bit index */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDBOOLPARAMETERCAST 1035 /** invalid bool parameter cast */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDNAMEATTRIBUTE 1036 /** invalid name attribute */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELINDEX 1037 /** invalid channel index */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDINPUTINDEX 1038 /** invalid input index */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDOUTPUTINDEX 1039 /** invalid output index */
#define LIBMCDRIVER_CIFX_ERROR_NOCONFIGURATIONSCHEMA 1040 /** no configuration schema */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDCONFIGURATIONSCHEMA 1041 /** invalid configuration schema */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDPARAMETERTYPE 1042 /** invalid parameter type */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDENDIANESSATTRIBUTE 1043 /** invalid endianess attribute */
#define LIBMCDRIVER_CIFX_ERROR_MISSINGCHANNELENDIANESSATTRIBUTE 1044 /** missing channel endianess attribute */
#define LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELENDIANESSATTRIBUTE 1045 /** invalid channel endianess attribute */

/*************************************************************************************************************************
 Error strings for LibMCDriver_CifX
**************************************************************************************************************************/

inline const char * LIBMCDRIVER_CIFX_GETERRORSTRING (LibMCDriver_CifXResult nErrorCode) {
  switch (nErrorCode) {
    case LIBMCDRIVER_CIFX_SUCCESS: return "no error";
    case LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED: return "functionality not implemented";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM: return "an invalid parameter was passed";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDCAST: return "a type cast failed";
    case LIBMCDRIVER_CIFX_ERROR_BUFFERTOOSMALL: return "a provided buffer is too small";
    case LIBMCDRIVER_CIFX_ERROR_GENERICEXCEPTION: return "a generic exception occurred";
    case LIBMCDRIVER_CIFX_ERROR_COULDNOTLOADLIBRARY: return "the library could not be loaded";
    case LIBMCDRIVER_CIFX_ERROR_COULDNOTFINDLIBRARYEXPORT: return "a required exported symbol could not be found in the library";
    case LIBMCDRIVER_CIFX_ERROR_INCOMPATIBLEBINARYVERSION: return "the version of the binary interface does not match the bindings interface";
    case LIBMCDRIVER_CIFX_ERROR_DRIVERERROR: return "a driver error occured";
    case LIBMCDRIVER_CIFX_ERROR_DRIVERNOTINITIALISED: return "the driver is not initialised";
    case LIBMCDRIVER_CIFX_ERROR_DRIVERNOTCONNECTED: return "the driver is not connected";
    case LIBMCDRIVER_CIFX_ERROR_DRIVERTYPENOTFOUND: return "the driver type has not been found";
    case LIBMCDRIVER_CIFX_ERROR_SDKALREADYLOADED: return "SDK already loaded";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDSDKRESOURCE: return "invalid SDK Resource";
    case LIBMCDRIVER_CIFX_ERROR_OUTPUTVALUENOTFOUND: return "output value not found";
    case LIBMCDRIVER_CIFX_ERROR_INPUTVALUENOTFOUND: return "input value not found";
    case LIBMCDRIVER_CIFX_ERROR_VALUENOTFOUND: return "value not found";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDDRIVERPROTOCOL: return "invalid driver protocol";
    case LIBMCDRIVER_CIFX_ERROR_COULDNOTPARSEDRIVERPROTOCOL: return "could not parse driver protocol";
    case LIBMCDRIVER_CIFX_ERROR_NOVERSIONDEFINITION: return "no version definition";
    case LIBMCDRIVER_CIFX_ERROR_NOMAJORVERSION: return "no major version";
    case LIBMCDRIVER_CIFX_ERROR_NOMINORVERSION: return "no minor version";
    case LIBMCDRIVER_CIFX_ERROR_NOPATCHVERSION: return "no patch version";
    case LIBMCDRIVER_CIFX_ERROR_NOBOARDATTRIBUTE: return "no board attribute";
    case LIBMCDRIVER_CIFX_ERROR_NOCHANNELINDEXATTRIBUTE: return "no channel index attribute";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELINDEXATTRIBUTE: return "invalid channel index attribute";
    case LIBMCDRIVER_CIFX_ERROR_NOINPUTIOSIZEATTRIBUTE: return "no input io size attribute";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDINPUTIOSIZEATTRIBUTE: return "invalid input io size attribute";
    case LIBMCDRIVER_CIFX_ERROR_NOOUTPUTIOSIZEATTRIBUTE: return "no output io size attribute";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDOUTPUTIOSIZEATTRIBUTE: return "invalid output io size attribute";
    case LIBMCDRIVER_CIFX_ERROR_MISSINGADDRESSATTRIBUTE: return "missing address attribute";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSATTRIBUTE: return "invalid address attribute";
    case LIBMCDRIVER_CIFX_ERROR_DUPLICATEINPUTIO: return "duplicate input io";
    case LIBMCDRIVER_CIFX_ERROR_DUPLICATEOUTPUTIO: return "duplicate output io";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDHOSTSTATETIMEOUT: return "invalid host state timeout";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDBUSSTATETIMEOUT: return "invalid bus state timeout";
    case LIBMCDRIVER_CIFX_ERROR_CHANNELALREADYOPEN: return "channel already open";
    case LIBMCDRIVER_CIFX_ERROR_UNKNOWNEXCEPTION: return "an unknown exception occured";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREAD: return "invalid address read";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSREADBIT: return "invalid address read bit";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSWRITE: return "invalid address write";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDADDRESSWRITEBIT: return "invalid address write bit";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDBITINDEX: return "invalid bit index";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDBOOLPARAMETERCAST: return "invalid bool parameter cast";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDNAMEATTRIBUTE: return "invalid name attribute";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELINDEX: return "invalid channel index";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDINPUTINDEX: return "invalid input index";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDOUTPUTINDEX: return "invalid output index";
    case LIBMCDRIVER_CIFX_ERROR_NOCONFIGURATIONSCHEMA: return "no configuration schema";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDCONFIGURATIONSCHEMA: return "invalid configuration schema";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDPARAMETERTYPE: return "invalid parameter type";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDENDIANESSATTRIBUTE: return "invalid endianess attribute";
    case LIBMCDRIVER_CIFX_ERROR_MISSINGCHANNELENDIANESSATTRIBUTE: return "missing channel endianess attribute";
    case LIBMCDRIVER_CIFX_ERROR_INVALIDCHANNELENDIANESSATTRIBUTE: return "invalid channel endianess attribute";
    default: return "unknown error";
  }
}

/*************************************************************************************************************************
 Declaration of handle classes 
**************************************************************************************************************************/

typedef LibMCDriver_CifXHandle LibMCDriver_CifX_Base;
typedef LibMCDriver_CifXHandle LibMCDriver_CifX_Driver;
typedef LibMCDriver_CifXHandle LibMCDriver_CifX_ChannelInformation;
typedef LibMCDriver_CifXHandle LibMCDriver_CifX_Driver_CifX;

namespace LibMCDriver_CifX {

  /*************************************************************************************************************************
   Declaration of enums
  **************************************************************************************************************************/
  
  enum class eValueType : LibMCDriver_CifX_int32 {
    Unknown = 0,
    IntegerValue = 1,
    BoolValue = 2,
    DoubleValue = 3
  };
  
} // namespace LibMCDriver_CifX;

// define legacy C-names for enums, structs and function types
typedef LibMCDriver_CifX::eValueType eLibMCDriver_CifXValueType;

#endif // __LIBMCDRIVER_CIFX_TYPES_HEADER_CPP
