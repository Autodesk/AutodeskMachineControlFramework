/*++

Copyright (C) 2022 Autodesk Inc.

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


*/

#include "libmcdriver_rasterizer_algorithm.hpp"
#include "libmcdriver_rasterizer_interfaceexception.hpp"

#include <cmath>

using namespace LibMCDriver_Rasterizer::Impl;



#define MINBLOCKCOUNT 1
#define MAXBLOCKCOUNT (1024 * 1024)

#define MAXCOORDINATE (512 * 1024 * 1024)
#define MAXLINECOUNT (512 * 1024 * 1024)

#define __RASTERASSERT(X,Y) { if (!(X)) throw std::runtime_error (Y); }


int32_t intFloorDiv(int32_t nDivident, int32_t nDivisor)
{
    if (nDivident == 0)
        return 0;
    if (nDivisor == 0)
        throw std::runtime_error("division by zero!");

    if (nDivident < 0) {
        if (nDivisor < 0) {
            // Positive division
            return ((uint32_t)(-nDivident) / (uint32_t)(-nDivisor));
        }
        else {
            uint32_t nUnsignedDivident = (uint32_t)(-nDivident);
            uint32_t nUnsignedDivisor = (uint32_t)(nDivisor);

            // Round division up in this case!
            uint32_t nUnsignedResult = (nUnsignedDivident + nUnsignedDivisor - 1) / nUnsignedDivisor;
            return -((int32_t)nUnsignedResult);
        }
    }
    else {
        if (nDivisor < 0) {
            uint32_t nUnsignedDivident = (uint32_t)(nDivident);
            uint32_t nUnsignedDivisor = (uint32_t)(-nDivisor);

            // Round division up in this case!
            uint32_t nUnsignedResult = (nUnsignedDivident + nUnsignedDivisor - 1) / nUnsignedDivisor;
            return -((int32_t)nUnsignedResult);
        }
        else {

            return ((uint32_t)(nDivident) / (uint32_t)(nDivisor));
        }
    }
}


CRasterizationAlgorithm::CRasterizationAlgorithm(uint32_t nUnitsPerSubPixel, uint32_t nSubPixelsPerPixelX, uint32_t nSubPixelsPerPixelY, uint32_t nPixelsPerBlock, uint32_t nBlockCountX, uint32_t nBlockCountY, uint32_t nExpectedLineCount)
    : m_nUnitsPerSubPixel (nUnitsPerSubPixel), 
    m_nSubPixelsPerPixelX (nSubPixelsPerPixelX),
    m_nSubPixelsPerPixelY (nSubPixelsPerPixelY),
    m_nPixelsPerBlock (nPixelsPerBlock),
    m_nBlockCountX (nBlockCountX),
    m_nBlockCountY (nBlockCountY),
    m_nExpectedLineCount (nExpectedLineCount),
    m_nNegativeHalfPlaneLineCount (0),
    m_nDisregardedLineCount (0),
    m_LineItemBufferCapacity (0),
    m_LineItemBufferIndex (0)

{
    if ((nUnitsPerSubPixel < RASTERALGORITHM_MINUNITSPERSUBPIXEL) || (nUnitsPerSubPixel > RASTERALGORITHM_MAXUNITSPERSUBPIXEL) || ((nUnitsPerSubPixel % 2) != 0))
        throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDUNITSPERSUBPIXEL);
    
    if ((nSubPixelsPerPixelX < RASTERALGORITHM_MINSUBPIXELSPERPIXEL) || (nSubPixelsPerPixelX > RASTERALGORITHM_MAXSUBPIXELSPERPIXEL))
        throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDSUBPIXELSPERPIXELS);

    if ((nPixelsPerBlock < RASTERALGORITHM_MINPIXELSPERBLOCK) || (nPixelsPerBlock > RASTERALGORITHM_MAXPIXELSPERBLOCK))
        throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDPIXELSPERBLOCK);

    if ((nBlockCountX < MINBLOCKCOUNT) || (nBlockCountX > MAXBLOCKCOUNT))
        throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDBLOCKCOUNT);
    if ((nBlockCountY < MINBLOCKCOUNT) || (nBlockCountY > MAXBLOCKCOUNT))
        throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDBLOCKCOUNT);

    m_nUnitsPerHalfSubPixel = nUnitsPerSubPixel / 2;
    m_nUnitsPerPixelX = nUnitsPerSubPixel * nSubPixelsPerPixelX;
    m_nUnitsPerPixelY = nUnitsPerSubPixel * nSubPixelsPerPixelY;
    m_nUnitsPerBlockX = m_nUnitsPerPixelX * nPixelsPerBlock;
    m_nUnitsPerBlockY = m_nUnitsPerPixelY * nPixelsPerBlock;

    m_nTotalSizeInUnitsX = (uint64_t) m_nUnitsPerBlockX * (uint64_t)m_nBlockCountX;
    m_nTotalSizeInUnitsY = (uint64_t) m_nUnitsPerBlockY * (uint64_t)m_nBlockCountY;

    m_Lines.reserve(m_nExpectedLineCount);

    m_Blocks.resize((uint64_t)nBlockCountX * (uint64_t)nBlockCountY);
    for (auto& block : m_Blocks) {
        block.m_pFirstItem = nullptr;
        block.m_Type = eBlockType::btUnknown;
    }

}

