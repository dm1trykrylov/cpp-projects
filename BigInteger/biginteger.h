#define BUFFER_SIZE 65536
#define BASE_LENGTH 9
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

const unsigned long long BASE = pow(10, BASE_LENGTH);
const unsigned long long MAX_DECIMAL_DIGITS = 10000;
const size_t SIZE = 64;

void parseString(std::string str, std::vector<u_int64_t>& dest);
std::string numToString(u_int64_t number, bool fixW);

class BigInteger {
 private:
  size_t size_;
  std::vector<u_int64_t> digits_;
  bool negative_;

 public:
  inline BigInteger() : BigInteger(0){};
  inline BigInteger(int64_t number);  // bigint from number

  inline BigInteger(int64_t number, size_t len);

  inline BigInteger(std::string str);  // bigint from string

  inline BigInteger(const BigInteger& other, bool negative);

  explicit operator bool() const;

  explicit operator double() const;

  size_t len() const;

  std::string toStringOld() const;

  inline std::string toString() const;

  uint64_t operator[](size_t index) const { return digits_[index]; }

  uint64_t& operator[](size_t index) { return digits_[index]; }

  bool isNegative() const { return negative_; }

  BigInteger operator-() const { return BigInteger(*this, !negative_); }
  void invert() { negative_ = !negative_; };

  void makeAbs() { negative_ = false; }

  BigInteger abs() const { return BigInteger(*this, false); }

  void setSign(bool sign) { negative_ = sign; }

  friend BigInteger operator+(BigInteger lhs, const BigInteger& right);

  friend BigInteger operator-(BigInteger lhs, const BigInteger& rhs);

  BigInteger& operator+=(const BigInteger& other);

  BigInteger& operator-=(const BigInteger& other);

  friend BigInteger operator*(const BigInteger& a, const int64_t n);

  friend BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs);

  BigInteger& operator*=(const BigInteger& rhs);

  BigInteger& operator*=(const int64_t number);

  friend BigInteger operator/(const BigInteger& a, const BigInteger& b);

  BigInteger& operator/=(const BigInteger& b);

  friend BigInteger operator/(BigInteger a, const int64_t& n);

  BigInteger& operator/=(const int64_t& n);

  BigInteger& operator%=(const BigInteger& a);

  friend BigInteger operator%(const BigInteger& a, const BigInteger& b);

  BigInteger& operator++();

  BigInteger operator++(int);

  BigInteger& operator--();

  BigInteger operator--(int);

  friend std::istream& operator>>(std::istream& in, BigInteger& bi);

  friend std::ostream& operator<<(std::ostream& out, const BigInteger& bi);

  void pushOverflow();

  void trimLeft() {
    size_ = digits_.size();
    while (size_ > 1 && digits_[size_ - 1] == 0) {
      --size_;
    }
    digits_.resize(size_);
  }

  void shiftLeft();
};

std::string numToString(u_int64_t number, bool fixW) {
  std::string tmp = std::to_string(number);
  if ((tmp.length() < BASE_LENGTH) && fixW) {
    tmp = std::string(BASE_LENGTH - tmp.length(), '0') + tmp;
  }
  return tmp;
}

static std::string decPow(int64_t pow) {
  if (pow < 0) {
    return "0";
  }
  std::string t = "1";
  t += std::string(pow, '0');
  return t;
}

BigInteger::BigInteger(int64_t number) {  // bigint from number
  digits_.clear();
  if (number < 0) {
    negative_ = true;
    number *= -1;
  } else {
    negative_ = false;
  }
  size_t next = 0;
  while (number && next < SIZE) {
    digits_.push_back(number % BASE);
    number /= BASE;
  }
  size_ = digits_.size();
  if (size_ == 0) {
    digits_.push_back(0);
  }
}

BigInteger::BigInteger(int64_t number, size_t len) {
  digits_.resize(len, number);
  size_ = len;
  if (number == 0) {
    size_ = 1;
  }
  negative_ = false;
}

BigInteger::BigInteger(std::string str)
    : digits_((str.size() - (str[0] == '-')) / BASE_LENGTH +
              ((str.size() - (str[0] == '-')) % BASE_LENGTH >
               0)) {  // bigint from string
  negative_ = (str[0] == '-');
  size_ = digits_.size();
  std::reverse(str.begin(), str.end());
  u_int64_t buffer = 0;
  size_t pos = 0;
  size_t power = 1;
  for (size_t i = 0; i < str.length(); ++i) {
    if ((i % BASE_LENGTH == 0) && i > 0) {
      digits_[pos++] = buffer;
      buffer = 0;
      power = 1;
    }
    if (str[i] != '-') {
      buffer += (str[i] - '0') * power;
      power *= 10;
    }
  }
  if (buffer) {
    digits_[pos] = buffer;
    buffer = 0;
  }
}

