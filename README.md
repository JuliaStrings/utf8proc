# libmojibake

[libmojibake](https://github.com/JuliaLang/libmojibake) is
a lightly updated fork of the [utf8proc
library](http://www.public-software-group.org/utf8proc) from Jan
Behrens and the rest of the [Public Software
Group](http://www.public-software-group.org/), who deserve *nearly all
of the credit* for this package: a small, clean C library that
provides Unicode normalization, case-folding, and other operations for
data in the [UTF-8 encoding](http://en.wikipedia.org/wiki/UTF-8).

The reason for this fork is that `utf8proc` is used for basic Unicode
support in the [Julia language](http://julialang.org/) and the Julia
developers wanted Unicode 7 support and other features, but the
Public Software Group currently does not seem to have the resources
necessary to update `utf8proc`.  We hope that the fork can be merged
back into the mainline `utf8proc` package before too long.

(The original `utf8proc` package also includes Ruby and PostgreSQL plug-ins.
We removed those from `libmojibake` in order to focus exclusively on the C
library for the time being.  We will strive to keep API changes to a minimum,
so `libmojibake` should still be usable with the old plug-in code.)

Like `utf8proc`, the `libmojibake` package is licensed under the
free/open-source [MIT "expat"
license](http://opensource.org/licenses/MIT) (plus certain Unicode
data governed by the similarly permissive [Unicode data
license](http://www.unicode.org/copyright.html#Exhibit1)); please see
the included `LICENSE.md` file for more detailed information.

## Quick Start ##

For compilation of the C library run `make`.

## General Information ##

The C library is found in this directory after successful compilation
and is named `libmojibake.a` (for the static library) and
`libmojibake.so` (for the dynamic library).

The Unicode version being supported is 5.0.0.
*Note:* Version 4.1.0 of Unicode Standard Annex #29 was used, as
version 5.0.0 had not been available at the time of implementation.

For Unicode normalizations, the following options are used:

* Normalization Form C:  `STABLE`, COMPOSE`
* Normalization Form D:  `STABLE`, `DECOMPOSE`
* Normalization Form KC: `STABLE`, `COMPOSE`, `COMPAT`
* Normalization Form KD: `STABLE`, `DECOMPOSE`, `COMPAT`

## C Library ##

The documentation for the C library is found in the `utf8proc.h` header file.
`utf8proc_map` is function you will most likely be using for mapping UTF-8
strings, unless you want to allocate memory yourself.

## To Do ##

* detect stable code points and process segments independently in order to save memory
* do a quick check before normalizing strings to optimize speed
* support stream processing

## Contact ##

Bug reports, feature requests, and other queries can be filed at
the [libmojibake page on Github](https://github.com/JuliaLang/libmojibake/issues).

