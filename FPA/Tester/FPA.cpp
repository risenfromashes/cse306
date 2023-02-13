#include <bits/stdc++.h>
#include <cmath>
#include <cstdint>

#define DOUBLE_EXP_BITS 11
#define DOUBLE_MANTISSA_BITS 52

const uint64_t DOUBLE_BIAS = (1ull << (DOUBLE_EXP_BITS - 1ull)) - 1ull;

#define EXP_BITS 12
#define MANTISSA_BITS 19

const uint64_t BIAS = (1ull << (EXP_BITS - 1ull)) - 1ull;

template <class T, class storage> void print_bits(T f, uint16_t num) {
  storage v = *((storage *)&f);
  int fmod = -1;
  for (int i = num - 1; i >= 0; i--) {
    if ((fmod = (fmod + 1) % 4) == 0) {
      std::cout << ' ';
    }
    std::cout << !!((1ull << i) & v);
  }
  std::cout << std::endl;
}

template <class T, class storage>
void vec_bits(T f, uint16_t num, std::vector<bool> &vec) {
  storage v = *((storage *)&f);
  int fmod = -1;
  for (int i = num - 1; i >= 0; i--) {
    vec.push_back(!!((1ull << i) & v));
  }
}

template <class T> void print_bits_8(T f, uint16_t num) {
  print_bits<T, uint8_t>(f, num);
}

template <class T> void print_bits_16(T f, uint16_t num) {
  print_bits<T, uint16_t>(f, num);
}

template <class T> void print_bits_32(T f, uint16_t num) {
  print_bits<T, uint32_t>(f, num);
}

template <class T> void print_bits_64(T f, uint16_t num) {
  print_bits<T, uint64_t>(f, num);
}

template <class T> void vec_bits_8(T f, uint16_t num, std::vector<bool> &v) {
  vec_bits<T, uint8_t>(f, num, v);
}

template <class T> void vec_bits_16(T f, uint16_t num, std::vector<bool> &v) {
  vec_bits<T, uint16_t>(f, num, v);
}

template <class T> void vec_bits_32(T f, uint16_t num, std::vector<bool> &v) {
  vec_bits<T, uint32_t>(f, num, v);
}

template <class T> void vec_bits_64(T f, uint16_t num, std::vector<bool> &v) {
  vec_bits<T, uint64_t>(f, num, v);
}

struct MyFloat {
  bool sign;
  uint16_t exp;
  uint32_t mantissa;
  bool rounded;

  MyFloat() {}

  MyFloat(double f) {
    rounded = false;
    if (f == 0.0) {
      sign = 0;
      exp = 0;
      mantissa = 0;
      return;
    }
    sign = f < 0;
    if (sign) {
      f = -f;
    }
    uint64_t v = *((uint64_t *)&f);
    exp = (((v & (((1ull << DOUBLE_EXP_BITS) - 1) << DOUBLE_MANTISSA_BITS)) >>
            DOUBLE_MANTISSA_BITS) -
           DOUBLE_BIAS + BIAS) &
          ((1ull << EXP_BITS) - 1);
    const uint64_t extra_bits = DOUBLE_MANTISSA_BITS - MANTISSA_BITS;
    uint64_t mantissa64 = (v & ((1ull << DOUBLE_MANTISSA_BITS) - 1));
    mantissa = (mantissa64 >> extra_bits) & ((1ull << MANTISSA_BITS) - 1);
    uint8_t B = !!(mantissa64 & (1ull << extra_bits));
    uint8_t G = !!(mantissa64 & (1ull << (extra_bits - 1)));
    uint8_t R = !!(mantissa64 & (1ull << (extra_bits - 2)));
    uint8_t S = 0;
    for (int i = 0; i < (extra_bits - 2); i++) {
      S |= !!(mantissa64 & (1ull << i));
    }

    if (G && ((B || S) || R)) {
      rounded = true;
      mantissa64 = mantissa64 + (1ull << extra_bits);
      mantissa = (mantissa64 >> extra_bits) & ((1ull << MANTISSA_BITS) - 1ull);
      if (mantissa == 0) {
        exp++;
      }
    }
  }

  operator double() {
    if (is_zero()) {
      return 0;
    }
    uint64_t d_exp =
        (exp - BIAS + DOUBLE_BIAS) & ((1ull << DOUBLE_EXP_BITS) - 1);
    uint64_t d_mantissa = (uint64_t)mantissa
                          << (DOUBLE_MANTISSA_BITS - MANTISSA_BITS);
    bool d_sign = sign;
    uint64_t v = 0;
    v |= d_mantissa;
    v |= (d_exp << DOUBLE_MANTISSA_BITS);
    v |= (d_sign * (1ull << 63));
    return *((double *)&v);
  }

  static MyFloat random() {
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_int_distribution<int> s_dist(0, 1);
    static std::uniform_int_distribution<uint16_t> exp_dist(1024u, 3071u);
    static std::uniform_int_distribution<uint32_t> m_dist(0, (1u << 20) - 1);

    MyFloat f;
    f.sign = s_dist(mt);
    f.exp = exp_dist(mt);
    f.mantissa = m_dist(mt);
    return f;
  }

