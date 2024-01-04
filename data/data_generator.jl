using OffsetArrays: Origin

parsehex(str) = parse(UInt32, str, base=16)

function parse_hex_range(line)
    m = match(r"^([0-9A-F]+)(\.\.([0-9A-F]+))? +; +([^#]+)", line)
    if isnothing(m)
        return nothing
    end
    i = parsehex(m[1])
    j = !isnothing(m[3]) ? parsehex(m[3]) : i
    desc = rstrip(m[4])
    return (i:j, desc)
end

function read_hex_ranges(filename)
    [r for r in parse_hex_range.(readlines(filename)) if !isnothing(r)]
end

function collect_codepoints(range_desc, description)
    list = UInt32[]
    for (r,d) in range_desc
        if d == description
            append!(list, r)
        end
    end
    list
end

function set_all!(d, keys, value)
    for k in keys
        d[k] = value
    end
end

#-------------------------------------------------------------------------------

derived_core_properties = read_hex_ranges("DerivedCoreProperties.txt")

ignorable = Set(collect_codepoints(derived_core_properties, "Default_Ignorable_Code_Point"))
uppercase = Set(collect_codepoints(derived_core_properties, "Uppercase"))
lowercase = Set(collect_codepoints(derived_core_properties, "Lowercase"))


#-------------------------------------------------------------------------------
function derive_indic_conjunct_break(derived_core_properties)
    props = Dict{UInt32, String}()
    set_all!(props, collect_codepoints(derived_core_properties, "InCB; Linker"),    "LINKER")
    set_all!(props, collect_codepoints(derived_core_properties, "InCB; Consonant"), "CONSONANT")
    set_all!(props, collect_codepoints(derived_core_properties, "InCB; Extend"),    "EXTEND")
    props
end

let indic_conjunct_break = derive_indic_conjunct_break(derived_core_properties)
    global function get_indic_conjunct_break(code)
        get(indic_conjunct_break, code, "NONE")
    end
end

#-------------------------------------------------------------------------------
function read_grapheme_boundclasses(grapheme_break_filename, emoji_data_filename)
    grapheme_boundclass = Dict{UInt32, String}()
    for (r,desc) in read_hex_ranges(grapheme_break_filename)
        set_all!(grapheme_boundclass, r, Base.uppercase(desc))
    end
    for (r,desc) in read_hex_ranges(emoji_data_filename)
        if desc == "Extended_Pictographic"
            set_all!(grapheme_boundclass, r, "EXTENDED_PICTOGRAPHIC")
        elseif desc == "Emoji_Modifier"
            set_all!(grapheme_boundclass, r, "EXTEND")
        end
    end
    return grapheme_boundclass
end

let grapheme_boundclasses = read_grapheme_boundclasses("GraphemeBreakProperty.txt", "emoji-data.txt")
    global function get_grapheme_boundclass(code)
        get(grapheme_boundclasses, code, "OTHER")
    end
end

#-------------------------------------------------------------------------------
function read_composition_exclusions(pattern)
    section = match(pattern, read("CompositionExclusions.txt",String)).match
    es = UInt32[]
    for line in split(section, '\n')
        m = match(r"^([0-9A-F]+) +#"i, line)
        if !isnothing(m)
            push!(es, parsehex(m[1]))
        end
    end
    es
end

exclusions = Set(read_composition_exclusions(r"# \(1\) Script Specifics.*?# Total code points:"s))
excl_version = Set(read_composition_exclusions(r"# \(2\) Post Composition Version precomposed characters.*?# Total code points:"s))

#-------------------------------------------------------------------------------
function read_case_folding(filename)
    case_folding = Dict{UInt32,Vector{UInt32}}()
    for line in readlines(filename)
        m = match(r"^([0-9A-F]+); [CF]; ([0-9A-F ]+);"i, line)
        !isnothing(m) || continue
        case_folding[parsehex(m[1])] = parsehex.(split(m[2]))
    end
    case_folding
end

let case_folding = read_case_folding("CaseFolding.txt")
    global function get_case_folding(code)
        get(case_folding, code, nothing)
    end
end

