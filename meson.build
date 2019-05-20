# This is the ABI version number, which may differ from the
# API version number (defined in utf8proc.h).
# Be sure to also update these in Makefile and MANIFEST!
project('utf8proc', 'c', version: '2.3.1', license: 'mit expat', default_options: [])

# Library
add_project_arguments('-DUTF8PROC_EXPORTS', language: 'c')
libutf8proc = library('utf8proc', 'utf8proc.c', install: true)
install_headers('utf8proc.h')

# Dependency
libutf8proc_dep = declare_dependency(link_with: libutf8proc)

# pkgconfig
pkg = import('pkgconfig')
pkg.generate(name: 'libutf8proc',
            description: 'UTF8 processing',
            filebase: 'libutf8proc',
            libraries: libutf8proc,
            extra_cflags: '-DUTF8PROC_EXPORTS')