CRasterizationAlgorithm::~CRasterizationAlgorithm()
{

}

bool CRasterizationAlgorithm::isOnScanLineX(int32_t nXunits)
{
    return ((nXunits % m_nUnitsPerHalfSubPixel) == 0);
}

bool CRasterizationAlgorithm::isOnScanLineY(int32_t nYunits)
{
    return ((nYunits % m_nUnitsPerHalfSubPixel) == 0);
}


void CRasterizationAlgorithm::addLine(int32_t nX1units, int32_t nY1units, int32_t nX2units, int32_t nY2units, int32_t nMetaData)
{
    bool bIsNegativeInX = ((nX1units <= 0) || (nX2units <= 0));       
    bool bIsTotalNegativeInY = ((nY1units <= 0) && (nY2units <= 0));
    bool bTouchesXAxis = ((nX1units <= 0) && (nX2units >= 0)) || ((nX1units >= 0) && (nX2units <= 0));
    bool bHasZeroLength = (nX1units == nX2units) && (nY1units == nY2units);

    bool bIsOutsideAreaInX = ((nX1units >= m_nTotalSizeInUnitsX) || (nX2units >= m_nTotalSizeInUnitsX));
    bool bIsOutsideAreaInY = ((nY1units >= m_nTotalSizeInUnitsY) || (nY2units >= m_nTotalSizeInUnitsY));

    // Disregard irrelevant lines
    bool bIsDisregarded = false;
    if (bHasZeroLength)
        bIsDisregarded = true;
    if (bIsOutsideAreaInX)
        bIsDisregarded = true;
    if (bIsOutsideAreaInY)
        bIsDisregarded = true;
    if (bIsTotalNegativeInY && (!bTouchesXAxis))
        bIsDisregarded = true;

    if (!bIsDisregarded) {

        if ((abs(nX1units) > MAXCOORDINATE) || (abs(nY1units) > MAXCOORDINATE)
            || (abs(nX2units) > MAXCOORDINATE) || (abs(nY2units) > MAXCOORDINATE))
            throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_LINECOORDINATEOVERFLOW);

        if (isOnScanLineX(nX1units))
            nX1units++;
        if (isOnScanLineY(nY1units))
            nY1units++;
        if (isOnScanLineX(nX2units))
            nX2units++;
        if (isOnScanLineY(nY2units))
            nY2units++;

        addLineInternal(nX1units, nY1units, nX2units, nY2units, nMetaData);

    }
    else {
        m_nDisregardedLineCount++;
    }
}

void CRasterizationAlgorithm::addLineInternal(int32_t nX1units, int32_t nY1units, int32_t nX2units, int32_t nY2units, int32_t nMetaData)
{

    if (m_Lines.size () >= MAXLINECOUNT)
        throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_TOOMANYLINESINLAYER);
    sRasterLine sLine;
    sLine.m_nLineIndex = (uint32_t)m_Lines.size();
    sLine.m_nX1 = nX1units;
    sLine.m_nY1 = nY1units;
    sLine.m_nX2 = nX2units;
    sLine.m_nY2 = nY2units;
    sLine.m_nMetaData = nMetaData;

    m_Lines.push_back(sLine);

    uint32_t nDeltaX = abs (nX1units - nX2units);
    uint32_t nDeltaY = abs (nY1units - nY2units);
    uint32_t nMaxDeltaBlocksX = (nDeltaX / m_nUnitsPerBlockX) + 1;
    uint32_t nMaxDeltaBlocksY = (nDeltaY / m_nUnitsPerBlockY) + 1;

    if (nMaxDeltaBlocksX > nMaxDeltaBlocksY)
        m_LineItemBufferCapacity += (uint64_t)nMaxDeltaBlocksX * 2;  
    else
        m_LineItemBufferCapacity += (uint64_t)nMaxDeltaBlocksY * 2;
}