#-------------------------------------------------------------------------------
# Utilities for reading per-char properties from UnicodeData.txt
function split_unicode_data_line(line)
    m = match(r"""
      ([0-9A-F]+);        # code
      ([^;]+);            # name
      ([A-Z]+);           # general category
      ([0-9]+);           # canonical combining class
      ([A-Z]+);           # bidi class
      (<([A-Z]*)>)?       # decomposition type
      ((\ ?[0-9A-F]+)*);  # decompomposition mapping
      ([0-9]*);           # decimal digit
      ([0-9]*);           # digit
      ([^;]*);            # numeric
      ([YN]*);            # bidi mirrored
      ([^;]*);            # unicode 1.0 name
      ([^;]*);            # iso comment
      ([0-9A-F]*);        # simple uppercase mapping
      ([0-9A-F]*);        # simple lowercase mapping
      ([0-9A-F]*)$        # simple titlecase mapping
    """ix, line)
    @assert !isnothing(m)
    code = parse(UInt32, m[1], base=16)
    (code             = code,
     name             = m[2],
     category         = m[3],
     combining_class  = parse(Int, m[4]),
     bidi_class       = m[5],
     decomp_type      = m[7],
     decomp_mapping   = m[8] == "" ? nothing : parsehex.(split(m[8])),
     bidi_mirrored    = m[13] == "Y",
     # issue #130: use nonstandard uppercase ß -> ẞ
     # issue #195: if character is uppercase but has no lowercase mapping,
     #             then make lowercase mapping = itself (vice versa for lowercase)
     uppercase_mapping = m[16] != ""                      ? parsehex(m[16]) :
                         code  == 0x000000df              ? 0x00001e9e      :
                         m[17] == "" && code in lowercase ? code            :
                         nothing,
     lowercase_mapping = m[17] != ""                      ? parsehex(m[17]) :
                         m[16] == "" && code in uppercase ? code            :
                         nothing,
     titlecase_mapping = m[18] != ""         ? parsehex(m[18]) :
                         code  == 0x000000df ? 0x00001e9e      :
                         nothing,
    )
end

function read_unicode_data(filename)
    raw_char_props = split_unicode_data_line.(readlines(filename))
    char_props = Origin(0)(Vector{eltype(raw_char_props)}())
    @assert issorted(raw_char_props, by=c->c.code)
    raw_char_props = Iterators.Stateful(raw_char_props)
    while !isempty(raw_char_props)
        c = popfirst!(raw_char_props)
        if occursin(", First>", c.name)
            nc = popfirst!(raw_char_props)
            @assert occursin(", Last>", nc.name)
            name = replace(c.name, ", First"=>"")
            for i in c.code:nc.code
                push!(char_props, (; c..., name=name, code=i))
            end
        else
            push!(char_props, c)
        end
    end
    return char_props
end

char_props = read_unicode_data("UnicodeData.txt")
char_hash = Dict(c.code=>c for c in char_props)

#-------------------------------------------------------------------------------
# Read character widths from UAX #11: East Asian Width
function read_east_asian_widths(filename)
    ea_widths = Dict{UInt32,Int}()
    for (rng,widthcode) in read_hex_ranges(filename)
        w = widthcode == "W" || widthcode == "F" ? 2 : # wide or full
            widthcode == "Na"|| widthcode == "H" ? 1 : # narrow or half-width
            nothing
        if !isnothing(w)
            set_all!(ea_widths, rng, w)
        end
    end
    return ea_widths
end

let ea_widths = read_east_asian_widths("EastAsianWidth.txt")
    # Following work by @jiahao, we compute character widths using a combination of
    #   * character category
    #   * UAX 11: East Asian Width
    #   * a few exceptions as needed
    # Adapted from http://nbviewer.ipython.org/gist/jiahao/07e8b08bf6d8671e9734
    global function derive_char_width(code, category)
        # Use a default width of 1 for all character categories that are
        # letter/symbol/number-like, as well as for unassigned/private-use chars.
        # This provides a useful nonzero fallback for new codepoints when a new
        # Unicode version has been released.
        width = 1

        # Various zero-width categories
        #
        # "Sk" not included in zero width - see issue #167
        if category in ("Mn", "Mc", "Me", "Zl", "Zp", "Cc", "Cf", "Cs")
            width = 0
        end

        # Widths from UAX #11: East Asian Width
        eaw = get(ea_widths, code, nothing)
        if !isnothing(eaw)
            width = eaw
        end

        # A few exceptional cases, found by manual comparison to other wcwidth
        # functions and similar checks.
        if category == "Mn"
            width = 0
        end

        if code == 0x00ad
            # Soft hyphen is typically printed as a hyphen (-) in terminals.
            width = 1
        elseif code == 0x2028 || code == 0x2029
            #By definition, should have zero width (on the same line)
            #0x002028 ' ' category: Zl name: LINE SEPARATOR/
            #0x002029 ' ' category: Zp name: PARAGRAPH SEPARATOR/
            width = 0
        end

        return width
    end
