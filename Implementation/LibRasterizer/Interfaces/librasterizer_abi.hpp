/*++

Copyright (C) 2021 Autodesk Inc.

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


This file has been generated by the Automatic Component Toolkit (ACT) version 1.7.0-develop.

Abstract: This is an autogenerated C++-Header file in order to allow an easy
 use of LibRasterizer

Interface version: 1.0.0

*/

#ifndef __LIBRASTERIZER_HEADER_CPP
#define __LIBRASTERIZER_HEADER_CPP

#ifdef __LIBRASTERIZER_EXPORTS
#ifdef _WIN32
#define LIBRASTERIZER_DECLSPEC __declspec (dllexport)
#else // _WIN32
#define LIBRASTERIZER_DECLSPEC __attribute__((visibility("default")))
#endif // _WIN32
#else // __LIBRASTERIZER_EXPORTS
#define LIBRASTERIZER_DECLSPEC
#endif // __LIBRASTERIZER_EXPORTS

#include "librasterizer_types.hpp"


#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************************************************************
 Class definition for Base
**************************************************************************************************************************/

/*************************************************************************************************************************
 Class definition for ImageObject
**************************************************************************************************************************/

/**
* Returns DPI values in X and Y.
*
* @param[in] pImageObject - ImageObject instance.
* @param[out] pDPIValueX - DPI value in X
* @param[out] pDPIValueY - DPI value in Y
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_getdpi(LibRasterizer_ImageObject pImageObject, LibRasterizer_double * pDPIValueX, LibRasterizer_double * pDPIValueY);

/**
* Returns image sizes.
*
* @param[in] pImageObject - ImageObject instance.
* @param[out] pSizeX - Size in X in mm
* @param[out] pSizeY - Size in Y in mm
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_getsize(LibRasterizer_ImageObject pImageObject, LibRasterizer_double * pSizeX, LibRasterizer_double * pSizeY);

/**
* Returns image pixel sizes.
*
* @param[in] pImageObject - ImageObject instance.
* @param[out] pPixelSizeX - Number of pixels in X
* @param[out] pPixelSizeY - Number of pixels in Y
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_getpixelsize(LibRasterizer_ImageObject pImageObject, LibRasterizer_uint32 * pPixelSizeX, LibRasterizer_uint32 * pPixelSizeY);

/**
* Saves image in a PNG file.
*
* @param[in] pImageObject - ImageObject instance.
* @param[in] pFileName - Filename to save image to.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_savetopng(LibRasterizer_ImageObject pImageObject, const char * pFileName);

/**
* Sets all pixels to a single value.
*
* @param[in] pImageObject - ImageObject instance.
* @param[in] nValue - Pixel value.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_clear(LibRasterizer_ImageObject pImageObject, LibRasterizer_uint8 nValue);

/**
* Returns one pixel of an image.
*
* @param[in] pImageObject - ImageObject instance.
* @param[in] nX - Pixel coordinate in X
* @param[in] nY - Pixel coordinate in Y
* @param[out] pValue - Pixel value at this position
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_getpixel(LibRasterizer_ImageObject pImageObject, LibRasterizer_uint32 nX, LibRasterizer_uint32 nY, LibRasterizer_uint8 * pValue);

/**
* Sets one pixel of an image.
*
* @param[in] pImageObject - ImageObject instance.
* @param[in] nX - Pixel coordinate in X
* @param[in] nY - Pixel coordinate in Y
* @param[in] nValue - New Pixel value at this position
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_setpixel(LibRasterizer_ImageObject pImageObject, LibRasterizer_uint32 nX, LibRasterizer_uint32 nY, LibRasterizer_uint8 nValue);

/**
* Returns a subset of an image or the whole image data.
*
* @param[in] pImageObject - ImageObject instance.
* @param[in] nXMin - Min Pixel coordinate in X. MUST be within image bounds.
* @param[in] nYMin - Min Pixel coordinate in Y. MUST be within image bounds.
* @param[in] nXMax - Max Pixel coordinate in X. MUST be within image bounds. MUST be larger or equal than MinX
* @param[in] nYMax - Max Pixel coordinate in Y. MUST be within image bounds. MUST be larger or equal than MinY
* @param[in] nValueBufferSize - Number of elements in buffer
* @param[out] pValueNeededCount - will be filled with the count of the written elements, or needed buffer size.
* @param[out] pValueBuffer - uint8 buffer of Pixel values of the rectangle, rowwise array. MUST have the exact number of pixels in size.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_getpixelrange(LibRasterizer_ImageObject pImageObject, LibRasterizer_uint32 nXMin, LibRasterizer_uint32 nYMin, LibRasterizer_uint32 nXMax, LibRasterizer_uint32 nYMax, const LibRasterizer_uint64 nValueBufferSize, LibRasterizer_uint64* pValueNeededCount, LibRasterizer_uint8 * pValueBuffer);

/**
* Exchanges a subset of an image or the whole image data.
*
* @param[in] pImageObject - ImageObject instance.
* @param[in] nXMin - Min Pixel coordinate in X. MUST be within image bounds.
* @param[in] nYMin - Min Pixel coordinate in Y. MUST be within image bounds.
* @param[in] nXMax - Max Pixel coordinate in X. MUST be within image bounds. MUST be larger or equal than MinX
* @param[in] nYMax - Max Pixel coordinate in Y. MUST be within image bounds. MUST be larger or equal than MinY
* @param[in] nValueBufferSize - Number of elements in buffer
* @param[in] pValueBuffer - uint8 buffer of New pixel values of the rectangle, rowwise array. MUST have the exact number of pixels in size.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_imageobject_setpixelrange(LibRasterizer_ImageObject pImageObject, LibRasterizer_uint32 nXMin, LibRasterizer_uint32 nYMin, LibRasterizer_uint32 nXMax, LibRasterizer_uint32 nYMax, LibRasterizer_uint64 nValueBufferSize, const LibRasterizer_uint8 * pValueBuffer);

/*************************************************************************************************************************
 Class definition for LayerObject
**************************************************************************************************************************/

