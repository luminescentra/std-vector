#ifndef OOP_ASSIGNMENTS_VECTOR_VECTOR_H_
#define OOP_ASSIGNMENTS_VECTOR_VECTOR_H_
#define VECTOR_MEMORY_IMPLEMENTED
#include <memory>
#include <iterator>
#include <stdexcept>
#include <exception>
#include <algorithm>
// sd::pair<score, trackid>, decltype(cmp)> queue(cmp);
template <typename T>
class Vector {
 public:
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Reference = T&;
  using ConstReference = const T&;
  using SizeType = size_t;
  using Iterator = Pointer;
  using ConstIterator = ConstPointer;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

 private:
  template <class Iter>
  using EnableIfForwardIter = std::enable_if_t<
      std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>>;

 public:
  Vector() noexcept = default;
  Vector(const std::initializer_list<T>& init_lst) {
    auto begin = init_lst.begin();
    auto end = init_lst.end();
    size_ = 0;
    capacity_ = 0;
    buffer_ = nullptr;
    size_t i = 0;
    try {
      if (end != begin) {
        size_ = capacity_ = end - begin;
        buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
        while (begin != end) {
          new (buffer_ + i) T(*begin);
          ++begin;
          ++i;
        }
      }
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t j = i; j >= 1; --j) {
          (buffer_ + j - 1)->~T();
        }
        operator delete(buffer_);
      }
      size_ = capacity_ = 0;
      buffer_ = nullptr;
      throw;
    }
  }

  /// Vector(std::initializer_list<T>&& init_lst);

  template <typename InputIterator, typename = EnableIfForwardIter<InputIterator>>
  Vector(InputIterator begin, InputIterator end) {
    size_ = 0;
    capacity_ = 0;
    buffer_ = nullptr;
    size_t i = 0;
    try {
      if (end != begin) {
        size_ = capacity_ = end - begin;
        buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
        while (begin != end) {
          new (buffer_ + i) T(*begin);
          ++begin;
          ++i;
        }
      }
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t j = i; j >= 1; --j) {
          (buffer_ + j - 1)->~T();
        }
        operator delete(buffer_);
      }
      size_ = capacity_ = 0;
      buffer_ = nullptr;
      throw;
    }
  }

  Vector(const Vector& other) {
    size_t i = 0;
    try {
      for (; i < size_; i++) {
        (buffer_ + i)->~T();
      }
      operator delete(buffer_);
      buffer_ = nullptr;
      if (other.buffer_ == nullptr) {
        size_ = 0;
        capacity_ = 0;
        return;
      }
      capacity_ = other.capacity_;
      size_ = other.size_;
      buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
      i = 0;
      for (; i < size_; ++i) {
        new (buffer_ + i) T(other.buffer_[i]);
      }
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t j = i; j >= 1; --j) {
          (buffer_ + j - 1)->~T();
        }
        operator delete(buffer_);
      }
      size_ = capacity_ = 0;
      buffer_ = nullptr;
      throw;
    }
  }

  Vector(Vector&& other) noexcept
      : buffer_(std::exchange(other.buffer_, nullptr))
      , size_(std::exchange(other.size_, 0))
      , capacity_(std::exchange(other.capacity_, 0)) {
  }

  explicit Vector(size_t size) {
    size_t i = 0;
    size_ = 0;
    capacity_ = 0;
    buffer_ = nullptr;
    try {
      if (size != 0) {
        size_ = size;
        capacity_ = size_;
        buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
        for (; i < capacity_; ++i) {
          new (buffer_ + i) T;
        }
      }
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t j = i; j >= 1; --j) {
          (buffer_ + j - 1)->~T();
        }
        operator delete(buffer_);
      }
      size_ = capacity_ = 0;
      buffer_ = nullptr;
      throw;
    }
  }

  Vector(size_t size, const T& value) {
    size_t i = 0;
    size_ = 0;
    capacity_ = 0;
    buffer_ = nullptr;
    try {
      if (size != 0) {
        size_ = size;
        capacity_ = size_;
        buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
        for (; i < capacity_; ++i) {
          new (buffer_ + i) T(value);
        }
      }
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t j = i; j >= 1; --j) {
          (buffer_ + j - 1)->~T();
        }
        operator delete(buffer_);
      }
      size_ = capacity_ = 0;
      buffer_ = nullptr;
      throw;
    }
  }

  Vector& operator=(const Vector& other) {
    if (this != &other) {
      auto backup_buff = std::exchange(buffer_, nullptr);
      auto backup_size = std::exchange(size_, 0);
      auto backup_capacity = std::exchange(capacity_, 0);
      try {
        Vector(other).Swap(*this);
        for (size_t i = 0; i < backup_size; i++) {
          (backup_buff + i)->~T();
        }
        operator delete(backup_buff);
      } catch (...) {
        buffer_ = backup_buff;
        size_ = backup_size;
        capacity_ = backup_capacity;
        throw;
      }
    }
    return *this;
  }

  Vector& operator=(Vector&& other) noexcept {
    if (this != &other) {
      Vector(std::move(other)).Swap(*this);
    }
    return *this;
  }

  ~Vector() noexcept(std::is_nothrow_destructible_v<ValueType>) {
    for (size_t i = 0; i < size_; ++i) {
      (buffer_ + i)->~T();
    }
    operator delete(buffer_);
    buffer_ = nullptr;
  }

  [[nodiscard]] SizeType Size() const noexcept {
    return size_;
  }

  [[nodiscard]] SizeType Capacity() const noexcept {
    return capacity_;
  }

  [[nodiscard]] bool Empty() const noexcept {
    return size_ == 0;
  }

  [[nodiscard]] ConstReference Front() const noexcept {
    return buffer_[0];
  }

  [[nodiscard]] Reference Front() noexcept {
    return buffer_[0];
  }

  [[nodiscard]] ConstReference Back() const noexcept {
    return buffer_[size_ - 1];
  }

  [[nodiscard]] Reference Back() noexcept {
    return buffer_[size_ - 1];
  }

  [[nodiscard]] ConstReference operator[](size_t idx) const noexcept {
    return buffer_[idx];
  }

  [[nodiscard]] Reference operator[](size_t idx) noexcept {
    return buffer_[idx];
  }

  [[nodiscard]] ConstReference At(size_t idx) const {
    if (idx >= size_) {
      throw std::out_of_range("");
    }
    return (*this)[idx];
  }

  [[nodiscard]] Reference At(size_t idx) {
    if (idx >= size_) {
      throw std::out_of_range("");
    }
    return (*this)[idx];
  }

  [[nodiscard]] ConstPointer Data() const noexcept {
    return buffer_;
  }

  [[nodiscard]] Pointer Data() noexcept {
    return buffer_;
  }

  void Swap(Vector& other) noexcept {
    std::swap(buffer_, other.buffer_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  void Resize(size_t size, const T& value) {
    auto backup_buff = buffer_;
    auto backup_size = size_;
    auto backup_capacity = capacity_;
    if (size_ == size) {
      return;
    }
    try {
      if (size > capacity_) {
        buffer_ = nullptr;
        capacity_ = size;
        if (backup_buff == nullptr) {
          size_t i = 0;
          try {
            buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
            for (; i < size; ++i) {
              new (buffer_ + i) T(value);
            }
          } catch (...) {
            if (buffer_ != nullptr) {
              for (size_t j = i; j >= 1; --j) {
                (buffer_ + j - 1)->~T();
              }
              operator delete(buffer_);
            }
            buffer_ = nullptr;
            throw;
          }
        } else {
          size_t i = 0;
          try {
            buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
            for (; i < size; ++i) {  // 0
              new (buffer_ + i) T;
            }
          } catch (...) {
            if (buffer_ != nullptr) {
              for (size_t j = i; j >= 1; --j) {
                (buffer_ + j - 1)->~T();
              }
              operator delete(buffer_);
            }
            buffer_ = nullptr;
            throw;
          }
          for (i = backup_size; i < size; ++i) {
            *(buffer_ + i) = T(value);
          }
          for (i = 0; i < backup_size; ++i) {
            *(buffer_ + i) = T(std::move(backup_buff[i]));
          }
          for (i = 0; i < backup_size; ++i) {
            (backup_buff + i)->~T();
          }
          operator delete(backup_buff);
        }
      } else {
        if (size > size_) {
          try {
            for (size_t i = size_; i < size; ++i) {
              *(buffer_ + i) = T(value);
            }
          } catch (...) {
            buffer_ = nullptr;
          }
        } else {
          for (size_t i = size; i < size_; ++i) {
            (buffer_ + i)->~T();
            // new (buffer_+i) T;
          }
          // capacity_ = size;
        }
      }
      size_ = size;
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {
          (buffer_ + i)->~T();
        }
        operator delete(buffer_);
      }
      buffer_ = backup_buff;
      size_ = backup_size;
      capacity_ = backup_capacity;
      throw;
    }
  }

  void Resize(size_t size) {
    auto backup_buff = buffer_;
    auto backup_size = size_;
    auto backup_capacity = capacity_;
    try {
      if (size > capacity_) {
        buffer_ = nullptr;
        capacity_ = size;
        if (backup_buff == nullptr) {
          size_t i = 0;
          try {
            buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
            for (; i < size; ++i) {
              new (buffer_ + i) T;
            }
          } catch (...) {
            if (buffer_ != nullptr) {
              for (size_t j = i; j >= 1; --j) {
                (buffer_ + j - 1)->~T();
              }
              operator delete(buffer_);
            }
            buffer_ = nullptr;
            throw;
          }
        } else {
          size_t i = 0;
          try {
            buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
            for (; i < size; ++i) {  // 0
              new (buffer_ + i) T;
            }
          } catch (...) {
            if (buffer_ != nullptr) {
              for (size_t j = i; j >= 1; --j) {
                (buffer_ + j - 1)->~T();
              }
              operator delete(buffer_);
            }
            buffer_ = nullptr;
            throw;
          }
          for (i = 0; i < backup_size; ++i) {
            *(buffer_ + i) = T(std::move(backup_buff[i]));
          }
          for (i = 0; i < backup_size; ++i) {
            (backup_buff + i)->~T();
          }
          operator delete(backup_buff);
        }
      } else {
        if (size > size_) {
          for (size_t i = size_; i < size; ++i) {
            new (buffer_ + i) T;
          }
          size_ = size;
          return;
        }
        for (size_t i = size; i < size_; ++i) {
          (buffer_ + i)->~T();
          // new (buffer_ + i) T;
        }
      }
      size_ = size;
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {
          (buffer_ + i)->~T();
        }
        operator delete(buffer_);
      }
      buffer_ = backup_buff;
      size_ = backup_size;
      capacity_ = backup_capacity;
      throw;
    }
  }

  void Reserve(size_t capacity) {
    if (capacity <= capacity_) {
      return;
    }
    auto backup_buff = buffer_;
    auto backup_size = size_;
    auto backup_capacity = capacity_;
    try {
      capacity_ = capacity;
      buffer_ = nullptr;
      if (backup_buff == nullptr) {
        size_t i = 0;
        try {
          buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
          // for (; i < capacity_; ++i) {
          // new (buffer_ + i) T;
          //}
        } catch (...) {
          if (buffer_ != nullptr) {
            for (size_t j = i; j >= 1; --j) {
              (buffer_ + j - 1)->~T();
            }
            operator delete(buffer_);
          }
          buffer_ = nullptr;
          throw;
        }
      } else {
        size_t i = 0;
        try {
          buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
          for (; i < size_; ++i) {  // 0
            new (buffer_ + i) T;
          }
        } catch (...) {
          if (buffer_ != nullptr) {
            for (size_t j = i; j >= 1; --j) {
              (buffer_ + j - 1)->~T();
            }
            operator delete(buffer_);
          }
          buffer_ = nullptr;
          throw;
        }
        for (i = 0; i < backup_size; ++i) {
          *(buffer_ + i) = T(std::move(backup_buff[i]));
        }
        for (i = 0; i < backup_size; ++i) {
          (backup_buff + i)->~T();
        }
        operator delete(backup_buff);
      }
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {
          (buffer_ + i)->~T();
        }
        operator delete(buffer_);
      }
      buffer_ = backup_buff;
      size_ = backup_size;
      capacity_ = backup_capacity;
      throw;
    }
  }

  void ShrinkToFit() {
    if (capacity_ == size_) {
      return;
    }
    if (size_ == 0) {
      try {
        // for (size_t i = 0; i < capacity_; ++i) {
        //(buffer_ + i)->~T();
        //}
        capacity_ = size_;
        operator delete(buffer_);
        buffer_ = nullptr;
        return;
      } catch (...) {
        buffer_ = nullptr;
        throw;
      }
    }
    auto backup_buff = buffer_;
    auto backup_size = size_;
    auto backup_capacity = capacity_;
    buffer_ = nullptr;
    try {
      capacity_ = size_;
      size_t i = 0;
      try {
        buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
        for (; i < backup_size; ++i) {  // 0
          new (buffer_ + i) T;
        }
      } catch (...) {
        if (buffer_ != nullptr) {
          for (size_t j = i; j >= 1; --j) {
            (buffer_ + j - 1)->~T();
          }
          operator delete(buffer_);
        }
        buffer_ = nullptr;
        throw;
      }
      for (i = 0; i < backup_size; ++i) {
        *(buffer_ + i) = T(std::move(backup_buff[i]));
      }
      for (i = 0; i < backup_size; ++i) {
        (backup_buff + i)->~T();
      }
      operator delete(backup_buff);
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {  // 0
          (buffer_ + i)->~T();
        }
        operator delete(buffer_);
      }
      buffer_ = backup_buff;
      size_ = backup_size;
      capacity_ = backup_capacity;
      throw;
    }
  }

  void Clear() {
    for (size_t i = 0; i < size_; ++i) {
      (buffer_ + i)->~T();
    }
    size_ = 0;
    operator delete(buffer_);
    buffer_ = nullptr;
    capacity_ = 0;
  }

  template <typename... Args>
  void EmplaceBack(Args... args) {
    auto backup_buff = buffer_;
    auto backup_size = size_;
    auto backup_capacity = capacity_;
    if (capacity_ > size_) {
      new (buffer_ + size_) T(std::forward<Args>(args)...);
      ++size_;
      return;
    }
    try {
      if (capacity_ == 0) {
        capacity_ = 1;
      } else {
        capacity_ *= 2;
      }
      buffer_ = nullptr;
      size_t i = 0;
      try {
        buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
        for (; i < size_ + 1; ++i) {  // 0
          new (buffer_ + i) T;
        }
      } catch (...) {
        if (buffer_ != nullptr) {
          for (size_t j = i; j >= 1; --j) {
            (buffer_ + j - 1)->~T();
          }
          operator delete(buffer_);
        }
        buffer_ = nullptr;
        throw;
      }
      for (i = 0; i < backup_size; ++i) {
        *(buffer_ + i) = T(std::move(backup_buff[i]));
      }
      *(buffer_ + size_) = T(std::forward<Args>(args)...);
      ++size_;
      for (i = 0; i < backup_size; ++i) {
        (backup_buff + i)->~T();
      }
      operator delete(backup_buff);
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {  // 0
          (buffer_ + i)->~T();
        }
        operator delete(buffer_);
      }
      buffer_ = backup_buff;
      size_ = backup_size;
      capacity_ = backup_capacity;
      throw;
    }
  }

  void PushBack(const T& value) {
    auto backup_buff = buffer_;
    auto backup_size = size_;
    auto backup_capacity = capacity_;
    if (capacity_ > size_) {
      new (buffer_ + size_) T(value);
      ++size_;
      return;
    }
    try {
      if (capacity_ == 0) {
        capacity_ = 1;
      } else {
        capacity_ *= 2;
      }
      buffer_ = nullptr;
      size_t i = 0;
      try {
        buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
        for (; i < size_ + 1; ++i) {  // 0
          new (buffer_ + i) T;
        }
      } catch (...) {
        if (buffer_ != nullptr) {
          for (size_t j = i; j >= 1; --j) {
            (buffer_ + j - 1)->~T();
          }
          operator delete(buffer_);
        }
        buffer_ = nullptr;
        throw;
      }
      for (i = 0; i < backup_size; ++i) {
        *(buffer_ + i) = T(std::move(backup_buff[i]));
      }
      *(buffer_ + size_) = T(value);
      ++size_;
      for (i = 0; i < backup_size; ++i) {
        (backup_buff + i)->~T();
      }
      operator delete(backup_buff);
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {  // 0
          (buffer_ + i)->~T();
        }
        operator delete(buffer_);
      }
      buffer_ = backup_buff;
      size_ = backup_size;
      capacity_ = backup_capacity;
      throw;
    }
  }

  void PushBack(T&& value) {
    auto backup_buff = buffer_;
    auto backup_size = size_;
    auto backup_capacity = capacity_;
    if (capacity_ > size_) {
      new (buffer_ + size_) T(std::move(value));
      ++size_;
      return;
    }
    try {
      if (capacity_ == 0) {
        capacity_ = 1;
      } else {
        capacity_ *= 2;
      }
      buffer_ = nullptr;
      size_t i = 0;
      try {
        buffer_ = static_cast<T*>(operator new(sizeof(T) * capacity_));
        for (; i < size_ + 1; ++i) {  // 0
          new (buffer_ + i) T;
        }
      } catch (...) {
        if (buffer_ != nullptr) {
          for (size_t j = i; j >= 1; --j) {
            (buffer_ + j - 1)->~T();
          }
          operator delete(buffer_);
        }
        buffer_ = nullptr;
        throw;
      }
      for (i = 0; i < backup_size; ++i) {
        *(buffer_ + i) = T(std::move(backup_buff[i]));
      }
      *(buffer_ + size_) = T(std::move(value));
      ++size_;
      for (i = 0; i < backup_size; ++i) {
        (backup_buff + i)->~T();
      }
      operator delete(backup_buff);
    } catch (...) {
      if (buffer_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {  // 0
          (buffer_ + i)->~T();
        }
        operator delete(buffer_);
      }
      buffer_ = backup_buff;
      size_ = backup_size;
      capacity_ = backup_capacity;
      throw;
    }
  }

  void PopBack() noexcept(std::is_nothrow_destructible_v<ValueType>) {
    auto backup_buff = buffer_;
    auto backup_size = size_;
    auto backup_capacity = capacity_;
    try {
      if (size_ == 0) {
        return;
      }
      --size_;
      (buffer_ + size_)->~T();
    } catch (...) {
      buffer_ = backup_buff;
      size_ = backup_size;
      capacity_ = backup_capacity;
      // throw;
    }
  }

  [[nodiscard]] ConstIterator cbegin() const noexcept {  // NOLINT
    return Data();
  }

  [[nodiscard]] ConstIterator begin() const noexcept {  // NOLINT
    return cbegin();
  }

  [[nodiscard]] Iterator begin() noexcept {  // NOLINT
    return Data();
  }

  [[nodiscard]] ConstIterator cend() const noexcept {  // NOLINT
    return Data() + size_;
  }

  [[nodiscard]] ConstIterator end() const noexcept {  // NOLINT
    return cend();
  }

  [[nodiscard]] Iterator end() noexcept {  // NOLINT
    return Data() + size_;
  }

  [[nodiscard]] ConstReverseIterator crbegin() const noexcept {  // NOLINT
    return ConstReverseIterator(cend());
  }

  [[nodiscard]] ConstReverseIterator rbegin() const noexcept {  // NOLINT
    return crbegin();
  }

  [[nodiscard]] ReverseIterator rbegin() noexcept {  // NOLINT
    return ReverseIterator(end());
  }

  [[nodiscard]] ConstReverseIterator crend() const noexcept {  // NOLINT
    return ConstReverseIterator(cbegin());
  }

  [[nodiscard]] ConstReverseIterator rend() const noexcept {  // NOLINT
    return crend();
  }

  [[nodiscard]] ReverseIterator rend() noexcept {  // NOLINT
    return ReverseIterator(begin());
  }

 private:
 private:
  T* buffer_{nullptr};
  size_t size_{0};
  size_t capacity_{0};
};