end

#-------------------------------------------------------------------------------
# Construct data tables which will drive libutf8proc
#
# These tables are "compressed" with an ad-hoc compression scheme (largely some
# simple deduplication and indexing) which can easily and efficiently be
# decompressed on the C side at runtime.

# Inverse decomposition mapping tables for combining two characters into a single one.
comb1st_indices = Dict{UInt32,Int}()
comb1st_indices_sorted_keys = Origin(0)(UInt32[])
comb2nd_indices = Dict{UInt32,Int}()
comb2nd_indices_sorted_keys = Origin(0)(UInt32[])
comb2nd_indices_nonbasic = Set{UInt32}()
comb_array = Origin(0)(Vector{Dict{Int,UInt32}}())
for char in char_props
    if isnothing(char.decomp_type) && !isnothing(char.decomp_mapping) &&
            length(char.decomp_mapping) == 2 && !isnothing(char_hash[char.decomp_mapping[1]]) &&
            char_hash[char.decomp_mapping[1]].combining_class == 0 &&
            char.code ∉ exclusions
        dm0 = char.decomp_mapping[1]
        dm1 = char.decomp_mapping[2]
        if !haskey(comb1st_indices, dm0)
            comb1st_indices[dm0] = length(comb1st_indices)
            push!(comb1st_indices_sorted_keys, dm0)
            push!(comb_array, Dict{Int,UInt32}())
            @assert length(comb1st_indices) == length(comb_array)
        end
        if !haskey(comb2nd_indices, dm1)
            push!(comb2nd_indices_sorted_keys, dm1)
            comb2nd_indices[dm1] = length(comb2nd_indices)
        end
        @assert !haskey(comb_array[comb1st_indices[dm0]], comb2nd_indices[dm1])
        comb_array[comb1st_indices[dm0]][comb2nd_indices[dm1]] = char.code
        if char.code > 0xFFFF
            push!(comb2nd_indices_nonbasic, dm1)
        end
    end
end

comb_indices = Dict{UInt32,Int}()
comb1st_indices_lastoffsets = Origin(0)(zeros(Int, length(comb1st_indices)))
comb1st_indices_firstoffsets = Origin(0)(zeros(Int, length(comb1st_indices)))
let
    cumoffset = 0
    for dm0 in comb1st_indices_sorted_keys
        index = comb1st_indices[dm0]
        first = nothing
        last = nothing
        offset = 0
        for b in eachindex(comb2nd_indices_sorted_keys)
            dm1 = comb2nd_indices_sorted_keys[b]
            if haskey(comb_array[index], b)
                if isnothing(first)
                    first = offset
                end
                last = offset
                if dm1 in comb2nd_indices_nonbasic
                    last += 1
                end
            end
            offset += 1
            if dm1 in comb2nd_indices_nonbasic
                offset += 1 
            end
        end
        comb1st_indices_firstoffsets[index] = first
        comb1st_indices_lastoffsets[index] = last
        @assert !haskey(comb_indices, dm0)
        comb_indices[dm0] = cumoffset
        cumoffset += last - first + 1 + 2
    end

    offset = 0
    for dm1 in comb2nd_indices_sorted_keys
        @assert !haskey(comb_indices, dm1)
        comb_indices[dm1] = 0x8000 | (comb2nd_indices[dm1] + offset)
        @assert comb2nd_indices[dm1] + offset <= 0x4000
        if dm1 in comb2nd_indices_nonbasic
            comb_indices[dm1] |= 0x4000
            offset += 1
        end
    end
end

utf16_encode(utf32_seq) = transcode(UInt16, transcode(String, utf32_seq))