  void print_repr() {
    // std::cout << "Rouded: " << rounded << std::endl;
    // std::cout << "Sign: " << sign << std::endl;
    // print_bits_8(sign, 8);
    // std::cout << "Exp: " << (exp - BIAS) << std::endl;
    // print_bits_16(exp, 16);
    // std::cout << "Mantissa: ";
    // print_bits_32(mantissa, 32);
    std::vector<bool> v;
    v.push_back(sign);
    vec_bits_16(exp, EXP_BITS, v);
    vec_bits_32(mantissa, MANTISSA_BITS, v);
    int fmod = -1;
    for (int i = 0; i < 32; i++) {
      fmod = (fmod + 1) % 8;
      if (i > 0 && fmod == 0) {
        std::cout << "  ";
      }
      std::cout << v[i];
    }
    std::cout << std::endl;
  }

  bool is_zero() {
    bool e = (exp & ((1ull << EXP_BITS) - 1)) == 0;
    bool m = (mantissa & ((1ull << MANTISSA_BITS) - 1)) == 0;
    return e && m;
  }

  friend MyFloat operator+(MyFloat a, MyFloat b) {
    if (a.is_zero()) {
      return b;
    }
    if (b.is_zero()) {
      return a;
    }

    MyFloat ret;
    uint8_t s1s2 = a.sign ^ b.sign;
    uint8_t bs = b.sign;
    bool sw = 0;
    if (a.exp < b.exp) {
      sw = 1u;
      std::swap(a, b);
    }

    uint16_t expdiff = a.exp - b.exp;

    uint32_t m_a = 1u << 31;
    uint32_t m_b = 1u << 31;

    m_a |= (a.mantissa << EXP_BITS);
    m_b |= (b.mantissa << EXP_BITS);
    if (expdiff >= 32) {
      return a;
    } else {
      m_b = m_b >> expdiff;
    }

    ret.exp = a.exp;

    if (s1s2) {
      m_b = -m_b;
    }

    uint64_t sum64 = 0ull + m_a + m_b;
    uint32_t sum = (sum64 << 32) >> 32;
    uint8_t cout = sum64 >> 32;

    uint8_t neg = s1s2 && (!cout);
    if (neg) {
      sum = -sum;
    }

    if (s1s2 == 0) {
      if (cout == 0) {
        sum <<= 1;
      } else {
        ret.exp++;
      }
    } else {
      for (;;) {
        if (sum & (1u << 31)) { // check msb
          break;
        }
        sum <<= 1;
        ret.exp--;
      }
      sum <<= 1;
    }
    const uint64_t extra_bits = 32 - MANTISSA_BITS;
    uint8_t B = !!(sum & (1u << extra_bits));
    uint8_t G = !!(sum & (1u << (extra_bits - 1)));
    uint8_t R = !!(sum & (1u << (extra_bits - 2)));
    uint8_t S = 0;
    for (int i = 0; i < (extra_bits - 2); i++) {
      S |= !!(sum & (1u << i));
    }

    if (G && ((B || S) || R)) {
      sum64 = sum + (1u << extra_bits);
      sum = (sum64 << 32) >> 32;
      if ((sum >> extra_bits) == 0) {
        ret.exp++;
      }
    }

    ret.mantissa = sum >> extra_bits;
    if (s1s2 == 0) {
      ret.sign = a.sign;
    } else {
      ret.sign = (!cout) ^ (!sw) ^ bs;
    }

    return ret;
  }
};

double gen_random() {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  static double double_min = std::numeric_limits<double>::min();
  static double double_max = std::numeric_limits<double>::max();
  static std::normal_distribution<double> dist(0, 1e5);
  return dist(mt);
}

double error(double a, double b) { return std::fabs(a - b) / a; }

void gen_test(double da, double db) {
  // 0.00001 + -9.99999
  MyFloat a = MyFloat(da);
  MyFloat b = MyFloat(db);
  double dr = double(a) + double(b);
  MyFloat r = a + b;

  double err1 = error(da, a);
  double err2 = error(db, b);
  double err3 = error(dr, r);

  // std::cout << "err1: " << err1 << std::endl;
  // std::cout << "err2: " << err2 << std::endl;
  std::cout << "err3: " << err3 << std::endl;

  std::cout << "a (double): " << std::endl;
  std::cout << da << std::endl;
  std::cout << "a :" << std::endl;
  a.print_repr();
  std::cout << "a (MyFloat): " << std::endl;
  std::cout << (double)a << std::endl;
  std::cout << std::endl;

  std::cout << "b (double): " << std::endl;
  std::cout << db << std::endl;
  std::cout << "b :" << std::endl;
  b.print_repr();
  std::cout << "b (MyFloat): " << std::endl;
  std::cout << (double)b << std::endl;
  std::cout << std::endl;

  std::cout << "r (double): " << std::endl;
  std::cout << dr << std::endl;
  std::cout << "r :" << std::endl;
  r.print_repr();
  std::cout << "r (MyFloat): " << std::endl;
  std::cout << (double)r << std::endl;
  std::cout << std::endl;
}

int main() {
  std::cout.precision(std::numeric_limits<double>::max_digits10);
  double da;
  double db;
  for (;;) {
    char ch;
    std::cout << "Enter 0 for test input" << std::endl
              << "Enter 1 for random input" << std::endl;
    std::cin >> ch;
    if (ch == '0') {
      std::cout << "Enter a (as double): ";
      std::cin >> da;
      std::cout << "Enter b (as double): ";
      std::cin >> db;
    } else {
      da = gen_random();
      db = gen_random();
    }
    gen_test(da, db);
  }
}
