#ifndef SIMPLE_SHPTR_HPP
#define SIMPLE_SHPTR_HPP

#include <cstddef>

template <typename T>
class SimpleShPtr {
public:
  SimpleShPtr() : ptr_(nullptr), count_(nullptr) {}
  SimpleShPtr(T* ptr) : ptr_(ptr), count_(new size_t(1)) {}

  ~SimpleShPtr() {
    release();
  }

  template <typename R>
  SimpleShPtr(const SimpleShPtr<R>& other)
      : ptr_(other.ptr_), count_(other.count_) {
    if (ptr_) {
      ++(*count_);
    }
  }

  template <typename R>
  SimpleShPtr(SimpleShPtr<R>&& other)
      : ptr_(other.ptr_), count_(other.count_) {
    other.ptr_ = nullptr;
  }


  template <typename R>
  SimpleShPtr& operator=(const SimpleShPtr<R>& other) {
    release();
    ptr_ = other.ptr_;
    count_ = other.count_;
    ++(*count_);
  }

  template <typename R>
  SimpleShPtr& operator=(SimpleShPtr<R>&& other) {
    release();
    ptr_ = other.ptr_;
    count_ = other.count_;
    other.ptr_ = nullptr;
  }

  T* get() const {
    return ptr_;
  }

  T operator*() const {
    return *ptr_;
  }

  T* operator->() const {
    return ptr_;
  }

  explicit operator bool() const noexcept {
    return ptr_ != 0;
  }

private:
  void release() {
    if (ptr_) {
      if (*count_ == 1) {
        delete ptr_;
        delete count_;
      } else {
        --(*count_);
      }
    }
  }

  T* ptr_;
  size_t* count_;
};

#endif
