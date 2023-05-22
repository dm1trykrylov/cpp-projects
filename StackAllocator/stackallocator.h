#include <cstddef>
#include <cstdlib>
#include <list>
#include <memory>
#include <type_traits>
// ==============StackStorage==============

template <size_t N>
class StackStorage {
 private:
  char* storage_;
  size_t current_idx_;

 public:
  StackStorage() : storage_((char*)malloc(N * sizeof(char))), current_idx_() {}

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
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;

  // StackAllocator() noexcept : storage_(new StackStorage<N>()) {}

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

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode* prev;
    BaseNode* next;
    explicit BaseNode() : prev(nullptr), next(nullptr) {}
    explicit BaseNode(BaseNode* prev, BaseNode* next)
        : prev(prev), next(next) {}
  };
  struct Node : BaseNode {
    T value;
    Node(BaseNode* prev, BaseNode* next) : BaseNode(prev, next) {}
    Node(const T& value, BaseNode* prev, BaseNode* next)
        : BaseNode(prev, next), value(std::move(value)) {}
  };
  template <bool IsConst>
  class CommonIterator {
   protected:
    BaseNode* node_;

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_type = CommonIterator<IsConst>;
    using value_type = T;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

    CommonIterator(const CommonIterator&) = default;
    CommonIterator(CommonIterator&&) = default;

    template <bool IsConst_ = IsConst, class = std::enable_if_t<IsConst_>>
    CommonIterator(const CommonIterator<false>& other) : node_(other.GetNode()) {}
    explicit CommonIterator(BaseNode* node) : node_(node) {}

    CommonIterator& operator=(const CommonIterator&) = default;
    CommonIterator& operator=(CommonIterator&&) = default;

    BaseNode* GetNode() const { return node_; }

    CommonIterator& operator++() {
      node_ = node_->next;
      return *this;
    }

    CommonIterator operator++(int) {
      CommonIterator copy = *this;
      ++(*this);
      return copy;
    }

    CommonIterator& operator--() {
      node_ = node_->prev;
      return *this;
    }

    CommonIterator operator--(int) {
      CommonIterator copy = *this;
      --(*this);
      return copy;
    }

    bool operator==(const CommonIterator& other) const {
      return node_ == other.node_;
    }

    bool operator!=(const CommonIterator& other) const {
      return node_ != other.node_;
    }

    pointer operator->() const {
      return std::addressof<pointer>(static_cast<Node*>(node_)->value);
    }

    reference operator*() const { return static_cast<Node*>(node_)->value; }
  };

 public:
  using value_type = T;
  using allocator_type = typename std::allocator_traits<
      Allocator>::template rebind_alloc<List<T, Allocator>::Node>;
  using allocator_traits = std::allocator_traits<allocator_type>;
  using base_allocator_traits = std::allocator_traits<Allocator>;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;
  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  explicit List(const Allocator& alloc = Allocator());
  List(size_type size, const Allocator& alloc = Allocator());
  List(size_type size, const_reference value,
       const Allocator& alloc = Allocator());
  List(const List& other);
  List(List&& other) noexcept;
  ~List();

  void clear();

  allocator_type get_allocator() const noexcept { return alloc_; }

  List& operator=(const List& other);

  void push_front(const_reference value);
  void push_front(T&& value);
  void push_back(const_reference value);
  void push_back(T&& value);

  void pop_front();
  void pop_back();

  size_type size() const { return size_; }

  iterator begin() { return iterator(head_->next); }
  iterator end() { return iterator(head_); }

  const_iterator begin() const { return const_iterator(head_->next); }
  const_iterator end() const { return const_iterator(head_); }

  const_iterator cbegin() const { return const_iterator(head_->next); }
  const_iterator cend() const { return const_iterator(head_); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(cbegin());
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  iterator insert(const_iterator pos, const_reference value);
  iterator insert(const_iterator pos, T&& value);
  void erase(const_iterator pos);

 private:
  typename std::allocator_traits<Allocator>::template rebind_alloc<
      List<T, Allocator>::Node>
      alloc_;
  BaseNode* head_;
  size_type size_;
};