BigInteger::BigInteger(const BigInteger& other,
                       bool negative) {  // copy constructor
  std::copy(other.digits_.begin(), other.digits_.end(),
            std::back_inserter(digits_));
  negative_ = negative;
  size_ = other.size_;
}

void parseString(std::string str, std::vector<u_int64_t>& dest) {
  std::reverse(str.begin(), str.end());
  u_int64_t buffer = 0;
  size_t pos = 0;
  size_t power = 1;
  for (size_t i = 0; i < str.length(); ++i) {
    if ((i % BASE_LENGTH == 0) && i > 0) {
      dest[pos++] = buffer;
      buffer = 0;
      power = 1;
    }
    if (str[i] != '-') {
      buffer += (str[i] - '0') * power;
      power *= 10;
    }
  }
  if (buffer) {
    dest[pos] = buffer;
    buffer = 0;
  }
}

std::string BigInteger::toString() const {
  std::string s;
  size_t size = digits_.size();
  if (negative_ && !(size == 1 && digits_[0] == 0)) {
    s += "-";
  }
  for (size_t i = 0; i < size; ++i) {
    std::string tmp = std::to_string(digits_[size - i - 1]);
    if (i > 0) {
      tmp = std::string(BASE_LENGTH - tmp.length(), '0') + tmp;
    }
    s += tmp;
  }
  return s;
}

bool operator<(const BigInteger& lhs, const BigInteger& rhs) {
  if (lhs.isNegative() && !rhs.isNegative()) {
    return true;
  }
  if (!lhs.isNegative() && rhs.isNegative()) {
    return false;
  }
  if (lhs.len() < rhs.len()) {
    return !lhs.isNegative();
  }
  if (lhs.len() > rhs.len()) {
    return lhs.isNegative();
  }
  for (size_t i = lhs.len(); i > 0; --i) {
    if (lhs[i - 1] < rhs[i - 1]) {
      return !lhs.isNegative();
    }
    if (lhs[i - 1] > rhs[i - 1]) {
      return lhs.isNegative();
    }
  }
  return (!lhs.isNegative() ? false : true);
}

bool operator>(const BigInteger& lhs, const BigInteger& rhs) {
  return rhs < lhs;
}

bool operator<=(const BigInteger& lhs, const BigInteger& rhs) {
  return !(lhs > rhs);
}

bool operator>=(const BigInteger& lhs, const BigInteger& rhs) {
  return !(lhs < rhs);
}

bool operator==(const BigInteger& lhs, const BigInteger& rhs) {
  if (lhs.len() != rhs.len()) {
    return false;
  }
  for (size_t i = lhs.len(); i > 0; --i) {
    if (lhs[i - 1] != rhs[i - 1]) {
      return false;
    }
  }
  return lhs.isNegative() == rhs.isNegative();
}

bool operator!=(const BigInteger& lhs, const BigInteger& rhs) {
  return !(lhs == rhs);
}

BigInteger operator+(BigInteger lhs, const BigInteger& right);

BigInteger operator-(BigInteger lhs, const BigInteger& rhs);

BigInteger operator+(BigInteger lhs, const BigInteger& rhs) {
  if (lhs.isNegative()) {
    if (rhs.isNegative())
      return -(-lhs + (-rhs));
    else
      return rhs - (-lhs);
  } else if (rhs.isNegative())
    return lhs - (-rhs);
  if (lhs.len() < rhs.len()) {
    lhs.digits_.resize(rhs.len());
  }
  size_t sz = rhs.len();
  for (size_t i = 0; i < sz; i++) {
    lhs[i] += rhs[i];
  }
  lhs.pushOverflow();
  lhs.trimLeft();
  return lhs;
}

