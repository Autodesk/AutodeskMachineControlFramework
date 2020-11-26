#ifndef MMALPP_BUFFER_UTILS_H
#define MMALPP_BUFFER_UTILS_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_buffer.h>

#include "exceptions/mmalpp_exceptions.h"
#include "../macros.h"

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Acquire a buffer header. Acquiring a buffer header increases a reference counter
 * on it and makes sure that the buffer header won't be recycled until all the references
 * to it are gone.
 * This is useful for instance if a component needs to return a buffer header but still needs
 * access to it for some internal processing (e.g. reference frames in video codecs).
 */
void
acquire_buffer_header_ (MMAL_BUFFER_HEADER_T* buffer_);

/**
 * Release a buffer header. Releasing a buffer header will decrease its reference counter
 * and when no more references are left, the buffer header will be recycled by
 * calling its 'release' callback function.
 * If a pre-release callback is set, this will be invoked before calling the buffer's release
 * callback and potentially postpone buffer recycling.
 * Once pre-release is complete the buffer header is recycled with
 * mmal_buffer_header_release_continue.
 */
void
release_buffer_header_ (MMAL_BUFFER_HEADER_T* buffer_);

/**
 * Reset a buffer header. Resets all header variables to default values.
 */
void
reset_buffer_header_ (MMAL_BUFFER_HEADER_T* buffer_);

/**
 * Replicate a buffer header into another one. Replicating a buffer header will not only do
 * an exact copy of all the public fields of the buffer header (including data and alloc_size),
 * but it will also acquire a reference to the source buffer header which will only be
 * released once the replicate has been released.
 */
void
replicate_buffer_header_ (MMAL_BUFFER_HEADER_T* buffer_src_,
                          MMAL_BUFFER_HEADER_T* buffer_dst_);

/// ************************************ NOT IMPLEMENTED ************************************  ///
/// *****************************************************************************************  ///

///**
// * Continue the buffer header release process. This should be called to complete buffer
// * header recycling once all pre-release activity has been completed.
// */
//void
//buffer_header_continue_release_(MMAL_BUFFER_HEADER_T* buffer_)
//{ mmal_buffer_header_release_continue(buffer_); }

///**
// * Set a buffer header pre-release callback. If the callback is NULL, the buffer
// * will be released back into the pool immediately as usual.
// * The callback is invoked just before a buffer is released back into a pool.
// * This is used by clients who need to trigger additional actions before the buffer
// * can finally be released (e.g. wait for a bulk transfer to complete).
// */
//void
//buffer_header_set_callback_(MMAL_BUFFER_HEADER_T* buffer_,
//                            MMAL_BH_PRE_RELEASE_CB_T cb_,
//                            void* userdata_)
//{ mmal_buffer_header_pre_release_cb_set(buffer_, cb_, userdata_); }

///**
// * Lock the data buffer contained in the buffer header in memory. This call does nothing
// * on all platforms except VideoCore where it is needed to pin a buffer in memory before
// * any access to it.
// */
//void
//buffer_header_lock_(MMAL_BUFFER_HEADER_T* buffer_)
//{ if (MMAL_STATUS_T status = mmal_buffer_header_mem_lock(buffer_); status)
//        e_check__(status, "cannot lock a buffer"); }

///**
// * Unlock the data buffer contained in the buffer header. This call does nothing on all
// * platforms except VideoCore where it is needed to un-pin a buffer in memory after any
// * access to it.
// */
//void
//buffer_header_unlock_(MMAL_BUFFER_HEADER_T* buffer_)
//{ mmal_buffer_header_mem_unlock(buffer_); }

};

MMALPP_END

#endif // MMALPP_BUFFER_UTILS_H
