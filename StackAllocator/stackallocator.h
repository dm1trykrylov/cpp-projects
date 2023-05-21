#include <cstdlib>
#include <list>
// ==============StackStorage==============

template <size_t N>
class StackStorage {
 private:
  char* storage_;
  size_t current_idx_;

 public:
  StackStorage() : storage_((char*)malloc(N * sizeof(char))), current_idx_(0) {}
  void* allocate(size_t bytes_count, size_t alignment);
  void deallocate(void*, size_t) {}
  StackStorage& operator=(const StackStorage& other) = delete;
  ~StackStorage() { free(storage_); }
};

template <size_t N>
void* StackStorage<N>::allocate(size_t bytes_count, size_t alignment) {
  size_t offset = current_idx_ % alignment;
  if (offset != 0) {
    current_idx_ += alignment - offset;
  }
  char* position = storage_ + current_idx_;
  current_idx_ += bytes_count;
  return position;
}

// ==============StackAllocator==============

template <typename T, size_t N>
class StackAllocator {
 private:
  StackStorage<N>* storage_;

 public:
  using value_type = T;
  using pointer = T*;
  StackAllocator() noexcept : storage_() {}
  StackAllocator(StackStorage<N>& storage) noexcept : storage_(&storage) {}
  template <typename A>
  StackAllocator(const StackAllocator<A, N>& other) noexcept
      : storage_(other.storage()) {}

  template <typename A>
  struct rebind {
    using value_type = A;
    using other = StackAllocator<A, N>;
  };

  StackStorage<N>* storage() const noexcept { return storage_; }
  template <typename A>
  StackAllocator& operator=(StackAllocator<A, N>& other);

  pointer allocate(size_t bytes_count);
  void deallocate(const pointer, size_t) {}
};

template <typename T, size_t N>
template <typename A>
StackAllocator<T, N>& StackAllocator<T, N>::operator=(
    StackAllocator<A, N>& other) {
  storage_ = other.storage();
  return *this;
}

template <typename T, size_t N>
T* StackAllocator<T, N>::allocate(size_t bytes_count) {
  size_t size = sizeof(value_type);
  return (pointer)(storage_->allocate(bytes_count * size, size));
}

// ==============StackAllocator non-members==============

template <typename A, typename B, size_t N>
bool operator==(const StackAllocator<A, N>& lhs,
                const StackAllocator<B, N>& rhs) noexcept {
  return lhs.storage() == rhs.storage();
}

template <typename A, typename B, size_t N>
bool operator!=(const StackAllocator<A, N>& lhs,
                const StackAllocator<B, N>& rhs) noexcept {
  return lhs.storage() != rhs.storage();
}

// ==============List==============

template<typename T, typename Alloc = std::allocator<T>>
using List = std::list<T, Alloc>;