BigInteger operator-(BigInteger lhs, const BigInteger& rhs) {
  if (rhs.isNegative())
    return lhs + (-rhs);
  else if (lhs.isNegative())
    return -(-lhs + rhs);
  else if (lhs < rhs)
    return -(rhs - lhs);
  size_t carry = 0;
  size_t new_carry = 0;
  if (lhs.len() < rhs.len()) {
    lhs.digits_.resize(rhs.len());
  }

  for (size_t i = 0; i < rhs.len(); ++i) {
    new_carry = 0;
    if (lhs[i] < carry + rhs[i]) {
      lhs[i] += BASE;
      new_carry = 1;
    }
    lhs[i] -= carry + rhs[i];
    carry = new_carry;
  }
  if (carry != 0) {
    if (lhs.len() > rhs.len()) {
      size_t i = rhs.len();
      while (carry != 0 && i < lhs.len()) {
        new_carry = 0;
        if (lhs[i] < carry) {
          lhs[i] += BASE;
          new_carry = 1;
        }
        lhs[i] -= carry;
        carry = new_carry;
        ++i;
      }
    }
    carry = 0;
  }
  lhs.trimLeft();
  return lhs;
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  *this = (*this) + other;
  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
  *this = (*this) - other;
  return *this;
}

BigInteger operator*(const BigInteger& a, int64_t n) {
  if (!a || !n) {
    return BigInteger(0);
  }
  int64_t rest = 0;
  BigInteger product(0, a.len());
  product.negative_ = a.negative_ ^ (n < 0);
  if (n < 0) {
    n *= -1;
  }
  for (size_t i = 0; i < a.len(); ++i) {
    product[i] = a[i] * n + rest;
    rest = product[i] / BASE;
    product[i] -= rest * BASE;
  }
  if (rest) {
    product.digits_.resize(product.len() + 1);
    product[product.len() - 1] += rest;
  }

  product.trimLeft();
  return product;
}

BigInteger& BigInteger::operator*=(const int64_t number) {
  *this = (*this) * number;
  return *this;
}

BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs) {
  if (!lhs || !rhs) {
    return BigInteger(0);
  }
  BigInteger product(0, lhs.len() + rhs.len() + 1);
  size_t sz1 = lhs.len();
  size_t sz2 = rhs.len();
  for (size_t i = 0; i < sz1; ++i) {
    int64_t rest = 0;
    for (size_t j = 0; j < sz2; ++j) {
      product[i + j] += lhs[i] * rhs[j] + rest;
      rest = product[i + j] / BASE;
      product[i + j] -= rest * BASE;
    }
    if (rest) {
      product[i + sz2] += rest;
    }
  }
  product.trimLeft();
  product.negative_ = lhs.negative_ ^ rhs.negative_;
  return product;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs) {
  *this = *this * rhs;
  return *this;
}

BigInteger operator/(BigInteger a, const int64_t& n) {
  int rest = 0;
  BigInteger product(0, a.len());
  for (size_t i = a.len(); i > 0; i--) {
    int64_t cur = rest * BASE + a[i - 1];
    product[i - 1] = cur / n;
    rest = cur % n;
  }
  product.negative_ = a.negative_ ^ (n < 0);
  product.trimLeft();
  return product;
  // return a / BigInteger(n);
}

BigInteger& BigInteger::operator/=(const int64_t& n) {
  *this = *this / n;
  return *this;
}
/*
int64_t operator%(BigInteger a, const int64_t& n) {
  int64_t rest = 0;
  BigInteger mod;
  for (size_t i = SIZE; i > 0; i--) {
    int64_t cur = rest * BASE + a[i - 1];
    mod[i - 1] = cur / n;
    rest = cur % n;
  }
  rest = (a.isNegative() ^ (n < 0)) ? -rest : rest;
  return rest;
  return a % BigInteger(n);
}
*/
BigInteger operator/(const BigInteger& a, const BigInteger& b) {
  if (!a) return BigInteger(0);
  if (!b) return BigInteger(0);
  if (b.abs() > a.abs()) {
    return BigInteger(0);
  }
  BigInteger abs_b = b.abs();

  BigInteger fraction(0, a.len() + 1);
  BigInteger curValue(0);
  size_t sz1 = a.len();
  for (size_t i = sz1; i > 0; i--) {
    curValue.digits_.push_back(0);
    curValue.shiftLeft();
    curValue.trimLeft();
    curValue[0] = a[i - 1];
    ++curValue.size_;
    int64_t x = 0;
    int64_t l = 0, r = BASE;
    while (l <= r) {
      int64_t m = (l + r) >> 1;
      BigInteger cur = abs_b * m;
      if (cur <= curValue) {
        x = m;
        l = m + 1;
      } else
        r = m - 1;
    }
    fraction[i - 1] = x;
    curValue = curValue - abs_b * x;
  }
  fraction.negative_ = a.negative_ ^ b.negative_;
  fraction.trimLeft();
  return fraction;
}

BigInteger& BigInteger::operator/=(const BigInteger& b) {
  *this = *this / b;
  return *this;
}

