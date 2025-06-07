#ifndef BITVECTOR_H
#define BITVECTOR_H

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <nmmintrin.h>
#include <immintrin.h>
#include <sstream>
#include <stdexcept>
#include <simde/x86/avx2.h>
#include <simde/x86/avx512.h>
namespace bowen
{
    template<typename T,unsigned int ALIGN_SIZE=32>
    class MMAllocator {
    public:
        typedef T value_type;
        MMAllocator() noexcept {}

        template<typename U>
        MMAllocator(const MMAllocator<U,ALIGN_SIZE> &) noexcept {}

        T *allocate(std::size_t n) {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
                throw std::bad_alloc();
            }

            void *ptr = _mm_malloc(n * sizeof(T), ALIGN_SIZE);
            if (!ptr) {
                throw std::bad_alloc();
            }
            return static_cast<T *>(ptr);
        }

        void deallocate(T *p, std::size_t) noexcept {
            _mm_free(p);
        }
    };

    typedef uint32_t BitType;
    const int WORD_BITS = static_cast<int>(sizeof(BitType) * 8);
    constexpr int compute_shift(int bits) {
        int shift = 0;
        while (bits > 1) {
            bits >>= 1;
            ++shift;
        }
        return shift;
    }
    static constexpr int WORD_SHIFT = compute_shift(WORD_BITS);
    static_assert((1u << WORD_SHIFT) == WORD_BITS,
                  "WORD_BITS must be a power of two for fast indexing");
    template<typename Allocator = MMAllocator<BitType>>
    class BitReference
    {
    private:
        BitType* m_ptr;
        BitType m_mask;

    public:
        BitReference(BitType* ptr = nullptr, BitType mask = 0)
            : m_ptr(ptr), m_mask(mask) {}

        operator bool() const
        {
            return (*m_ptr & m_mask) != 0;
        }

        BitReference& operator=(bool value)
        {
            if (value)
                *m_ptr |= m_mask;
            else
                *m_ptr &= ~m_mask;
            return *this;
        }

        BitReference& operator=(const BitReference& ref)
        {
            return *this = static_cast<bool>(ref);
        }

        void flip()
        {
            *m_ptr ^= m_mask;
        }
    };
    template<typename Allocator = MMAllocator<BitType>>
    class BitIterator {
        using iterator_category = std::random_access_iterator_tag;
        using value_type = bool;
        using difference_type = std::ptrdiff_t;
        using pointer = void; // Not applicable for bit iterators
        using reference = BitReference<Allocator>;

    private:
        BitType* m_ptr;
        unsigned int m_offset;

    public:
        BitIterator(BitType* ptr = nullptr, unsigned int offset = 0)
            : m_ptr(ptr), m_offset(offset) {}

        reference operator*() const {
            return BitReference<Allocator>(m_ptr, 1UL << m_offset);
        }

        BitIterator& operator++() {
            if (++m_offset == WORD_BITS) {
                m_offset = 0;
                ++m_ptr;
            }
            return *this;
        }

        BitIterator operator++(int) {
            BitIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        BitIterator& operator--() {
            if (m_offset-- == 0) {
                m_offset = WORD_BITS - 1;
                --m_ptr;
            }
            return *this;
        }

        bool operator==(const BitIterator& other) const {
            return m_ptr == other.m_ptr && m_offset == other.m_offset;
        }

        bool operator!=(const BitIterator& other) const {
            return !(*this == other);
        }
    };
    template<typename Allocator = MMAllocator<BitType>>
    class bitvector
    {
    private:
        BitType* m_data;
        size_t m_size;
        size_t m_capacity;
        Allocator m_allocator;

        void allocate_memory(size_t word_count) {
            m_data = m_allocator.allocate(word_count);
        }

        void deallocate_memory() {
            if (m_data) {
                m_allocator.deallocate(m_data, m_capacity);
            }
        }

        static size_t num_words(size_t bits)
        {
            return (bits + WORD_BITS - 1) / WORD_BITS;
        }

    public:
        typedef BitIterator<Allocator> iterator;
        typedef bool value_type;
        typedef size_t size_type;
        typedef BitReference<Allocator> reference;

        bitvector()
            : m_data(nullptr), m_size(0), m_capacity(0) {}

        explicit bitvector(size_t n, bool value = false)
            : m_size(n), m_capacity(num_words(n))
        {
            allocate_memory(m_capacity);
            std::memset(m_data, value ? ~0 : 0, m_capacity * sizeof(BitType));
        }

        bitvector(const bitvector& other)
            : m_size(other.m_size), m_capacity(other.m_capacity)
        {
            allocate_memory(m_capacity);
            std::copy(other.m_data, other.m_data + m_capacity, m_data);
        }

        bitvector& operator=(const bitvector& other)
        {
            if (this != &other)
            {
                deallocate_memory();
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                m_allocator = other.m_allocator;
                allocate_memory(m_capacity);
                std::copy(other.m_data, other.m_data + m_capacity, m_data);
            }
            return *this;
        }

        ~bitvector()
        {
            deallocate_memory();
        }

        reference operator[](size_t pos)
        {
#ifndef BITVECTOR_NO_BOUND_CHECK
            if (pos >= m_size){
                std::stringstream  ss;
                ss << "bitvector index out of range" << "pos: "<< pos << " size: " << m_size << std::endl;
                throw std::out_of_range(ss.str());
            }
#endif

            size_t word_index = pos >> WORD_SHIFT;
            BitType mask = static_cast<BitType>(1) << (pos & (WORD_BITS - 1));
            return BitReference<Allocator>(&m_data[word_index], mask);
        }

        bool operator[](size_t pos) const
        {
#ifndef BITVECTOR_NO_BOUND_CHECK
            if (pos >= m_size){
                std::stringstream  ss;
                ss << "bitvector index out of range" << "pos: "<< pos << " size: " << m_size << std::endl;
                throw std::out_of_range(ss.str());
            }
#endif
            size_t word_index = pos >> WORD_SHIFT;
            BitType mask = static_cast<BitType>(1) << (pos & (WORD_BITS - 1));
            return (m_data[word_index] & mask) != 0;
        }
        inline void set_bit(size_t pos, bool value){
#ifndef BITVECTOR_NO_BOUND_CHECK
            if (pos >= m_size){
                std::stringstream  ss;
                ss << "bitvector index out of range" << "pos: "<< pos << " size: " << m_size << std::endl;
                throw std::out_of_range(ss.str());
            }
#endif
            BitType mask = 1UL << (pos % WORD_BITS);
            BitType * ptr = &m_data[pos / WORD_BITS];
            if (value)
                *ptr |= mask;
            else
                *ptr &= ~mask;
        }
        inline void set_bit_true_unsafe(const size_t& pos) const {
            BitType mask = 1UL << (pos % WORD_BITS);
            BitType * ptr = &m_data[pos / WORD_BITS];
            *ptr |= mask;
        }
        inline void qset_bit_true_6_v2(size_t pos,const size_t stride,const size_t size) const {
                __m256i vStride = _mm256_set1_epi64x(stride);
                __m256i vPos = _mm256_set_epi64x(pos + 3 * stride, pos + 2 * stride, pos + stride, pos);

                // Loop to handle setting bits
                int i = 0;
                int left = size % 4;
                int stride4limit = size - left;
                for (; i < stride4limit; i += 4) {
                    //pos % WORD_BITS
                    __m256i vModPos = _mm256_rem_epi64(vPos, _mm256_set1_epi64x(WORD_BITS));
                    //1UL << (pos % WORD_BITS)
                    __m256i vMask = _mm256_sllv_epi64(_mm256_set1_epi64x(1UL), vModPos);
                    //pos / WORD_BITS
                    __m256i vIdx = _mm256_div_epi64(vPos, _mm256_set1_epi64x(WORD_BITS));

                    // Load, modify, and store back values

                        BitType *ptr0 = &m_data[_mm256_extract_epi64(vIdx, 0)];
                        *ptr0 |= _mm256_extract_epi64(vMask, 0);
                        BitType *ptr1 = &m_data[_mm256_extract_epi64(vIdx, 1)];
                        *ptr1 |= _mm256_extract_epi64(vMask, 1);
                        BitType *ptr2 = &m_data[_mm256_extract_epi64(vIdx, 2)];
                        *ptr2 |= _mm256_extract_epi64(vMask, 2);
                        BitType *ptr3 = &m_data[_mm256_extract_epi64(vIdx, 3)];
                        *ptr3 |= _mm256_extract_epi64(vMask, 3);

                    vPos = _mm256_add_epi64(vPos, vStride);
                }

                __m256i vModPos2 = _mm256_rem_epi64(vPos, _mm256_set1_epi64x(WORD_BITS));
                __m256i vMask2 = _mm256_sllv_epi64(_mm256_set1_epi64x(1UL), vModPos2);
                __m256i vIdx2 = _mm256_div_epi64(vPos, _mm256_set1_epi64x(WORD_BITS));

                // Load, modify, and store back values
                int j = i;

                if (j < size) {
                    BitType *ptr = &m_data[_mm256_extract_epi64(vIdx2, 0)];
                    *ptr |= _mm256_extract_epi64(vMask2, 0);
                } else
                    return;
                ++j;
                if (j < size) {
                    BitType *ptr = &m_data[_mm256_extract_epi64(vIdx2, 1)];
                    *ptr |= _mm256_extract_epi64(vMask2, 1);
                } else
                    return;
                ++j;
                if (j < size) {
                    BitType *ptr = &m_data[_mm256_extract_epi64(vIdx2, 2)];
                    *ptr |= _mm256_extract_epi64(vMask2, 2);
                }



        }
        inline void set_bit_true_6(size_t pos, const size_t stride) {
            //_mm_prefetch((char *) &m_data[pos + 6 * stride / WORD_BITS], _MM_HINT_T0);
            for (int i = 0; i < 6; i++) {
                BitType mask = 1UL << (pos % WORD_BITS);
                BitType *ptr = &m_data[pos / WORD_BITS];
                *ptr |= mask;
                pos += stride;
            }
        }
        void incrementUntilZero(size_t& pos){
            // Ensure the position is within bounds
#ifndef BITVECTOR_NO_BOUND_CHECK
            if (pos >= m_size){
                std::stringstream  ss;
                ss << "bitvector index out of range" << "pos: "<< pos << " size: " << m_size << std::endl;
                throw std::out_of_range(ss.str());
                    return;
            }
#endif
            while (pos < m_size&& pos%WORD_BITS!=0 && (*this)[pos] != 0) // Check if bit at pos is 1
            {
                ++pos; // Increment pos to the next bit
            }
            while(pos < m_size-WORD_BITS&& (*this)[pos] != 0){
                BitType num = m_data[pos / WORD_BITS];
                int oneCounts = _tzcnt_u32(~num);
                if(oneCounts == WORD_BITS){
                    pos+= oneCounts;
                }else{
                    pos+= oneCounts;
                    break;
                }
            }
            // Iterate through the bits starting from the given position
            while (pos < m_size && (*this)[pos] != 0) // Check if bit at pos is 1
            {
                ++pos; // Increment pos to the next bit
            }
        }


        void push_back(bool value)
        {
            if (m_size == m_capacity * WORD_BITS)
            {
                reserve(m_capacity ? m_capacity * WORD_BITS * 2 : WORD_BITS);
            }

            size_t word_index = m_size >> WORD_SHIFT;
            BitType mask = static_cast<BitType>(1)
                           << (m_size & (WORD_BITS - 1));
            if (value)
                m_data[word_index] |= mask;
            else
                m_data[word_index] &= ~mask;
            ++m_size;
        }

        void reserve(size_t new_capacity)
        {
            if (new_capacity > m_capacity * WORD_BITS)
            {
                size_t new_word_count = num_words(new_capacity);

                BitType *new_data = m_allocator.allocate(new_word_count);
                std::copy(m_data, m_data + m_capacity, new_data);
                deallocate_memory();
                m_data = new_data;
                m_capacity = new_word_count;
            }
        }

        void assign(size_t n, bool value)
        {
            if (n > m_capacity * WORD_BITS)
            {
                deallocate_memory();
                m_capacity = num_words(n);
                allocate_memory(m_capacity);
            }
            m_size = n;
            std::memset(m_data, value ? ~0 : 0, m_capacity * sizeof(BitType));
        }

        BitType *data() {
            return m_data;
        }

        size_t size() const
        {
            return m_size;
        }

        bool empty() const
        {
            return m_size == 0;
        }

        iterator begin()
        {
            return iterator(m_data, 0);
        }

        iterator end()
        {
            return iterator(m_data + num_words(m_size), 0);
        }
    };

} // namespace bowen

#endif
