#              Copyright Julien Vernay 2020.
# Distributed under the Boost Software License, Version 1.0.
#    (See copy at https://www.boost.org/LICENSE_1_0.txt)


# AIM OF THIS SCRIPT:
# Generating a constexpr C++ library header from the C source file.

# STEPS:
# 1. getting #define to convert them to `static constexpr`
#    and #include to external files, to put them in the .hpp
# 2. we build the Abstract Syntax Tree (AST)
# 3. renaming entities to their namespaced counterparts
# 4. filtering and modifications on the top-level declarations
#    (for instance, removing `extern` declarations)
# 5. outputting the modified AST to the .hpp file

#================= CONFIG ===================

OUTPUT_FILE = "utf8proc.hpp"
OUTPUT_HEADER_GUARD = "UTF8PROC_HPP"

# pycparser uses fake standard headers because these can be expensive to parse,
# and only types (struct, enum...) must be known to correctly process the files.
LIBC_HEADERS_DIR = "fake_libc_headers"

# main source file
SOURCE_FILE = "../utf8proc.c"

# headers, only used to access public #define
HEADER_FILES = [ "../utf8proc.h" ]

# gives definitions for the preprocessor
PREPROCESSOR_DEFINITIONS = { 
    "UTF8PROC_STATIC": None, # this removes DLLEXPORT
    "UTF8PROC_NO_ALLOC": None, # this removes allocating functions, which cannot be constexpr
}

EXCLUDE_DEFINITIONS = [ "UTF8PROC_DLLEXPORT" ]

# to make it more C++ idiomatic, prefixed names (such as utf8proc_iterate or UTF8PROC_BOUNDCLASS_ZWJ)
# are unprefixed and put inside a namespace
PREFIX = "utf8proc_" # the prefix is case-insensitive
NAMESPACE = "utf8proc" # resulting namespace

# in header-only libraries, there cannot be any `static` entities, so we put them in a private namespace
STATIC_NAMESPACE = "detail"

PREVENT_RENAMING = [ "utf8proc_bool", "bool" ]

#================== START OF SCRIPT ===================

from pycparser import parse_file, c_generator, c_parser, c_ast, preprocess_file
from os import path

LIBC_PREFIX_PATH = LIBC_HEADERS_DIR + "/"

#=== TRACKING #define and #include ===
# here we want to convert #define to `constexpr auto`
# and includes to external libraries must not be lost.
print("Tracking #define and #include...")

public_defines = {}
all_includes = set()

for header in HEADER_FILES:
    with open(header) as f:
        for line in f.readlines():
            if line.startswith("#"):
                line = line[1:]
                parts = line.split(maxsplit=2)
                if len(parts) >= 3 and parts[0] == "define":
                    name = parts[1]
                    if name not in EXCLUDE_DEFINITIONS:
                        if name.upper().startswith(PREFIX.upper()):
                            newname = name[len(PREFIX):]
                            public_defines[newname] = parts[2].strip()
                elif len(parts) >= 2 and parts[0] == "include":
                    line = parts[1]
                    begin = line.find("<")
                    if begin != -1:
                        line = line[begin+1 : line.index(">")]
                        if path.exists(LIBC_PREFIX_PATH + line):
                            # it is a standard header
                            # converting <stdlib.h> to <cstdlib>
                            line = "c" + line[:line.index(".")]
                        all_includes.add(line)

# include must be searched through SOURCE_FILE because they are needed for implementation
# and the implementation is needed to have constexpr
with open(SOURCE_FILE) as f:
    for line in f.readlines():
            if line.startswith("#"):
                line = line[1:]
                parts = line.split(maxsplit=2)
                if len(parts) >= 2 and parts[0] == "include":
                    line = parts[1]
                    begin = line.find("<")
                    if begin != -1:
                        line = line[begin+1 : line.index(">")]
                        if path.exists(LIBC_PREFIX_PATH + line):
                            # it is a standard header
                            # converting <stdlib.h> to <cstdlib>
                            line = "c" + line[:line.index(".")]
                        all_includes.add(line)

#=== BUILDING AST ===
# This step involves giving correct flags to the processors and call pycparser.
print("Building AST...")

# <stdlib.h> and others should be searched first in the fake libc
preproc_args = [ "-I{}".format(LIBC_HEADERS_DIR) ]

# we add the definitions
for name, value in PREPROCESSOR_DEFINITIONS.items():
    if value is None:
        preproc_args.append("-D{}".format(name))
    else:
        preproc_args.append("-D{}={}".format(name, value))

# here `cpp` means C PreProcessor (and not C++)
ast = parse_file(SOURCE_FILE, use_cpp=True, cpp_args=preproc_args)

#=== RENAMING ===
# This step is in charge of renaming characters to:
# - omit the PREFIX (which is replaced by NAMESPACE in the generated .hpp)
# - add `std::` if the entity is believed to come from standard libraries
# - add `STATIC_NAMESPACE::` if the entity was marked static (not accessible from outside)
print("Renaming...")

# how should we rename entities
mapping_entities = {}
# entities that needs to be prefixed by STATIC_NAMESPACE
static_entities = set()

