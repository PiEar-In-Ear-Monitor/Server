#ifndef PIEAR_SERVER_RINGBUFFER_H
#define PIEAR_SERVER_RINGBUFFER_H

#include <cstring>
#include <mutex>
#include "logger.h"

namespace PiEar {
    /**
     * Ring buffer implementation.
     * @tparam T
     */
    template<typename T>
    class ringBuffer {
    public:
        /**
         * Constructor for the ring buffer.
         * Note: size_of_chunk is expected to be a factor of size_of_buffer.
         * @param size_of_buffer Size of the buffer.
         * @param size_of_chunk Size of each chunk of the buffer.
         */
        explicit ringBuffer(int size_of_buffer, int size_of_chunk) : // NOLINT(bugprone-easily-swappable-parameters)
                _size(size_of_buffer),
                _buffer((T*)malloc(sizeof(T) * size_of_buffer)),
                _chunkSize(size_of_chunk),
                _mutex() {
            if (_buffer == nullptr) {
                PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Failed to allocate memory for ring buffer.";
                throw std::bad_alloc();
            }
            memset(_buffer, 0, sizeof(T) * size_of_buffer);
        }
        /**
         * Destructor for the ring buffer.
         */
        ~ringBuffer() {
            free(_buffer);
        }
        /**
         * Inserts a chunk into the ring buffer.
         * If the buffer is full, the oldest chunk is overwritten,
         * and the write index is incremented by the chunk size.
         * @param item The chunk to insert.
         */
        void push(const T *item) {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_chunkCount == capacity()) {
                PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::warning) << "Ring buffer is full, overwriting oldest chunk.";
                _readIndex = (_readIndex + _chunkSize) % _size;
                _chunkCount--;
            }
            memcpy(_buffer + _writeIndex, item, sizeof(T) * _chunkSize);
            _writeIndex = (_writeIndex + _chunkSize) % _size;
            _chunkCount++;
        }
        /**
         * Retrieves a chunk from the ring buffer.
         * If the buffer is empty, nullptr is returned.
         * Freeing the returned chunk is the responsibility of the caller.
         * @return The chunk at the read index.
         */
        auto pop() -> T* {
            T *item = static_cast<T *>(malloc(sizeof(T) * _chunkSize));
            std::lock_guard<std::mutex> lock(_mutex);
            if (_chunkCount == 0) {
                free(item);
                return nullptr;
            }
            memcpy(item, _buffer + _readIndex, sizeof(T) * _chunkSize);
            _readIndex = (_readIndex + _chunkSize) % _size;
            _chunkCount--;
            return item;
        }
        /**
         * Returns the size of the buffer in
         * terms of how many objects it can hold.
         * @return The size of the buffer.
         */
        [[nodiscard]] auto size() const -> int {
            return _size;
        }
        /**
         * Returns the number of chunks the buffer can hold.
         * @return The number of chunks.
         */
        [[nodiscard]] auto capacity() const -> int {
            return _size / _chunkSize;
        }
        /**
         * Returns the chunk size in bytes in terms
         * of how many objects it can hold.
         * @return The chunk size.
         */
        [[nodiscard]] auto chunkSize() const -> int {
            return _chunkSize;
        }
    private:
        const int _size;      //!< Size of the buffer.
        T *_buffer;           //!< Buffer.
        int _readIndex = 0;   //!< Read index.
        int _writeIndex = 0;  //!< Write index.
        std::mutex _mutex;    //!< Mutex.
        const int _chunkSize; //!< Size of each chunk.
        int _chunkCount = 0;  //!< Number of chunks in the buffer.
    };
}
#endif //PIEAR_SERVER_RINGBUFFER_H