template <typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& alloc) : alloc_(alloc), size_(0) {
  head_ = static_cast<BaseNode*>(allocator_traits::allocate(alloc_, 1));
  head_->next = head_;
  head_->prev = head_;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_type size, const Allocator& alloc) : List(alloc) {
  Node* current = static_cast<Node*>(head_);
  head_->next = current;
  Node* node;
  try {
    for (size_t i = 0; i < size; ++i) {
      node = static_cast<Node*>(allocator_traits::allocate(alloc_, 1));

      allocator_traits::construct(alloc_, node, current, nullptr);

      current->next = node;
      std::swap(current, node);
      ++size_;
    }
  } catch (...) {
    allocator_traits::deallocate(alloc_, node, 1);
    current->next = head_;
    head_->prev = current;
    clear();
    throw;
  }
  current->next = head_;
  head_->prev = current;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_type size, const_reference value,
                         const Allocator& alloc)
    : List(alloc) {
  Node* current = static_cast<Node*>(head_);
  head_->next = current;
  Node* node;
  try {
    for (size_t i = 0; i < size; ++i) {
      node = static_cast<Node*>(allocator_traits::allocate(alloc_, 1));
      allocator_traits::construct(alloc_, node, value, nullptr, nullptr);

      node->prev = current;
      current->next = node;
      current = node;
      ++size_;
    }
  } catch (...) {
    allocator_traits::deallocate(alloc_, node, 1);
    current->next = head_;
    head_->prev = current;
    clear();
    throw;
  }

  current->next = head_;
  head_->prev = current;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other)
    : List(base_allocator_traits::select_on_container_copy_construction(
          other.alloc_)) {
  try {
    for (const auto& value : other) {
      insert(cend(), value);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(List&& other) noexcept
    : List(base_allocator_traits::select_on_container_copy_construction(
          other.alloc_)) {
  head_ = std::move(other.head_);
  size_ = std::move(other.size_);
}

template <typename T, typename Allocator>
void List<T, Allocator>::erase(const_iterator pos) {
  Node* node = static_cast<Node*>(pos.GetNode());
  if (node->prev != nullptr) {
    node->prev->next = node->next;
  }
  if (node->next != nullptr) {
    node->next->prev = node->prev;
  }
  allocator_traits::destroy(alloc_, node);
  allocator_traits::deallocate(alloc_, node, 1);
  --size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::clear() {
  while (size_ > 0) {
    erase(cbegin());
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  clear();
  allocator_traits::deallocate(alloc_, static_cast<Node*>(head_), 1);
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (allocator_traits::propagate_on_container_copy_assignment::value) {
    this->alloc_ = other.alloc_;
  }
  try {
    clear();
    for (const auto& value : other) {
      insert(cbegin(), value);
    }
  } catch (...) {
    clear();
  }
  return *this;
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::insert(
    const_iterator pos, const_reference value) {
  Node* node;
  try {
    node = allocator_traits::allocate(alloc_, 1);
    allocator_traits::construct(alloc_, node, value, nullptr, nullptr);
  } catch (...) {
    allocator_traits::deallocate(alloc_, node, 1);
    throw;
  }
  BaseNode* next_node = pos.GetNode();
  node->next = next_node;
  node->prev = next_node->prev;
  next_node->prev->next = node;
  next_node->prev = node;
  ++size_;
  return iterator(static_cast<BaseNode*>(node));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::insert(
    const_iterator pos, T&& value) {
  Node* node;
  try {
    node = allocator_traits::allocate(alloc_, 1);
    allocator_traits::construct(alloc_, node, std::move(value), nullptr,
                                nullptr);
  } catch (...) {
    allocator_traits::deallocate(alloc_, node, 1);
    throw;
  }
  BaseNode* next_node = pos.GetNode();
  node->next = next_node;
  node->prev = next_node->prev;
  next_node->prev->next = node;
  next_node->prev = node;
  ++size_;
  return iterator(static_cast<BaseNode*>(node));
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const_reference value) {
  insert(cbegin(), value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(T&& value) {
  insert(cbegin(), value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const_reference value) {
  insert(cend(), value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(T&& value) {
  insert(cend(), value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  erase(cbegin());
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  erase(--cend());
}

// non-member functions