# Utility for packing all UTF-16 encoded sequences into one big array
struct UTF16Sequences
    storage::Vector{UInt16}
    indices::Dict{Vector{UInt16},Int}
end
UTF16Sequences() = UTF16Sequences(UInt16[], Dict{Vector{UInt16},Int}())

"""
Return "sequence code" (seqindex in the C code) for a sequence: a UInt16 where
* The 14 low bits are the index into the `sequences.storage` array where the
  sequence resides
* The two top bits are the length of the sequence, or if equal to 3, the first
  entry of the sequence itself contains the length.
"""
function encode_sequence!(sequences::UTF16Sequences, utf32_seq::Vector)
    if length(utf32_seq) == 0
        return typemax(UInt16)
    end
    # lencode contains the length of the UTF-32 sequence after decoding
    # No sequence has len 0, so we encode len 1 as 0, len 2 as 1.
    # We have only 2 bits for the length, though, so longer sequences are
    # encoded in the sequence data itself.
    seq_lencode = length(utf32_seq) - 1
    utf16_seq = utf16_encode(utf32_seq)
    idx = get!(sequences.indices, utf16_seq) do
        i = length(sequences.storage)
        utf16_seq_enc = seq_lencode < 3 ? utf16_seq :
                        pushfirst!(copy(utf16_seq), seq_lencode)
        append!(sequences.storage, utf16_seq_enc)
        i
    end
    @assert idx <= 0x3FFF
    seq_code = idx | (min(seq_lencode, 3) << 14)
    return seq_code
end

function encode_sequence!(sequences::UTF16Sequences, code::Integer)
    encode_sequence!(sequences, [code])
end

function encode_sequence!(sequences::UTF16Sequences, ::Nothing)
    return typemax(UInt16)
end

function char_table_properties!(sequences, char)
    code = char.code

    return (
        category             = char.category,
        combining_class      = char.combining_class,
        bidi_class           = char.bidi_class,
        decomp_type          = char.decomp_type,
        decomp_seqindex      = encode_sequence!(sequences, char.decomp_mapping),
        casefold_seqindex    = encode_sequence!(sequences, get_case_folding(code)),
        uppercase_seqindex   = encode_sequence!(sequences, char.uppercase_mapping),
        lowercase_seqindex   = encode_sequence!(sequences, char.lowercase_mapping),
        titlecase_seqindex   = encode_sequence!(sequences, char.titlecase_mapping),
        comb_index           = get(comb_indices, code, typemax(UInt16)),
        bidi_mirrored        = char.bidi_mirrored,
        comp_exclusion       = code in exclusions || code in excl_version,
        ignorable            = code in ignorable,
        control_boundary     = char.category in ("Zl", "Zp", "Cc", "Cf") &&
                               !(char.code in (0x200C, 0x200D)),
        charwidth            = derive_char_width(code, char.category),
        boundclass           = get_grapheme_boundclass(code),
        indic_conjunct_break = get_indic_conjunct_break(code),
    )
end

# Many character properties are duplicates. Deduplicate them, constructing a
# per-character array of indicies into the properties array
sequences = UTF16Sequences()
char_table_props = [char_table_properties!(sequences, cp) for cp in char_props]

deduplicated_props = Origin(0)(Vector{eltype(char_table_props)}())
char_property_indices = Origin(0)(zeros(Int, 0x00110000))
let index_map = Dict{eltype(char_table_props),Int}()
    for (char, table_props) in zip(char_props, char_table_props)
        entry_idx = get!(index_map, table_props) do
            idx = length(deduplicated_props)
            push!(deduplicated_props, table_props)
            idx
        end
        # Add 1 because unassigned codes occupy slot at index 0
        char_property_indices[char.code] = entry_idx + 1
    end
end

# Now compress char_property_indices by breaking it into pages and
# deduplicating those (this works as compression because there are large
# contiguous ranges of code space with identical properties)
prop_page_indices = Int[]
prop_pages = Int[]
let
    page_size = 0x100
    page_index_map = Dict{Vector{Int}, Int}()
    for page in Iterators.partition(char_property_indices, page_size)
        page_idx = get!(page_index_map, page) do
            idx = length(prop_pages)
            append!(prop_pages, page)
            idx
        end
        push!(prop_page_indices, page_idx)
    end