/**
* Returns count of Entity in Layer.
*
* @param[in] pLayerObject - LayerObject instance.
* @param[out] pEntityCount - Number of entity in layer.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_getentitycount(LibRasterizer_LayerObject pLayerObject, LibRasterizer_uint32 * pEntityCount);

/**
* Returns a entity in a layer.
*
* @param[in] pLayerObject - LayerObject instance.
* @param[in] nEntityIndex - Index of entity in layer.
* @param[out] pGeometryType - Geometry type of Entity.
* @param[in] nPointsBufferSize - Number of elements in buffer
* @param[out] pPointsNeededCount - will be filled with the count of the written elements, or needed buffer size.
* @param[out] pPointsBuffer - Position2D buffer of Points of Entity.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_getentity(LibRasterizer_LayerObject pLayerObject, LibRasterizer_uint32 nEntityIndex, LibRasterizer::eGeometryType * pGeometryType, const LibRasterizer_uint64 nPointsBufferSize, LibRasterizer_uint64* pPointsNeededCount, LibRasterizer::sPosition2D * pPointsBuffer);

/**
* Adds a entity to a layer.
*
* @param[in] pLayerObject - LayerObject instance.
* @param[in] nPointsBufferSize - Number of elements in buffer
* @param[in] pPointsBuffer - Position2D buffer of Points of Entity.
* @param[in] eGeometryType - Geometry type of Entity.
* @param[out] pEntityIndex - Index of entity in layer.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_addentity(LibRasterizer_LayerObject pLayerObject, LibRasterizer_uint64 nPointsBufferSize, const LibRasterizer::sPosition2D * pPointsBuffer, LibRasterizer::eGeometryType eGeometryType, LibRasterizer_uint32 * pEntityIndex);

/**
* Removes self-intersections of a layer.
*
* @param[in] pLayerObject - LayerObject instance.
* @param[out] pLayerObjectInstance - New Layer Object Instance
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_removeselfintersections(LibRasterizer_LayerObject pLayerObject, LibRasterizer_LayerObject * pLayerObjectInstance);

/**
* Merges layer into another layer object.
*
* @param[in] pLayerObject - LayerObject instance.
* @param[in] pOtherLayerObject - Layer object to modify.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_mergeinto(LibRasterizer_LayerObject pLayerObject, LibRasterizer_LayerObject pOtherLayerObject);

/**
* Calculates an offset to a layer.
*
* @param[in] pLayerObject - LayerObject instance.
* @param[in] dOffsetValue - Offset in mm
* @param[out] pLayerObjectInstance - Offsetted Layer Object Instance
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_calculateoffset(LibRasterizer_LayerObject pLayerObject, LibRasterizer_double dOffsetValue, LibRasterizer_LayerObject * pLayerObjectInstance);

/**
* Thickens up polylines of a layer.
*
* @param[in] pLayerObject - LayerObject instance.
* @param[in] dThickness - Thickness in mm
* @param[out] pLayerObjectInstance - Offsetted Layer Object Instance
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_thickenpolylines(LibRasterizer_LayerObject pLayerObject, LibRasterizer_double dThickness, LibRasterizer_LayerObject * pLayerObjectInstance);

/**
* Thickens up hatches of a layer.
*
* @param[in] pLayerObject - LayerObject instance.
* @param[in] dThickness - Thickness in mm
* @param[out] pLayerObjectInstance - Offsetted Layer Object Instance
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_thickenhatches(LibRasterizer_LayerObject pLayerObject, LibRasterizer_double dThickness, LibRasterizer_LayerObject * pLayerObjectInstance);

/**
* Distorts a layer with a callback function
*
* @param[in] pLayerObject - LayerObject instance.
* @param[in] dRefinementValue - Lines longer than this value are split up in smaller segments to not loose distortion information.
* @param[in] pDistortionCallback - pointer to the callback function.
* @param[in] pUserData - pointer to arbitrary user data that is passed without modification to the callback.
* @param[out] pLayerObjectInstance - Offsetted Layer Object Instance
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_layerobject_distortlayer(LibRasterizer_LayerObject pLayerObject, LibRasterizer_double dRefinementValue, LibRasterizer::DistortionCallback pDistortionCallback, LibRasterizer_pvoid pUserData, LibRasterizer_LayerObject * pLayerObjectInstance);

/*************************************************************************************************************************
 Class definition for Rasterizer
**************************************************************************************************************************/

