#include <stdexcept>
#include <type_traits>
#include <vector>

template <typename T, typename Alloc = std::allocator<T>>
class Deque {
 public:
  typedef T value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef std::vector<pointer> map_type;
  typedef size_t size_type;
  typedef int32_t difference_type;
  // typedef Alloc dataAllocator;  // allocates memory for the chunk

  template <bool isConst>
  struct common_iterator {
    common_iterator(std::conditional_t<isConst, const Deque&, Deque&> deque)
        : common_iterator<isConst>(deque, 0) {}
    common_iterator(std::conditional_t<isConst, const Deque&, Deque&> deque,
                    size_type pos)
        : deque_(deque), pos_(pos) {}

    common_iterator(const common_iterator<false>& other)
        : deque_(other.IDeque()), pos_(other.Pos()) {}
    common_iterator& operator++() {
      ++pos_;
      return *this;
    }
    common_iterator& operator--() {
      --pos_;
      return *this;
    }
    common_iterator& operator+=(difference_type offset) {
      pos_ += offset;
      return *this;
    }
    common_iterator& operator-=(difference_type offset) {
      pos_ -= offset;
      return *this;
    }
    common_iterator operator-(difference_type offset) {
      auto tmp = *this;
      tmp.pos_ -= offset;
      return tmp;
    }
    common_iterator operator+(difference_type offset) {
      auto tmp = *this;
      tmp.pos_ += offset;
      return tmp;
    }

    bool operator<(const common_iterator& other) const noexcept {
      return pos_ < other.pos_;
    }

    bool operator>(const common_iterator& other) const noexcept {
      return pos_ > other.pos_;
    }

    bool operator<=(const common_iterator& other) const noexcept {
      return pos_ <= other.pos_;
    }

    bool operator>=(const common_iterator& other) const noexcept {
      return pos_ >= other.pos_;
    }

    bool operator!=(const common_iterator& other) const noexcept {
      return pos_ != other.pos_;
    }

    bool operator==(const common_iterator& other) const noexcept {
      return pos_ == other.pos_;
    }

    difference_type operator-(const common_iterator& other) const noexcept {
      return static_cast<difference_type>(pos_) -
             static_cast<difference_type>(other.pos_);
    }
    // reference operator*() { return deque_[pos_]; }
    // const_reference operator*() const { return deque_[pos_]; }
    std::conditional_t<isConst, const_reference, reference> operator*() {
      return deque_[pos_];
    }
    // pointer operator->() { return deque_.at_ptr(pos_); }
    // const_pointer operator->() const { return deque_.at_ptr(pos_); }
    std::conditional_t<isConst, const_pointer, pointer> operator->() {
      return deque_.at_ptr(pos_);
    }
    constexpr size_type Pos() const noexcept { return pos_; }
    std::conditional_t<isConst, const Deque&, Deque&> IDeque() const {
      return deque_;
    }

   private:
    std::conditional_t<isConst, const Deque&, Deque&> deque_;
    size_t pos_;
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;

 private:
  static const size_t kChunkSize = 16;
  map_type map_;
  size_type begin_idx_;
  size_type end_idx_;
  size_type begin_pos_;
  size_type end_pos_;
  size_type size_;

  // Alloc alloc_;
  std::allocator<T> alloc_;

 public:
  Deque() : begin_idx_(0), end_idx_(0), begin_pos_(0), end_pos_(0), size_(0) {}

  Deque(size_type count, const T& value = T()) : Deque() {
    for (size_t i = 0; i < count; ++i) {
      push_back(value);
    }
  }
  Deque(const Deque& other) {
    for (size_t i = 0; i < other.size(); ++i) {
      push_back(other[i]);
    }
  }
  Deque& operator=(const Deque& other) {
    clear();
    for (size_t i = 0; i < other.size(); ++i) {
      push_back(other[i]);
    }
    return *this;
  }
  ~Deque() {
    for (size_t i = 0; i < map_.size(); ++i) {
      alloc_.deallocate(map_[i], kChunkSize);
    }
  }

  void push_front(const T& value);
  void pop_front();
  void push_back(const T& value);
  void pop_back();

  size_t size() const { return size_; }
  reference front();
  reference back();
  reference operator[](size_type pos);
  const_reference operator[](size_type pos) const;
  reference at(size_type pos);
  pointer at_ptr(size_type pos);
  const_pointer at_ptr(size_type pos) const;
  const_reference at(size_type pos) const;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  iterator insert(const_iterator pos, const T& value);
  iterator erase(const_iterator pos);

