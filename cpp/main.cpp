#include "utf8proc.hpp"

int main() {
  namespace u8 = utf8proc;

  // from tests/case.c : issue #130
  // these tests are done in compile-time!
  static_assert(u8::toupper(0x00df) == 0x1e9e, "");
  static_assert(u8::totitle(0x00df) == 0x1e9e, "");
  static_assert(u8::tolower(0x00df) == 0x00df, "");
  static_assert(u8::tolower(0x1e9e) == 0x00df, "");
  static_assert(u8::toupper(0x1e9e) == 0x1e9e, "");

  return 0;
}