/**
* Returns DPI values in X and Y.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[out] pDPIValueX - DPI value in X
* @param[out] pDPIValueY - DPI value in Y
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_getdpi(LibRasterizer_Rasterizer pRasterizer, LibRasterizer_double * pDPIValueX, LibRasterizer_double * pDPIValueY);

/**
* Returns image sizes.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[out] pSizeX - Size in X in mm
* @param[out] pSizeY - Size in Y in mm
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_getsize(LibRasterizer_Rasterizer pRasterizer, LibRasterizer_double * pSizeX, LibRasterizer_double * pSizeY);

/**
* Returns image pixel sizes.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[out] pPixelSizeX - Number of pixels in X
* @param[out] pPixelSizeY - Number of pixels in Y
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_getpixelsize(LibRasterizer_Rasterizer pRasterizer, LibRasterizer_uint32 * pPixelSizeX, LibRasterizer_uint32 * pPixelSizeY);

/**
* Returns image position.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[out] pPositionX - Position in X in mm
* @param[out] pPositionY - Position in Y in mm
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_getposition(LibRasterizer_Rasterizer pRasterizer, LibRasterizer_double * pPositionX, LibRasterizer_double * pPositionY);

/**
* Sets image position.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[in] dPositionX - Position in X in mm
* @param[in] dPositionY - Position in Y in mm
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_setposition(LibRasterizer_Rasterizer pRasterizer, LibRasterizer_double dPositionX, LibRasterizer_double dPositionY);

/**
* Set subsampling values in X and Y.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[in] nSubsamplingX - Subsampling in X
* @param[in] nSubsamplingY - Subsampling in Y
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_setsubsampling(LibRasterizer_Rasterizer pRasterizer, LibRasterizer_uint32 nSubsamplingX, LibRasterizer_uint32 nSubsamplingY);

/**
* Returns subsampling values in X and Y.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[out] pSubsamplingX - Subsampling in X
* @param[out] pSubsamplingY - Subsampling in Y
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_getsubsampling(LibRasterizer_Rasterizer pRasterizer, LibRasterizer_uint32 * pSubsamplingX, LibRasterizer_uint32 * pSubsamplingY);

/**
* Adds a layer object to subsample.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[in] pLayerObject - Layer object instance.
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_addlayer(LibRasterizer_Rasterizer pRasterizer, LibRasterizer_LayerObject pLayerObject);

/**
* Calculates the image.
*
* @param[in] pRasterizer - Rasterizer instance.
* @param[in] bAntialiased - Image output is greyscale if true, black and white with 0.5 threshold if false.
* @param[out] pImageObject - ImageObject Instance
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_rasterizer_calculateimage(LibRasterizer_Rasterizer pRasterizer, bool bAntialiased, LibRasterizer_ImageObject * pImageObject);

/*************************************************************************************************************************
 Class definition for SliceStack
**************************************************************************************************************************/

/**
* Returns layer count of slice stack.
*
* @param[in] pSliceStack - SliceStack instance.
* @param[out] pLayerCount - Returns number of layers
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_slicestack_getlayercount(LibRasterizer_SliceStack pSliceStack, LibRasterizer_uint32 * pLayerCount);

/**
* Returns layer thickness of slice stack.
*
* @param[in] pSliceStack - SliceStack instance.
* @param[out] pLayerThickness - Returns layer thickness in mm
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_slicestack_getlayerthickness(LibRasterizer_SliceStack pSliceStack, LibRasterizer_double * pLayerThickness);

/**
* Returns Z value of bottom of slice stack in mm.
*
* @param[in] pSliceStack - SliceStack instance.
* @param[out] pZValue - Z-Value of bottom of slice stack
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_slicestack_getbottomz(LibRasterizer_SliceStack pSliceStack, LibRasterizer_double * pZValue);

/**
* Returns Z value of top of slice stack in mm.
*
* @param[in] pSliceStack - SliceStack instance.
* @param[out] pZValue - Z-Value of top of slice stack
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_slicestack_gettopz(LibRasterizer_SliceStack pSliceStack, LibRasterizer_double * pZValue);

/**
* Returns layer object for a given Z Value.
*
* @param[in] pSliceStack - SliceStack instance.
* @param[in] nLayerIndex - Index of layer.
* @param[out] pLayerObject - LayerObject Instance
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_slicestack_getlayer(LibRasterizer_SliceStack pSliceStack, LibRasterizer_uint32 nLayerIndex, LibRasterizer_LayerObject * pLayerObject);

/*************************************************************************************************************************
 Class definition for Context
**************************************************************************************************************************/

