#include <iostream>

#include "deque.h"

template <typename T>
void AskSize(Deque<T>& d) {
  std::cout << "size: " << d.size() << '\n';
}

template <typename T>
void CheckFront(Deque<T>& d) {
  std::cout << "front: " << d.front() << '\n';
}

template <typename T>
void CheckBack(Deque<T>& d) {
  std::cout << "back: " << d.back() << '\n';
}

template <typename T>
void CheckAt(Deque<T>& d, size_t pos) {
  std::cout << '[' << pos << ']' << ' ' << d[pos] << '\n';  
}
template <typename T>
void CheckAtSafe(Deque<T>& d, size_t pos) {
  std::cout << '[' << pos << ']' << ' ' << d.at(pos) << '\n';  
}
template <typename T, class Alloc = std::allocator<T>>
void PrintIt(typename Deque<T, Alloc>::iterator& it) {
  std::cout << "it at " << it.Pos() << ' ' << *it << '\n';  
}

int main() {
  auto d = Deque<size_t>();
  AskSize(d);
  d.push_front(1);
  AskSize(d);
  CheckFront(d);
  CheckBack(d);
  d.push_front(2);
  AskSize(d);
  CheckFront(d);
  CheckAt(d, 0);
  CheckAtSafe(d, 0);
  CheckBack(d);
  auto it = d.begin();
  PrintIt<size_t>(it);
  d.pop_front();
  CheckAt(d, 0);
  CheckAtSafe(d, 0);
  auto it1 = d.begin();
  PrintIt<size_t>(it1);
  // delete d;
  return 0;
}