void CRasterizationAlgorithm::buildBlocks()
{
    for (auto& block : m_Blocks) {
        block.m_pFirstItem = nullptr;
        block.m_Type = eBlockType::btUnknown;
    }

    m_LineListItemBuffer.resize(m_LineItemBufferCapacity);
    m_LineItemBufferIndex = 0;

    for (auto& line : m_Lines) {
        if (line.m_nX1 == line.m_nX2) {
            int nBlockX = intFloorDiv(line.m_nX1, m_nUnitsPerBlockX);
            int nBlockY1 = intFloorDiv(line.m_nY1, m_nUnitsPerBlockY);
            int nBlockY2 = intFloorDiv(line.m_nY2, m_nUnitsPerBlockY);
            buildBlocksVertical (nBlockX, nBlockY1, nBlockY2, &line);
        }
        else if (line.m_nY1 == line.m_nY2) {
            int nBlockY = intFloorDiv(line.m_nY1, m_nUnitsPerBlockY);
            int nBlockX1 = intFloorDiv(line.m_nX1, m_nUnitsPerBlockX);
            int nBlockX2 = intFloorDiv(line.m_nX2, m_nUnitsPerBlockX);
            buildBlocksHorizontal(nBlockY, nBlockX1, nBlockX2, &line);
        }
        else {
            buildBlocksRational(&line);
        }
    }
}

sRasterBlockStructure* CRasterizationAlgorithm::getBlock(uint32_t nBlockIndexX, uint32_t nBlockIndexY)
{
    if ((nBlockIndexX < m_nBlockCountX) && (nBlockIndexY < m_nBlockCountY))
        return &m_Blocks.at((uint64_t)nBlockIndexX + (uint64_t)nBlockIndexY * (uint64_t)m_nBlockCountX);

    return nullptr;
}

eBlockType CRasterizationAlgorithm::getBlockInfoAtXY(int32_t nXunits, int32_t nYunits)
{
    if ((nXunits < 0) || (nYunits < 0))
        return eBlockType::btOutsideRange;

    uint32_t nBlockX = ((uint32_t)nXunits / m_nUnitsPerBlockX);
    uint32_t nBlockY = ((uint32_t)nYunits / m_nUnitsPerBlockY);

    return getBlockInfo(nBlockX, nBlockY);

}

eBlockType CRasterizationAlgorithm::getBlockInfo(int32_t nBlockX, int32_t nBlockY)
{

    sRasterBlockStructure* pBlock = getBlock(nBlockX, nBlockY);
    if (pBlock == nullptr)
        return eBlockType::btOutsideRange;

    return pBlock->m_Type;

}

void CRasterizationAlgorithm::addBlockToBuffer(int32_t nBlockX, int32_t nBlockY, std::vector<uint32_t>& buffer)
{
    __RASTERASSERT(buffer.size () == ((size_t)m_nPixelsPerBlock * m_nPixelsPerBlock), "invalid buffer block size");

}


void CRasterizationAlgorithm::buildBlocksHorizontal(int nBlockY, int nBlockX1, int nBlockX2, sRasterLine* pLine)
{
    __RASTERASSERT(pLine != nullptr, "unassigned raster line");

    if (nBlockX1 > nBlockX2) {
        std::swap(nBlockX1, nBlockX2);
    }

    if (nBlockX2 < 0)
        return;
    if (nBlockX1 >= (int32_t) m_nBlockCountX)
        return;
    if (nBlockY < 0)
        return;
    if (nBlockY >= (int32_t)m_nBlockCountY)
        return;

    if (nBlockX1 < 0)
        nBlockX1 = 0;
    if (nBlockX2 >= (int32_t)m_nBlockCountX)
        nBlockX2 = (int32_t)m_nBlockCountX - 1;

    for (int32_t nBlockX = nBlockX1; nBlockX <= nBlockX2; nBlockX ++) {
        sRasterBlockStructure* pBlock = getBlock (nBlockX, nBlockY);
        __RASTERASSERT(pBlock != nullptr, "invalid horizontal block");
            
        addLineToBlock (pLine, pBlock);
    }
}

