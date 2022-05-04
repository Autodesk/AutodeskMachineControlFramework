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


#ifndef __LIBMCDRIVER_RASTERIZER_ALGORITHM
#define __LIBMCDRIVER_RASTERIZER_ALGORITHM

#include <vector>
#include <memory>

#define RASTERALGORITHM_MINUNITSPERSUBPIXEL 4
#define RASTERALGORITHM_MAXUNITSPERSUBPIXEL (1024 * 1024)
#define RASTERALGORITHM_DEFAULTUNITSPERSUBPIXEL 256

#define RASTERALGORITHM_MINSUBPIXELSPERPIXEL 1
#define RASTERALGORITHM_MAXSUBPIXELSPERPIXEL 32

#define RASTERALGORITHM_MINPIXELSPERBLOCK 4
#define RASTERALGORITHM_MAXPIXELSPERBLOCK 1024
#define RASTERALGORITHM_DEFAULTPIXELSPERBLOCK 32

namespace LibMCDriver_Rasterizer {
    namespace Impl {

        enum class eBlockType : int32_t {
            btUnknown = 0,
            btCompleteInside = 1,
            btCompleteOutside = 2,
            btBorder = 3,
            btOutsideRange = 4
        };

        typedef struct _sRasterLine
        {
            uint32_t m_nLineIndex;
            int32_t m_nX1;
            int32_t m_nY1;
            int32_t m_nX2;
            int32_t m_nY2;
            int32_t m_nMetaData;
        } sRasterLine;

        typedef struct _sRasterLineListItem sRasterLineListItem;

        struct _sRasterLineListItem
        {
            sRasterLine* m_pLine;
            sRasterLineListItem* m_pNextItem;

        };

        typedef struct _sRasterBlockStructure
        {
            sRasterLineListItem * m_pFirstItem;
            uint32_t m_nLineCount;
            int32_t * m_pScanSeedValues;
            eBlockType m_Type;
        } sRasterBlockStructure;

        class CRasterizationAlgorithm {
            private:

                // unitsPerSubPixel must be even and greater than 4
                uint32_t m_nUnitsPerSubPixel;
                uint32_t m_nUnitsPerHalfSubPixel;

                // Subvoxelization can differ in X and Y.
                // needs to be between 1 and 32
                uint32_t m_nSubPixelsPerPixelX;
                uint32_t m_nSubPixelsPerPixelY;
                uint32_t m_nUnitsPerPixelX;
                uint32_t m_nUnitsPerPixelY;

                // Block size must be at least 4 pixels
                uint32_t m_nPixelsPerBlock;
                uint32_t m_nUnitsPerBlockX;
                uint32_t m_nUnitsPerBlockY;
                uint32_t m_nScanLinesPerBlock;

                // Block count must be positive
                uint32_t m_nBlockCountX;
                uint32_t m_nBlockCountY;
                uint64_t m_nTotalSizeInUnitsX;
                uint64_t m_nTotalSizeInUnitsY;
                uint64_t m_nBlocksInUse;

                // Expected Line Count is just for memory preallocation
                uint32_t m_nExpectedLineCount;

                // How many lines are touching negative half-plane in X?
                uint32_t m_nNegativeHalfPlaneLineCount;

                // How many lines have been disregarded for non-relevance?
                uint32_t m_nDisregardedLineCount;

                //
                uint64_t m_LineItemBufferCapacity;
                uint32_t m_LineItemBufferIndex;
                std::vector<sRasterLineListItem> m_LineListItemBuffer;

                // Block and Line arrays
                std::vector<sRasterBlockStructure> m_Blocks;
                std::vector<sRasterLine> m_Lines;
                std::vector<int32_t> m_ScanSeedValueBuffer;

                bool isOnScanLineX(int32_t nXunits);
                bool isOnScanLineY(int32_t nYunits);

                void addLineInternal (int32_t nX1units, int32_t nY1units, int32_t nX2units, int32_t nY2units, int32_t nMetaData);

                void buildBlocksVertical(int nBlockX, int nBlockY1, int nBlockY2, sRasterLine* pLine);
                void buildBlocksHorizontal (int nBlockY, int nBlockX1, int nBlockX2, sRasterLine* pLine);
                void buildBlocksRational (sRasterLine* pLine);
                void addLineToBlock(sRasterLine* pLine, _sRasterBlockStructure* pBlock);

            public:

                CRasterizationAlgorithm(uint32_t nUnitsPerSubPixel, uint32_t nSubPixelsPerPixelX, uint32_t nSubPixelsPerPixelY, uint32_t nPixelsPerBlock, uint32_t nBlockCountX, uint32_t nBlockCountY, uint32_t nExpectedLineCount);
                ~CRasterizationAlgorithm();

                void addLine(int32_t nX1units, int32_t nY1units, int32_t nX2units, int32_t nY2units, int32_t nMetaData);

                void buildBlocks();
                void buildBlockScanLines(uint32_t nBlockIndexX, uint32_t nBlockIndexY);

                sRasterBlockStructure* getBlock(uint32_t nBlockIndexX, uint32_t nBlockIndexY);
                eBlockType getBlockInfoAtXY (int32_t nXunits, int32_t nYunits);
                eBlockType getBlockInfo(int32_t nBlockX, int32_t nBlockY);

                void addBlockToBuffer (int32_t nBlockX, int32_t nBlockY, std::vector<uint32_t> & buffer);

                

        };

        typedef std::shared_ptr<CRasterizationAlgorithm> PRasterizationAlgorithm;

    }

}

#endif // __LIBMCDRIVER_RASTERIZER_ALGORITHM
