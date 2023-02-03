#include <iostream>
#include <string>
#include <cassert>

#include "biginteger.h"

size_t counter = 1;

void printMessage(std::string message) {
  std::cout << counter++ << "===" << message << "===\n";
}

void testIO() {
  // Test 1. Initialization & IO
  printMessage("IO");
  BigInteger bi1;
  std::cout << bi1 << '\n';
  bi1 -= 2 * bi1;

  BigInteger bi2 = 1002023;
  std::cout << bi2 << '\n';

  std::cout << "enter\n";
  BigInteger bi3;
  std::cin >> bi3;
  std::cout << bi3 << '\n';
}

void testIncrementAndCompare() {
  printMessage("Increment");
  BigInteger bi1, bi2;
  std::cin >> bi1;
  ++bi1;
  std::cout << "++bi\n" << bi1 << '\n';
  bi2 += 2;
  std::cout << "bi2+=\n" << bi2 << '\n';
  bi1 += bi2;
  std::cout << "bi+=bi\n" << bi1 << '\n';
  std::cout << "postfix++\n" << bi1++ << '\n';
  std::cout << "bi1 < bi2\n" << std::boolalpha << (bi1 < bi2) << '\n';
}

void testMinus() {
  printMessage("Minus");
  BigInteger bi1;
  std::cin >> bi1;
  //bi1 += 1212;
  std::cout << "+=1212 " << bi1+1212 << '\n';
  //--bi1;
  std::cout << "-- " << bi1 << '\n';
  std::cout << "- " << -bi1 << '\n';
  BigInteger bi2;
  std::cin >> bi2;
  std::cout << "bi1 - bi2 " << bi1 - bi2 << '\n';
  std::cout << "bi1 + bi2 " << bi1 + bi2 << '\n';
  std::cout << ("-123902310000000000213321"_bi).toString() << '\n';
}

void testLiterals() {
  printMessage("Literals");
  auto bi1 = 12122322_bi;
  std::cout << bi1 << '\n';
  auto bi2 = "-12122322"_bi;
  std::cout << bi2 << '\n';
  std::cout << std::boolalpha << (bi1 == bi2) << '\n';
  auto bi3 = bi2 + bi1;
  std::cout << bi3 << '\n';
}

void testConversion() {
  printMessage("Conversion");
  if ("12212"_bi) {
    std::cout << "OK\n";
  } else {
    std::cout << "WA\n";
  }
  if ("0"_bi) {
    std::cout << "WA\n";
  } else {
    std::cout << "OK\n";
  }
  auto bi3 = "-121412"_bi;
  std::cout << bi3 << '\n';
}

void testDiv() {
  printMessage("Division");
  /*
  std::cout << "short\n";
  BigInteger b;
  int64_t a;
  std::cin >> b >> a;
  std::cout << b / a << '\n';*/

  // std::cout << "long\n";
  BigInteger b1, b2;
  std::cin >> b1;
  std::cin >> b2;
  std::cout << (b1 / b2) << '\n';
}

void testMult() {
  //printMessage("Multiplication");
  BigInteger b1, b2;
  std::cin >> b1;
  std::cin >> b2;
  std::cout << (b1 * b2) << '\n';
  std::cout << (b1 * -11) << '\n';
  std::cout << (b2 * 8) << '\n';
  std::cout << (0 * b1) << '\n';
  std::cout << (b1 % b2) << '\n';
  std::cout << (b2 % b1) << '\n';
  

}

void testRational() {
  BigInteger b1, b2, b3, b4;
  std::cin >> b1 >> b2 >> b3 >> b4;
  Rational r1(b1), r2(b2), r3(b3), r4(b4);
  std::cout << r1.asDecimal() << '\n';
  std::cout << (r1 / r2).asDecimal(5) << '\n';
  std::cout << (r3 / r4).asDecimal(5) << '\n';
  std::cout << (r3 - r1) << '\n';
  std::cout << (r3 < r1) << '\n';
  std::cout << (r3 > r1) << '\n';
  std::cout << (r3 == r1) << '\n';
  std::cout << (r3 != r1) << '\n';
  r1 /= r2;
  r3 /= r4;
  std::cout << (r3 < r1) << '\n';
  std::cout << (r3 > r1) << '\n';
  std::cout << (r3 == r1) << '\n';
  std::cout << (r3 != r1) << '\n';
}

void vhtest() {
  BigInteger a("9000");
  //std::cin >> a;
  BigInteger b("5000");
  //std::cin >> b;
  BigInteger c("4");
  BigInteger d("-5");
  BigInteger e("7");
  BigInteger f("5050505050505050505050505050505");
  BigInteger p("-25000");

  assert(a + b == BigInteger("14000"));
  assert(a + c == BigInteger("9004"));
  assert(a + d == BigInteger("8995"));
  assert(a + d == BigInteger("8995"));

  assert(b - a == BigInteger("-4000"));
  assert(a - c == BigInteger("8996"));
  assert(a - d == BigInteger("9005"));
  assert(d - e == BigInteger("-12"));
  assert(c - e == BigInteger("-3"));

  assert(-a == BigInteger("-9000"));
  assert(-d == BigInteger("5"));

  assert(!(c > e));
  assert(b > c);

  assert(c * e == BigInteger("28"));
  assert(b * d == BigInteger("-25000"));
  assert(b * e == BigInteger("35000"));
  assert(c * e == BigInteger("28"));
  assert(f * d == BigInteger("-25252525252525252525252525252525"));

  BigInteger x;
  BigInteger y;
  Rational s;
  Rational t = -402365939;
  s = 1000000007;
  s *= 1000000009;
  s *= 2147483647;
  //((s = 1000000007) *= 1000000009) *= 2147483647;
  std::cout << s.toString() << '\n';
  std::cout << (t / s).asDecimal(25) << '\n';
  //BigInteger h("1000000000000000000000000000000");
  //std::cout << ((t * h) / s).asDecimal() << '\n';
}

int main() {
#ifdef IO
  testIO();
#endif
#ifdef INCR
  testIncrementAndCompare();
#endif
#ifdef MINUS
  testMinus();
#endif
#ifdef LIT
  testLiterals();
#endif
#ifdef CONV
  testConversion();
#endif
#ifdef DIV
  testDiv();
#endif
#ifdef MULT
  testMult();
#endif
#ifdef RAT
  testRational();
#endif
vhtest();
  return 0;
}