BigInteger operator%(const BigInteger& a, const BigInteger& b) {
  if (a < b) {
    return BigInteger(a, a.isNegative());
  }
  if (a == 0 || b == 0) {
    return BigInteger(0);
  }
  /*
  BigInteger rest(0, a.size_);
  BigInteger curValue;

  BigInteger abs_b = b.abs();
  for (size_t i = a.size_; i > 0; i--) {
    curValue.shiftLeft();
    curValue.digits_.push_back(a[i - 1]);
    ++curValue.size_;
    int64_t x = 0;
    int64_t l = 0, r = BASE;
    while (l <= r) {
      int64_t m = (l + r) >> 1;
      BigInteger cur = abs_b * m;
      if (cur <= curValue) {
        x = m;
        l = m + 1;
      } else
        r = m - 1;
    }
    rest[i - 1] = x;
    curValue = curValue - abs_b * x;
  }
  curValue.negative_ = a.negative_;
  curValue.trimLeft();
  return curValue;
  */
  return a - b * (a / b);
}
/*
BigInteger operator%(const BigInteger& a, const BigInteger& b) {
  return a - b * (a / b);
}
*/
BigInteger& BigInteger::operator%=(const BigInteger& a) {
  *this = *this % a;
  return *this;
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger result(*this);
  *this += 1;
  return result;
}

BigInteger& BigInteger::operator--() {
  *this -= BigInteger(1);
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger tmp(*this);
  *this -= BigInteger(1);
  return tmp;
}

BigInteger::operator bool() const {
  return (this->len() > 1) || ((this->len() == 1) && (this->digits_[0] != 0));
}

BigInteger::operator double() const {
  ssize_t num = 0;
  for (size_t i = 0; i < size_; ++i) {
    num *= BASE;
    num += digits_[size_ - 1 - i];
  }
  return num;
}

size_t BigInteger::len() const { return digits_.size(); }

BigInteger operator""_bi(unsigned long long number) {
  return BigInteger(number);
}

BigInteger operator""_bi(const char* number, size_t) {
  return BigInteger(std::string(number));
}

std::istream& operator>>(std::istream& in, BigInteger& bi) {
  std::string buffer;
  in >> buffer;
  bi = BigInteger(buffer);
  return in;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& bi) {
  out << bi.toString();
  return out;
}

void BigInteger::pushOverflow() {
  size_t size = digits_.size();
  if (size > 0) {
    for (size_t i = 0; i < size - 1; ++i) {
      while (digits_[i] >= BASE) {
        digits_[i] -= BASE;
        ++digits_[i + 1];
      }
    }
  }
  if (digits_[size - 1] >= BASE) {
    digits_.push_back(1);
    digits_[size - 1] -= BASE;
  }
}

void BigInteger::shiftLeft() {
  size_ = digits_.size();
  if (digits_.size() > 1 && digits_[size_ - 1]) {
    digits_.resize(size_ + 1);
    digits_[size_] = digits_[size_ - 1];
    ++size_;
  }

  for (size_t i = digits_.size(); i > 1; --i) digits_[i - 1] = digits_[i - 2];
}

class Rational {
 private:
  BigInteger numerator_;
  BigInteger denominator_;
  void shift();

 public:
  Rational() : Rational(0){};
  Rational(int64_t number) : numerator_(number), denominator_(1){};
  Rational(const BigInteger& bi) : numerator_(bi), denominator_(1){};
  Rational(const BigInteger& num, const BigInteger& denom);
  // Rational(const Rational& r)
  //     : numerator_(r.numerator_), denominator_(r.denominator_){};

  std::string toString() const;

  std::string asDecimal(size_t precision = 0) const;

  bool isNegative() const { return numerator_.isNegative(); }

  Rational operator-() const;

  Rational operator+() const { return *this; }

  friend Rational operator+(const Rational& a, const Rational& b);

  friend Rational operator-(const Rational& a, const Rational& b);

  friend Rational operator*(const Rational& a, const Rational& b);

  friend Rational operator/(const Rational& a, const Rational& b);

  Rational& operator+=(const Rational& a);

  Rational& operator-=(const Rational& a);

  Rational& operator*=(const Rational& a);

  Rational& operator/=(const Rational& a);

  friend bool operator<(const Rational& lhs, const Rational& rhs);

  friend bool operator>(const Rational& lhs, const Rational& rhs);

  friend bool operator<=(const Rational& lhs, const Rational& rhs);

  friend bool operator>=(const Rational& lhs, const Rational& rhs);

  friend bool operator==(const Rational& lhs, const Rational& rhs);

  friend bool operator!=(const Rational& lhs, const Rational& rhs);