  void reallocate();
  void clear() {
    size_t sz = size_;
    for (size_t i = 0; i < sz; ++i) {
      pop_back();
    }
  }
};

template <typename T, typename Alloc>
void Deque<T, Alloc>::push_front(const T& value) {
  if (map_.size() == 0) {
    reallocate();
    begin_pos_ = kChunkSize - 1;
    end_pos_ = begin_pos_;
  } else {
    if (begin_pos_ > 0) {
      --begin_pos_;
    } else {
      if (begin_idx_ == 0) {
        reallocate();
      }
      --begin_idx_;
      begin_pos_ = kChunkSize - 1;
    }
  }
  ++size_;
  alloc_.construct(map_[begin_idx_] + begin_pos_, T(value));
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::pop_front() {
  pointer to_destroy = map_[begin_idx_] + begin_pos_;
  if (begin_pos_ < kChunkSize - 1) {
    ++begin_pos_;
  } else {
    if (begin_idx_ < end_idx_) {
      ++begin_idx_;
      begin_pos_ = 0;
    }
  }
  --size_;
  alloc_.destroy(to_destroy);
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::push_back(const T& value) {
  if (map_.size() == 0) {
    reallocate();
    begin_pos_ = kChunkSize - 1;
    end_pos_ = begin_pos_;
  } else {
    if (end_pos_ < kChunkSize - 1) {
      ++end_pos_;
    } else {
      if (end_idx_ == map_.size() - 1) {
        reallocate();
      }
      ++end_idx_;
      end_pos_ = 0;
    }
  }
  ++size_;
  alloc_.construct(map_[end_idx_] + end_pos_, T(value));
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::pop_back() {
  pointer to_destroy = map_[end_idx_] + end_pos_;
  if (end_pos_ > 0) {
    --end_pos_;
  } else {
    if (begin_idx_ < end_idx_) {
      --end_idx_;
      begin_pos_ = 0;
    }
  }
  --size_;
  alloc_.destroy(to_destroy);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::reference Deque<T, Alloc>::front() {
  return map_[begin_idx_][begin_pos_];
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::reference Deque<T, Alloc>::back() {
  return map_[end_idx_][end_pos_];
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::reference Deque<T, Alloc>::operator[](size_type pos) {
  size_t idx = begin_pos_ + pos;
  size_t map_idx = begin_idx_ + idx / kChunkSize;
  size_t chunk_idx = idx % kChunkSize;
  return map_[map_idx][chunk_idx];
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::const_reference Deque<T, Alloc>::operator[](
    size_type pos) const {
  size_t idx = begin_pos_ + pos;
  size_t map_idx = begin_idx_ + idx / kChunkSize;
  size_t chunk_idx = idx % kChunkSize;
  return map_[map_idx][chunk_idx];
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::reference Deque<T, Alloc>::at(size_type pos) {
  if (pos >= size_) {
    throw std::out_of_range("Deque index out of range");
  }
  return this->operator[](pos);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::pointer Deque<T, Alloc>::at_ptr(size_type pos) {
  return &this->operator[](pos);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::const_pointer Deque<T, Alloc>::at_ptr(
    size_type pos) const {
  return &this->operator[](pos);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::const_reference Deque<T, Alloc>::at(
    size_type pos) const {
  if (pos >= size_) {
    throw std::out_of_range("Deque index out of range");
  }
  return this->operator[](pos);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::iterator Deque<T, Alloc>::begin() {
  return iterator(*this, 0);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::iterator Deque<T, Alloc>::end() {
  return iterator(*this, size_);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::const_iterator Deque<T, Alloc>::begin() const {
  return const_iterator(*this, 0);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::const_iterator Deque<T, Alloc>::end() const {
  return const_iterator(*this, size_);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::const_iterator Deque<T, Alloc>::cbegin() const {
  return const_iterator(*this, 0);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::const_iterator Deque<T, Alloc>::cend() const {
  return const_iterator(*this, size_);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::iterator Deque<T, Alloc>::insert(const_iterator pos,
                                                           const T& value) {
  push_back(back());
  size_t insert_pos = pos.Pos();
  for (size_t i = size_ - 1; i > insert_pos; --i) {
    alloc_.construct(&this->operator[](i), T(this->operator[](i - 1)));
  }
  alloc_.construct(&this->operator[](insert_pos), T(value));
  return iterator(*this, insert_pos + 1);
}

template <typename T, typename Alloc>
typename Deque<T, Alloc>::iterator Deque<T, Alloc>::erase(const_iterator pos) {
  size_t erase_pos = pos.Pos();
  alloc_.destroy(&this->operator[](erase_pos));
  for (size_t i = erase_pos; i < size_ - 1; ++i) {
    alloc_.construct(&this->operator[](i), T(this->operator[](i + 1)));
  }
  // alloc_.deallocate(&this->operator[](size_ - 1));
  return iterator(*this, erase_pos + 1);
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::reallocate() {
  size_t old_size = map_.size();
  if (old_size == 0) {
    ++old_size;
  }
  map_type new_map(old_size * 3);
  // allocate memory and copy existing data
  for (size_t i = 0; i < new_map.size(); ++i) {
    if (i < map_.size() || i >= map_.size() * 2) {
      new_map[i] = alloc_.allocate(kChunkSize);
    } else {
      new_map[i] = map_[i - map_.size()];
    }
  }
  begin_idx_ += old_size;
  end_idx_ += old_size;
  map_ = new_map;
}