void CRasterizationAlgorithm::buildBlocksVertical(int nBlockX, int nBlockY1, int nBlockY2, sRasterLine* pLine)
{
    __RASTERASSERT(pLine != nullptr, "unassigned raster line");

    if (nBlockY1 > nBlockY2) {
        std::swap(nBlockY1, nBlockY2);
    }
    
    if (nBlockY2 < 0)
        return;
    if (nBlockY1 >= (int32_t)m_nBlockCountY)
        return;
    if (nBlockX < 0)
        return;
    if (nBlockX >= (int32_t)m_nBlockCountX)
        return;

    if (nBlockY1 < 0)
        nBlockY1 = 0;
    if (nBlockY2 >= (int32_t)m_nBlockCountY)
        nBlockY2 = (int32_t)m_nBlockCountY - 1;

    for (int32_t nBlockY = nBlockY1; nBlockY <= nBlockY2; nBlockY++) {
        sRasterBlockStructure* pBlock = getBlock(nBlockX, nBlockY);
        __RASTERASSERT(pBlock != nullptr, "invalid vertical block");

        addLineToBlock(pLine, pBlock);
    }

}

void CRasterizationAlgorithm::buildBlocksRational(sRasterLine* pLine)
{
    __RASTERASSERT(pLine != nullptr, "unassigned raster line");
    int32_t nX1, nY1, nX2, nY2;

    // Sort Points in X
    if (pLine->m_nX1 > pLine->m_nX2) {
        nX1 = pLine->m_nX2;
        nY1 = pLine->m_nY2;
        nX2 = pLine->m_nX1;
        nY2 = pLine->m_nY1;
    }
    else {
        nX1 = pLine->m_nX1;
        nY1 = pLine->m_nY1;
        nX2 = pLine->m_nX2;
        nY2 = pLine->m_nY2;
    }

    int64_t nDeltaX = (int64_t)nX2 - (int64_t)nX1;
    int64_t nDeltaY = (int64_t)nY2 - (int64_t)nY1;
    __RASTERASSERT(nDeltaX > 0, "Delta X assumption wrong");
    __RASTERASSERT(nDeltaY != 0, "Delta Y assumption wrong");

    int32_t nBlockX1 = intFloorDiv(nX1, m_nUnitsPerBlockX);
    int32_t nBlockX2 = intFloorDiv(nX2, m_nUnitsPerBlockX);

    if (nBlockX2 < 0)
        return;
    if (nBlockX1 >= (int32_t)m_nBlockCountX)
        return;

    if (nBlockX1 == nBlockX2) {
        int32_t nBlockY1 = intFloorDiv(nY1, m_nUnitsPerBlockY);
        int32_t nBlockY2 = intFloorDiv(nY2, m_nUnitsPerBlockY);

        buildBlocksVertical(nBlockX1, nBlockY1, nBlockY2, pLine);
    }
    else {
        if (nBlockX1 < 0)
            nBlockX1 = 0;
        if (nBlockX2 >= (int32_t)m_nBlockCountX)
            nBlockX2 = (int32_t)m_nBlockCountX - 1;

        for (int32_t nBlockX = nBlockX1; nBlockX <= nBlockX2; nBlockX++) {

            int nBlockY1 = 0;
            int nBlockY2 = 0;

            int32_t startX = nBlockX * m_nUnitsPerBlockX;
            if (startX < nX1) {
                startX = nX1;
                nBlockY1 = intFloorDiv(nY1, m_nUnitsPerBlockY);
            }
            else {
                                
                // calculate nBlockY1
                int32_t dX = startX - nX1;

                __RASTERASSERT(dX > 0, "dX assumption wrong");
                int64_t nNumerator = (int64_t) dX * (int64_t)nDeltaY;
                int64_t nDenominator = nDeltaX;                      
                
            }

            int32_t endX = (nBlockX + 1) * m_nUnitsPerBlockX;
            if (endX > nX2) {
                endX = nX2;
                nBlockY2 = intFloorDiv(nY2, m_nUnitsPerBlockY);
            }
            else {
                                
                // calculate nBlockY1
                int32_t dX = startX - nX1;

                __RASTERASSERT(dX > 0, "dX assumption wrong");
                int64_t nNumerator = (int64_t)dX * (int64_t)nDeltaY;
                int64_t nDenominator = nDeltaX;                

            }

            buildBlocksVertical(nBlockX, nBlockY1, nBlockY2, pLine);

        }
    }

}

void CRasterizationAlgorithm::addLineToBlock(sRasterLine* pLine, _sRasterBlockStructure* pBlock)
{
    __RASTERASSERT(pLine != nullptr, "unassigned raster line");
    __RASTERASSERT(pBlock != nullptr, "unassigned raster block");
    __RASTERASSERT(m_LineItemBufferIndex < m_LineItemBufferCapacity, "line item capacity overflow");

    sRasterLineListItem* pItem = &m_LineListItemBuffer.at(m_LineItemBufferIndex);
    pItem->m_pLine = pLine;
    pItem->m_pNextItem = pBlock->m_pFirstItem;
    pBlock->m_pFirstItem = pItem;

    m_LineItemBufferIndex++;


    

}