  friend std::ostream& operator<<(std::ostream& out, const Rational& r) {
    out << r.toString();
    return out;
  }

  explicit operator double();
};

Rational::Rational(const BigInteger& num, const BigInteger& denom)
    : numerator_(num), denominator_(denom) {
  /*if (num > 0 && denom < 0) {
    numerator_ = -num;
  } else if (num.isNegative() && denom.isNegative()) {
    numerator_ = -num;
  } else {
    numerator_ = num;
  }
  denominator_ = denom >= 0 ? denom : -denom;*/
  if (num > 0 && denom < 0)
    numerator_ = -num;
  else if (num < 0 && denom < 0) {
    numerator_ = -num;
  } else {
    numerator_ = num;
  }
  denominator_ = denom >= 0 ? denom : -denom;
  shift();
};

std::string Rational::toString() const {
  if (denominator_ == 1) {
    return numerator_.toString();
  } else {
    return numerator_.toString() + "/" + denominator_.toString();
  }
}

std::string Rational::asDecimal(size_t precision) const {
  BigInteger quotinent;
  BigInteger numerator;
  std::string decimal;
  std::string result;
  if (precision > 0) {
    if (numerator_.isNegative()) {
      result += '-';
    }
    BigInteger power(decPow(precision));
    numerator = numerator_.abs() * power;
    quotinent = (numerator / denominator_);
    decimal = quotinent.toString();
    if (decimal.length() < precision) {
      decimal = std::string(precision - decimal.length(), '0') + decimal;
    }
    if (decimal.length() == precision) {
      result += '0';
    }
    result += decimal.substr(0, decimal.length() - precision) + '.' +
              decimal.substr(decimal.length() - precision, precision);
  } else {
    quotinent = (numerator_ / denominator_);
    result = quotinent.toString();
  }
  return result;
}

void Rational::shift() {
  BigInteger x = numerator_ >= 0 ? numerator_ : -numerator_;
  BigInteger y = denominator_ >= 0 ? denominator_ : -denominator_;
  while (x != 0 && y != 0) {
    if (x > y) {
      x = x % y;
    } else {
      y = y % x;
    }
  }
  numerator_ /= x + y;
  denominator_ /= x + y;
  // numerator_.setSign(numerator_.isNegative() ^ denominator_.isNegative());
  // denominator_.setSign(false);
}

Rational Rational::operator-() const {
  Rational neg(*this);
  neg.numerator_.invert();
  return neg;
}

Rational operator+(const Rational& a, const Rational& b) {
  return Rational(a.numerator_ * b.denominator_ + b.numerator_ * a.denominator_,
                  a.denominator_ * b.denominator_);
}

Rational operator-(const Rational& a, const Rational& b) {
  return Rational(
      ((a.numerator_ * b.denominator_) - (b.numerator_ * a.denominator_)),
      (a.denominator_ * b.denominator_));
}

Rational operator*(const Rational& a, const Rational& b) {
  return Rational(a.numerator_ * b.numerator_, a.denominator_ * b.denominator_);
}

Rational operator/(const Rational& a, const Rational& b) {
  return Rational(a.numerator_ * b.denominator_, a.denominator_ * b.numerator_);
}

Rational& Rational::operator+=(const Rational& a) {
  *this = *this + a;
  return *this;
}

Rational& Rational::operator-=(const Rational& a) {
  *this = *this - a;
  return *this;
}

Rational& Rational::operator*=(const Rational& a) {
  *this = *this * a;
  return *this;
}

Rational& Rational::operator/=(const Rational& a) {
  *this = *this / a;
  return *this;
}

Rational::operator double() {
  return (static_cast<double>(numerator_) / static_cast<double>(denominator_));
}

bool operator<(const Rational& lhs, const Rational& rhs) {
  if (lhs.isNegative() && !rhs.isNegative()) {
    return true;
  }
  if (!lhs.isNegative() && rhs.isNegative()) {
    return false;
  }

  if (rhs.isNegative()) {
    return -rhs < -lhs;
  }
  Rational quotinent = lhs / rhs;
  return quotinent.numerator_ < quotinent.denominator_;
}

bool operator>(const Rational& lhs, const Rational& rhs) { return rhs < lhs; }

bool operator<=(const Rational& lhs, const Rational& rhs) {
  return !(lhs > rhs);
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
  return !(lhs < rhs);
}

bool operator==(const Rational& lhs, const Rational& rhs) {
  return !(lhs > rhs) && !(lhs < rhs);
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
  return !(lhs == rhs);
}