end

#-------------------------------------------------------------------------------
function write_c_index_array(io, array, linelen)
    print(io, "{\n  ")
    i = 0
    for x in array
        i += 1
        if i == linelen
            i = 0
            print(io, "\n  ")
        end
        print(io, x, ", ")
    end
    print(io, "};\n\n")
end

function c_enum_name(prefix, str)
    if isnothing(str)
        return "0"
    else
        return "UTF8PROC_$(prefix)_$(Base.uppercase(str))"
    end
end

function c_uint16(seqindex)
    if seqindex == typemax(UInt16)
        return "UINT16_MAX"
    else
        return string(seqindex)
    end
end

function print_c_data_tables(io, sequences, prop_page_indices, prop_pages, deduplicated_props,
                             comb1st_indices_firstoffsets, comb1st_indices_lastoffsets,
                             comb2nd_indices_sorted_keys, comb_array, comb2nd_indices_nonbasic)
    print(io, "static const utf8proc_uint16_t utf8proc_sequences[] = ")
    write_c_index_array(io, sequences.storage, 8)
    print(io, "static const utf8proc_uint16_t utf8proc_stage1table[] = ")
    write_c_index_array(io, prop_page_indices, 8)
    print(io, "static const utf8proc_uint16_t utf8proc_stage2table[] = ")
    write_c_index_array(io, prop_pages, 8)

    print(io, """
        static const utf8proc_property_t utf8proc_properties[] = {
          {0, 0, 0, 0, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX,  false,false,false,false, 1, 0, UTF8PROC_BOUNDCLASS_OTHER, UTF8PROC_INDIC_CONJUNCT_BREAK_NONE},
        """)
    for prop in deduplicated_props
        print(io, "  {",
              c_enum_name("CATEGORY", prop.category), ", ",
              prop.combining_class, ", ",
              c_enum_name("BIDI_CLASS", prop.bidi_class), ", ",
              c_enum_name("DECOMP_TYPE", prop.decomp_type), ", ",
              c_uint16(prop.decomp_seqindex), ", ",
              c_uint16(prop.casefold_seqindex), ", ",
              c_uint16(prop.uppercase_seqindex), ", ",
              c_uint16(prop.lowercase_seqindex), ", ",
              c_uint16(prop.titlecase_seqindex), ", ",
              c_uint16(prop.comb_index), ", ",
              prop.bidi_mirrored, ", ",
              prop.comp_exclusion, ", ",
              prop.ignorable, ", ",
              prop.control_boundary, ", ",
              prop.charwidth, ", ",
              "0, ", # bitfield padding
              c_enum_name("BOUNDCLASS", prop.boundclass), ", ",
              c_enum_name("INDIC_CONJUNCT_BREAK", prop.indic_conjunct_break),
              "},\n"
        )
    end
    print(io, "};\n\n")

    print(io, "static const utf8proc_uint16_t utf8proc_combinations[] = {\n  ")
    i = 0
    for a in eachindex(comb1st_indices_firstoffsets)
        offset = 0
        print(io, comb1st_indices_firstoffsets[a], ", ", comb1st_indices_lastoffsets[a], ", ")
        for b in eachindex(comb2nd_indices_sorted_keys)
            dm1 = comb2nd_indices_sorted_keys[b]
            if offset > comb1st_indices_lastoffsets[a]
                break
            end
            if offset >= comb1st_indices_firstoffsets[a]
                i += 1
                if i == 8
                    i = 0
                    print(io, "\n  ")
                end
                v = get(comb_array[a], b, 0)
                if dm1 in comb2nd_indices_nonbasic
                    print(io, (v & 0xFFFF0000) >> 16, ", ")
                end
                print(io, v & 0xFFFF, ", ")
            end
            offset += 1
            if dm1 in comb2nd_indices_nonbasic
                offset += 1
            end
        end
        print(io, "\n")
    end
    print(io, "};\n\n")
end


if !isinteractive()
    print_c_data_tables(stdout, sequences, prop_page_indices, prop_pages, deduplicated_props,
                        comb1st_indices_firstoffsets, comb1st_indices_lastoffsets,
                        comb2nd_indices_sorted_keys, comb_array, comb2nd_indices_nonbasic)
end