/**
* creates an empty slice stack.
*
* @param[in] pContext - Context instance.
* @param[in] nLayerCount - Number of layers. MUST be positive.
* @param[in] dLayerThickness - Layerthickness in mm. MUST be positive.
* @param[in] dBottomZ - Bottom Z value in mm.
* @param[out] pSliceStackInstance - Instance of Slice Stack
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_context_createslicestack(LibRasterizer_Context pContext, LibRasterizer_uint32 nLayerCount, LibRasterizer_double dLayerThickness, LibRasterizer_double dBottomZ, LibRasterizer_SliceStack * pSliceStackInstance);

/**
* creates an empty layer object.
*
* @param[in] pContext - Context instance.
* @param[out] pLayerObject - Instance of a Layer Object
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_context_createemptylayer(LibRasterizer_Context pContext, LibRasterizer_LayerObject * pLayerObject);

/**
* creates a rasterizer object.
*
* @param[in] pContext - Context instance.
* @param[in] nPixelSizeX - Pixel size in X. MUST be positive.
* @param[in] nPixelSizeY - Pixel size in Y. MUST be positive.
* @param[in] dDPIX - DPI in X. MUST be positive.
* @param[in] dDPIY - DPI in Y. MUST be positive.
* @param[out] pRasterizerInstance - Instance of Rasterizer
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_context_createrasterizer(LibRasterizer_Context pContext, LibRasterizer_uint32 nPixelSizeX, LibRasterizer_uint32 nPixelSizeY, LibRasterizer_double dDPIX, LibRasterizer_double dDPIY, LibRasterizer_Rasterizer * pRasterizerInstance);

/*************************************************************************************************************************
 Global functions
**************************************************************************************************************************/

/**
* retrieves the binary version of this library.
*
* @param[out] pMajor - returns the major version of this library
* @param[out] pMinor - returns the minor version of this library
* @param[out] pMicro - returns the micro version of this library
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_getversion(LibRasterizer_uint32 * pMajor, LibRasterizer_uint32 * pMinor, LibRasterizer_uint32 * pMicro);

/**
* Returns the last error recorded on this object
*
* @param[in] pInstance - Instance Handle
* @param[in] nErrorMessageBufferSize - size of the buffer (including trailing 0)
* @param[out] pErrorMessageNeededChars - will be filled with the count of the written bytes, or needed buffer size.
* @param[out] pErrorMessageBuffer -  buffer of Message of the last error, may be NULL
* @param[out] pHasError - Is there a last error to query
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_getlasterror(LibRasterizer_Base pInstance, const LibRasterizer_uint32 nErrorMessageBufferSize, LibRasterizer_uint32* pErrorMessageNeededChars, char * pErrorMessageBuffer, bool * pHasError);

/**
* Releases shared ownership of an Instance
*
* @param[in] pInstance - Instance Handle
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_releaseinstance(LibRasterizer_Base pInstance);

/**
* Acquires shared ownership of an Instance
*
* @param[in] pInstance - Instance Handle
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_acquireinstance(LibRasterizer_Base pInstance);

/**
* Injects an imported component for usage within this component
*
* @param[in] pNameSpace - NameSpace of the injected component
* @param[in] pSymbolAddressMethod - Address of the SymbolAddressMethod of the injected component
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_injectcomponent(const char * pNameSpace, LibRasterizer_pvoid pSymbolAddressMethod);

/**
* Returns the address of the SymbolLookupMethod
*
* @param[out] pSymbolLookupMethod - Address of the SymbolAddressMethod
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_getsymbollookupmethod(LibRasterizer_pvoid * pSymbolLookupMethod);

/**
* Creates a new context.
*
* @param[out] pInstance - New Context instance
* @return error code or 0 (success)
*/
LIBRASTERIZER_DECLSPEC LibRasterizerResult librasterizer_createcontext(LibRasterizer_Context * pInstance);

#ifdef __cplusplus
}
#endif

#endif // __LIBRASTERIZER_HEADER_CPP
