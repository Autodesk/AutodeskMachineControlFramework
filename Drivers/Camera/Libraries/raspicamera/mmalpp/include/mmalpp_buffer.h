#ifndef MMALPP_BUFFER_H
#define MMALPP_BUFFER_H

#include <iterator>

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_buffer.h>

#include "utils/mmalpp_buffer_utils.h"
#include "macros.h"

MMALPP_BEGIN

/// Iterator
class Buffer_iterator {

public:

    using value_type = uint8_t;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = uint8_t*;
    using reference = uint8_t&;
    using const_reference = const uint8_t&;

    /// ctor.
    Buffer_iterator(uint8_t* data,
                    uint32_t length,
                    bool end = false);

    /// Pre-increment.
    Buffer_iterator&
    operator++();

    /// Post-increment.
    Buffer_iterator
    operator++(int);

    /// Access non-const.
    reference
    operator*();

    /// Access const.
    const_reference
    operator*() const;

private:
    const pointer data_; /// data ptr
    const std::size_t length_; /// length
    std::size_t pos_; /// actual position

};

/******************************************************************/

/// operator== overload.
inline bool
operator==(const Buffer_iterator& a,
           const Buffer_iterator& b)
{ return &*a == &*b; }

/// operator!= overload.
inline bool
operator!=(const Buffer_iterator& a,
           const Buffer_iterator& b)
{ return !(a == b); }

/******************************************************************/

/// Buffer class.
class Buffer {

public:

    using value_type = uint8_t;
    using reference = uint8_t&;
    using const_reference = const uint8_t&;
    using iterator = Buffer_iterator;
    using const_iterator = const Buffer_iterator;

    /// ctor.
    Buffer(MMAL_BUFFER_HEADER_T* buffer);

    Buffer() = default;
    Buffer(const Buffer&) = default;
    Buffer(Buffer&&) = default;
    Buffer& operator=(Buffer&& ) = default;
    Buffer& operator=(const Buffer& ) = default;

    /**
     * Acquire a buffer header. Acquiring a buffer header increases a reference counter
     * on it and makes sure that the buffer header won't be recycled until all the references
     * to it are gone. If you call acquire you should call release before destroy the object,
     * otherwise it will be memory leak.
     */
    void
    acquire();

    /**
     * Release a buffer header. Use this if you have previously acquired one.
     * Once all references have been released, the buffer will be recycled.
     */
    void
    release();

    /**
     * Copy meta-data of Buffer. It copies presentation timestamp, decoding timestamp,
     * command, flags, type.
     */
    void
    copy_meta(const Buffer& buffer);

    /**
     * Copy all fields from another Buffer. It copies presentation timestamp,
     * decoding timestamp, command, flags, type and data too.
     * This buffer must have sufficient size to store length bytes from the source buffer.
     * This method implicitly sets offset to zero, and length to the number of bytes copied.
     */
    void
    copy_from(const Buffer& buffer);

    /**
     * Replicates the source Buffer. This copies all fields from the source buffer,
     * including the internal data pointer. In other words, after replication this buffer
     * and the source buffer will share the same block of memory for data.
     * The source buffer will also be referenced internally by this buffer and will
     * only be recycled once this buffer is released.
     */
    void
    replicate(const Buffer& src);

    /**
     * Resets all buffer header fields to default values.
     */
    void
    reset();

    /**
     * Get type of Buffer.
     */
    MMAL_BUFFER_HEADER_TYPE_SPECIFIC_T
    type() const;

    /**
     * Check is this Buffer exists.
     */
    bool
    is_null() const;

    /**
     * Get the length of the payload in the Buffer.
     */
    uint32_t
    size() const;

    /**
     * Get flags if this Buffer.
     */
    uint32_t
    flags() const;

    /**
     * Get flags if this Buffer.
     */
    void
    set_flags(uint32_t flags);

    /**
     * Get offset position of this Buffer.
     */
    uint32_t
    offset() const;

    /**
     * Get command of this Buffer. This is usually 0 for buffers returned
     * by an encoder; typically this is only used by buffers sent to the
     * callback of a control port.
     */
    uint32_t
    command() const;

    /**
     * Get decoding timestamp.
     */
    int64_t
    decoding_timestamp() const;

    /**
     * Get presentation timestamp.
     */
    int64_t
    presentation_timestamp() const;

    /**
     * Get allocated size.
     */
    uint32_t
    allocated_size() const;

    /**
     * Get a const pointer to Buffer's data.
     */
    const uint8_t*
    data() const;

    /**
     * Get a pointer to Buffer's data.
     */
    uint8_t*
    data();

    /**
     * Buffer hold data as a byte array.
     * You can have access to data pointer by [] operator.
     * This operator doesn't do bound check.
     */
    uint8_t&
    operator[](uint32_t n);

    /**
     * Buffer hold data as a byte array.
     * You can have access to data pointer by [] operator.
     * This operator doesn't do bound check.
     */
    const uint8_t&
    operator[](uint32_t n) const;

    /**
     * Begin iterator.
     */
    iterator
    begin();

    /**
     * End iterator.
     */
    iterator
    end();

    /**
     * Begin const-iterator.
     */
    const_iterator
    begin() const;

    /**
     * End const-iterator.
     */
    const_iterator
    end() const;

    /**
     * Get the MMAL_BUFFER_HEADER_T pointer.
     */
    MMAL_BUFFER_HEADER_T*
    get() const;

    /**
     * Locks the buffer before access
     */
    void
    lock() const;

    /**
     * Locks the buffer before access
     */
    void
    unlock() const;

private:
    MMAL_BUFFER_HEADER_T* buffer_;

};

/// USED FOR DEBUG
// std::ostream& operator<<(std::ostream& os, const Buffer& b)
// {
//     for (auto i = b.begin(); i != b.end(); ++i)
//         os << *i;
//     return os;
// }

MMALPP_END

#endif // MMALPP_BUFFER_H