template <typename T>
[[nodiscard]] bool operator==(const Vector<T>& a, const Vector<T>& b) noexcept {
  if (a.Size() != b.Size()) {
    return false;
  }
  if (a.Data() == nullptr) {
    return true;
  }
  for (size_t i = 0; i < a.Size(); ++i) {
    if (a.Data()[i] != b.Data()[i]) {
      return false;
    }
  }
  return true;
}

template <typename T>
[[nodiscard]] bool operator<(const Vector<T>& a, const Vector<T>& b) noexcept {
  if (a.Empty() && b.Empty()) {
    return false;
  }
  if (a.Empty()) {
    return true;
  }
  if (b.Empty()) {
    return false;
  }
  size_t n = std::min(b.Size(), a.Size());
  size_t i = 0;
  bool flag = false;
  for (; i < n; ++i) {
    if (a.Data()[i] != b.Data()[i]) {
      flag = true;
      break;
    }
  }
  if (!flag && b.Size() <= a.Size()) {
    return false;
  }
  if (!flag && b.Size() > a.Size()) {
    return true;
  }
  return a.Data()[i] < b.Data()[i];
}

template <typename T>
[[nodiscard]] inline bool operator!=(const Vector<T>& a, const Vector<T>& b) noexcept {
  return !(a == b);
}

template <typename T>
[[nodiscard]] inline bool operator<=(const Vector<T>& a, const Vector<T>& b) noexcept {
  return (a < b || a == b);
}

template <typename T>
[[nodiscard]] inline bool operator>(const Vector<T>& a, const Vector<T>& b) noexcept {
  return !(a <= b);
}

template <typename T>
[[nodiscard]] inline bool operator>=(const Vector<T>& a, const Vector<T>& b) noexcept {
  return !(a < b);
}

#endif  // OOP_ASSIGNMENTS_VECTOR_VECTOR_H_
