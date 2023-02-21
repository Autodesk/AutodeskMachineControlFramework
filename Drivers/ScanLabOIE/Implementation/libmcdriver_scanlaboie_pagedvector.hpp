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

#ifndef __LIBMCDRIVER_SCANLABOIE_PAGEDVECTOR
#define __LIBMCDRIVER_SCANLABOIE_PAGEDVECTOR

#include <vector>
#include <stdexcept>
#include <array>

namespace LibMCDriver_ScanLabOIE {
	namespace Impl {

		template <class T, unsigned int DEFAULTBLOCKSIZE = 1024>

		class CPagedVector {
			size_t m_nBlockSize;
			size_t m_nCount;
			T* m_pHeadBlock;
			std::vector<T*> m_pBlocks;
		public:

			CPagedVector() {
				m_nCount = 0;
				m_pHeadBlock = nullptr;
				m_nBlockSize = DEFAULTBLOCKSIZE;
				if (m_nBlockSize == 0)
					throw std::runtime_error("invalid paged vector block size");
			}

			CPagedVector(size_t nBlockSize) {
				m_nCount = 0;
				m_pHeadBlock = nullptr;
				m_nBlockSize = nBlockSize;
				if (m_nBlockSize == 0)
					throw std::runtime_error("invalid paged vector block size");
			}

			~CPagedVector() {
				clearAllData();
			}

			size_t getCount() {
				return m_nCount;
			}

			T* allocData() {
				size_t nIdx = (m_nCount % m_nBlockSize);

				// Allocate new node block if necessary
				if (nIdx == 0) {
					m_pHeadBlock = new T[m_nBlockSize];
					m_pBlocks.push_back(m_pHeadBlock);
				}

				T* pResult = &m_pHeadBlock[nIdx];
				m_nCount++;

				return pResult;
			}

			T* allocData(size_t& nNewIndex) {
				nNewIndex = m_nCount;
				return allocData();
			}

			T& allocDataRef(size_t & nNewIndex) {
				size_t nIdx = (m_nCount % m_nBlockSize);

				// Allocate new node block if necessary
				if (nIdx == 0) {
					m_pHeadBlock = new T[m_nBlockSize];
					m_pBlocks.push_back(m_pHeadBlock);
				}

				nNewIndex = m_nCount;
				m_nCount++;

				return m_pHeadBlock[nIdx];
			}

			T* getData(size_t nIdx) {
				if (nIdx >= m_nCount)
					throw std::runtime_error("invalid paged vector index");

				T* block = m_pBlocks[nIdx / m_nBlockSize];
				return &block[nIdx % m_nBlockSize];
			}

			T& getDataRef(size_t nIdx) {
				if (nIdx >= m_nCount)
					throw std::runtime_error("invalid paged vector index");

				T* block = m_pBlocks[nIdx / m_nBlockSize];
				return block[nIdx % m_nBlockSize];
			}

			void clearAllData() {
				for (auto iIterator = m_pBlocks.begin(); iIterator != m_pBlocks.end(); iIterator++)
				{
					T* pBlock = *iIterator;
					delete[] pBlock;
				}

				m_pBlocks.clear();
				m_nCount = 0;
				m_pHeadBlock = nullptr;
			}

			size_t getBlockSize() {
				return m_nBlockSize;
			}
		};

	}
}

#endif // __LIBMCDRIVER_SCANLABOIE_PAGEDVECTOR