# checks wheter an entity name must be mapped to another name
def rename_entity(value):
    
    if isinstance(value, str) and value not in PREVENT_RENAMING:

        if value in mapping_entities:
            value = mapping_entities[value]

        if value in static_entities:
            return STATIC_NAMESPACE + "::" + value

        elif value.upper().startswith(PREFIX.upper()):
            newvalue = value[len(PREFIX):]
            mapping_entities[value] = newvalue
            
            return newvalue

    return value

# renames every encoutered entity if needed (change namespace)
class RenamingVisitor(c_ast.NodeVisitor):

    def __init__(self):
        self.parent = 0

    def generic_visit(self, node):
        if node.coord is not None and node.coord.file.startswith(LIBC_PREFIX_PATH):
            # is from a standard header
            if not node.name.startswith("_"):
                # this is a public name that may be referenced in the code
                mapping_entities[node.name] = "std::" + node.name
            return
        
        # for every attribute that may reference an entity
        for attrname in node.attr_names:
            value = getattr(node, attrname)

            if isinstance(value, str):
                setattr(node, attrname, rename_entity(value))
            elif isinstance(value, list):
                newvalue = [ rename_entity(v) for v in value]
                setattr(node, attrname, newvalue)
        
        # iterate other its children
        oldparent = self.parent
        self.parent = node
        for c in node:
            self.visit(c)
        self.parent = oldparent
    
    def visit_Decl(self, node):
        self.generic_visit(node)

        if "static" in node.storage:
            if isinstance(self.parent, c_ast.Compound):
                # this is a variable declaration, not part of STATIC_NAMESPACE
                node.storage = []
                node.funcspec = ["constexpr"]
            else:
                # top-level declaration which will be in STATIC_NAMESPACE
                static_entities.add(node.name)

    
visitor = RenamingVisitor()
visitor.visit(ast)

#=== AST STRUCTURE MODIFICATION ===
# This step modifies the AST, by removing and adding top-level nodes.
# - removing `extern` declarations which have no use in a header-only library
# - converting `typedef enum { ... } my_enum_t;` to:
#     enum { ... };
#     typedef int my_enum_t;
#   this is needed because it seems bitwise operations do not work well with enum types in C++??
# - adding constexpr to these declarations
print("Structure modifications on top-level declarations...")

# enable adding declarations at top-level
to_add = []

# enable filtering of entities that are not wanted
to_remove = []

# we modify top-level declarations
for index,node in enumerate(ast.ext):
    if node.coord.file.startswith(LIBC_PREFIX_PATH):
        # `node` is standard, must not be generated in the .hpp
        to_remove.append(node)
        continue

    # if declaration
    if isinstance(node, c_ast.Decl):
        if "extern" in node.storage:
            # extern has no use in constexpr
            to_remove.append(node)
        else:
            node.funcspec = ["constexpr"]
        
    elif isinstance(node, c_ast.Typedef):
        # check if this is a typedef of an enumeration
        if isinstance(node.type, c_ast.TypeDecl) and isinstance(node.type.type, c_ast.Enum):
            enum = node.type.type
            node.type.type = c_ast.IdentifierType(names=["int"])
            ast.ext[index] = enum
            to_add.append( (index+1, node) )

    elif isinstance(node, c_ast.FuncDef):
        node.decl.funcspec = ["constexpr"]

    else:
        # unknown node, we remove it
        to_remove.append(node)

for nb_added,(index,node) in enumerate(to_add):
    # adding an element shifts all the elements by 1,
    # so, it is why we do `nb_added + index` instead of only `index`
    ast.ext.insert(nb_added + index, node)

for node in to_remove:
    ast.ext.remove(node)

#=== OUTPUT ===
print("Generating output...")


with open(OUTPUT_FILE, "w") as f:
    
    # start of header guards
    f.write("#ifndef " + OUTPUT_HEADER_GUARD + "\n")
    f.write("#define " + OUTPUT_HEADER_GUARD + "\n")
    
    f.write("\n")

    # includes
    for include in all_includes:
        f.write("#include <{}>\n".format(include))
    
    f.write("\n")

    # defines
    for name,value in public_defines.items():
        f.write("constexpr auto {} = {};\n".format(name,value))
    
    # namespace begin
    f.write("\n" + "namespace " + NAMESPACE + " {\n\n")


    generator = c_generator.CGenerator()
    # we iterate over individual entities to put them correctly in namespaces
    in_detail_namespace = False
    for node in ast.ext:

        detail = False
        if isinstance(node, c_ast.Decl) and "static" in node.storage:
            detail = True
            node.storage = []
        elif isinstance(node, c_ast.FuncDef) and "static" in node.decl.storage:
            detail = True
            node.decl.storage = []

        if detail and not in_detail_namespace:
            f.write("namespace " + STATIC_NAMESPACE + " {\n")
            in_detail_namespace = True
        
        elif not detail and in_detail_namespace:
            f.write("} // namespace " + STATIC_NAMESPACE + "\n")
            in_detail_namespace = False

        f.write(generator.visit(node))

        if not isinstance(node, c_ast.FuncDef):
            f.write(";")
        f.write("\n")
    
    if in_detail_namespace:
        f.write("} // namespace " + STATIC_NAMESPACE + "\n")

    # namespace end
    f.write("\n} // namespace " + NAMESPACE + "\n\n")

    #end of headerguard
    f.write("#endif\n")

print("Done!")