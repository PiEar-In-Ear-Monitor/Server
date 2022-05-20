#ifndef PIEAR_SERVER_RINGBUFFER_H
#define PIEAR_SERVER_RINGBUFFER_H

#include <mutex>

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
        explicit ringBuffer(int size_of_buffer, int size_of_chunk) :
                _size(size_of_buffer),
                _buffer((T*)malloc(sizeof(T) * size_of_buffer)),
                _readIndex(0),
                _writeIndex(0),
                _chunkSize(size_of_chunk),
                _chunkCount(0),
                _mutex() {
            if (_buffer == nullptr) {
                throw std::bad_alloc();
            }
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
                // TODO: Log this
                _readIndex = (_readIndex + _chunkSize) % _size;
                _chunkCount--;
            }
            for (int i = 0; i < _chunkSize; i++) {
                _buffer[_writeIndex] = item[i];
                _writeIndex = (++_writeIndex) % _size;
            }
            _chunkCount++;
        }
        /**
         * Retrieves a chunk from the ring buffer.
         * If the buffer is empty, nullptr is returned.
         * Freeing the returned chunk is the responsibility of the caller.
         * @return The chunk at the read index.
         */
        T* pop() {
            T *item = static_cast<T *>(malloc(sizeof(T) * _chunkSize));
            std::lock_guard<std::mutex> lock(_mutex);
            if (_chunkCount == 0) {
                return nullptr;
            }
            for (int i = 0; i < _chunkSize; i++) {
                item[i] = _buffer[_readIndex];
                _readIndex = (++_readIndex) % _size;
            }
            _chunkCount--;
            return item;
        }
        /**
         * Returns the size of the buffer.
         * @return The size of the buffer.
         */
        [[nodiscard]] int size() const {
            return _size;
        }
        /**
         * Returns the number of chunks in the buffer.
         * @return The number of chunks in the buffer.
         */
        [[nodiscard]] int capacity() const {
            return _size / _chunkSize;
        }
    private:
        const int _size;      //!< Size of the buffer.
        T *_buffer;           //!< Buffer.
        int _readIndex;       //!< Read index.
        int _writeIndex;      //!< Write index.
        std::mutex _mutex;    //!< Mutex.
        const int _chunkSize; //!< Size of each chunk.
        int _chunkCount;      //!< Number of chunks in the buffer.
    };
} // PiEar
#endif //PIEAR_SERVER_RINGBUFFER_H
