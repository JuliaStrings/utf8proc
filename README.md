# libmojibake
[![Build Status](https://travis-ci.org/JuliaLang/libmojibake.png)](https://travis-ci.org/JuliaLang/libmojibake)

[libmojibake](https://github.com/JuliaLang/libmojibake) is a
development fork of the [utf8proc
library](http://www.public-software-group.org/utf8proc) from Jan
Behrens and the rest of the [Public Software
Group](http://www.public-software-group.org/), who deserve *nearly all
of the credit* for this package: a small, clean C library that
provides Unicode normalization, case-folding, and other operations for
data in the [UTF-8 encoding](http://en.wikipedia.org/wiki/UTF-8).  The
main difference from utf8proc is that the Unicode support in
libmojibake is more up-to-date (Unicode 7 vs. Unicode 5).

The reason for this fork is that utf8proc is used for basic Unicode
support in the [Julia language](http://julialang.org/) and the Julia
developers wanted Unicode 7 support and other features, but the Public
Software Group is currently occupied with other projects.  As we implement
and test new features in libmojibake, we are contributing patches back
to utf8proc with the hope that they can be merged upstream.

(The original utf8proc package also includes Ruby and PostgreSQL plug-ins.
We removed those from libmojibake in order to focus exclusively on the C
library for the time being.  We will strive to keep API changes to a minimum,
so libmojibake should still be usable with the old plug-in code.)

Like utf8proc, the libmojibake package is licensed under the
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

The Unicode version being supported is 7.0.0.  (Grapheme segmentation
is currently based on version 4.1.0 of Unicode Standard Annex #29, but
we hope to update this soon.)

For Unicode normalizations, the following options are used:

* Normalization Form C:  `STABLE`, `COMPOSE`
* Normalization Form D:  `STABLE`, `DECOMPOSE`
* Normalization Form KC: `STABLE`, `COMPOSE`, `COMPAT`
* Normalization Form KD: `STABLE`, `DECOMPOSE`, `COMPAT`

## C Library ##

The documentation for the C library is found in the `utf8proc.h` header file.
`utf8proc_map` is function you will most likely be using for mapping UTF-8
strings, unless you want to allocate memory yourself.

## To Do ##

See the Github [issues list](https://github.com/JuliaLang/libmojibake/issues).

## Contact ##

Bug reports, feature requests, and other queries can be filed at
the [libmojibake issues page on Github](https://github.com/JuliaLang/libmojibake/issues).

