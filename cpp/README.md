# utf8proc.hpp

Here is a version of the utf8proc library, but made usable in constexpr expressions.
You just need to include the `utf8proc.hpp` header, and the library is usable.
(the `fake_libc_headers` is only useful for the generation, not for the usage)

Some differences from the C library:
- The public entities are not prefixed by `utf8proc_`. Instead, they are in the `utf8proc::` namespace.
- The private entities (`static` from `.c` files) are in the `utf8proc::detail::` namespace.
- All entities are constexpr.
- Allocating entities are not available, because `malloc` is not constexpr.
  This includes `map`, `map_custom`, and facilities to do normalizations directly on UTF-8 strings (such as `utf8proc_NFKC`).
  With relaxed rules on constexpr from C++20, it may be possible to allow them.

## Generation

Generation of `utf8proc.hpp` requires Python 3 and [pycparser](https://github.com/eliben/pycparser).
```
> pip install pycparser
> python3 gen_hpp.py
```
It may take some minutes (notably the AST building), and then the file was correctly generated.

You may verify it by compiling `main.cpp`.
Notice that the checks are done using `static_assert` (at compile-time), so if it compiles, it means that the library works.