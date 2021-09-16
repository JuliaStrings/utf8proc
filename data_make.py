#!/usr/bin/python3
# Generate 'utf8proc_data.c' by parsing the Unicode data files 'UnicodeData.txt' etc.
# from the Unicode Character Database.  Tested with UCD 13.0.0 & 14.0.0-dev.
# Usage: data_make.py [options…] [<data-dir>]

import re, os, sys, fileinput, copy, datetime, getopt, platform
from collections import defaultdict

Me     = os.path.basename(sys.argv[0])
Vers   = '2.7.0dev'		# ???
DDir   = "./data"
Targ   = "utf8proc_data.out.c"
UVers  = None
gVerb  = 0
gFmt   = 0
gCMap  = False
gS1Byt = False
gFix26 = False


def Print (s):
	print(s, file=sys.stderr)

def Error (msg):
	Print("%s: ERROR: %s." % (Me, msg))

def TextFile (name):
	return DDir+ "/" +name+ ".txt"

def Sed (expr, file):
	return os.popen("/usr/bin/sed -En '" +expr+ "' '" +TextFile(file)+ "'", 'r', 1)

def Cat (file):
	return fileinput.input(files=TextFile(file), mode='r')

def Hex (s):
	return int(s, 16)

def Hex0 (s):
	return int(s, 16) if s else -1

def HexArray (hexs):
	v = []
	for h in hexs.split(' '):
		if h: v.append(int(h, 16))
	return v


try:
	opts, args = getopt.getopt(sys.argv[1:], "vxcf:o:",
							   ["verbose", "fix26", "cmap", "format=", "output="])
	omap = {'verbose':'v', 'fix26':'x', 'cmap':'c', 'format':'f', 'output':'o'}
	for o, a in opts:
		o = o.lstrip('-')
		o = omap.get(o, o)
		if   o == 'v':	gVerb += 1
		elif o == 'x':	gFix26 = True
		elif o == 'c':	gCMap  = True
	#	elif o == 'b':	gS1Byt = True
		elif o == 'f':	gFmt   = min(max(0, int(a)), 2)
		elif o == 'o':	Targ   = a

	if len(args): DDir = args[0]
	UVers = (' ' + Sed(r"1s/.+-([0-9.]+)\..+/\1/p;q",
					   "DerivedCoreProperties").readline()).strip()
	if gFmt: gFix26 = True
	if gFmt or UVers >= '14.0.0': gCMap = True
	gS1Byt = gFmt > 0
except getopt.GetoptError as err:
	Error(err)
	exit(1)

if gVerb: print("# Settings: data-format: %d  fix-2.6.1: %d  has-casemap: %d  UCD-version: %s"
				% (gFmt, gFix26, gCMap, UVers))

#--------------------------------------------------------------------------------------------------

def ParseDCProps (name, start):
	v = set()
	r1 = re.compile(r'^[0-9A-F]+')
	r2 = re.compile(r'^([0-9A-F]{4,6})\.\.([0-9A-F]+)')
	for L in Sed("/^# Derived Property: " +start+ "/,/^# Total code points:/p", "DerivedCoreProperties"):
		m = re.match(r2, L)
		if m:
			for i in range(Hex(m[1]), Hex(m[2]) + 1): v.add(i)
		else:
			m = re.match(r1, L)
			if m: v.add(Hex(m[0]))
	return v


def Ignorable ():
	return ParseDCProps("Ignorable", "Default_Ignorable_Code_Point")

def Uppercase ():
	return ParseDCProps("Uppercase", "Uppercase")

def Lowercase ():
	return ParseDCProps("Lowercase", "Lowercase")


def GraphemeBounds ():
	v = defaultdict(lambda: "Other")
	r1 = re.compile(r'^([0-9A-F]+)\s*;\s*([A-Za-z_]+)')
	r2 = re.compile(r'^([0-9A-F]{4,6})\.\.([0-9A-F]+)\s*;\s*([A-Za-z_]+)')
	for L in Cat("GraphemeBreakProperty"):
		m = re.match(r2, L)
		if m:
			C = m[3]
			for n in range(Hex(m[1]), Hex(m[2]) + 1): v[n] = C
		else:
			m = re.match(r1, L)
			if m:
				C = m[2]
				v[Hex(m[1])] = C

	r1 = re.compile(r'^([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s*Extended_Pictographic\W', re.I)
	r2 = re.compile(r'^([0-9A-F]+)\s*;\s*Extended_Pictographic\W', re.I)
	r3 = re.compile(r'^([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s*Emoji_Modifier\W', re.I)
	r4 = re.compile(r'^([0-9A-F]+)\s*;\s*Emoji_Modifier\W', re.I)
	extPict = 'EXTENDED_PICTOGRAPHIC'; extend = 'EXTEND'
	for L in Cat("emoji-data"):
		m = re.match(r1, L)
		if m:
			for e in range(Hex(m[1]), Hex(m[2]) + 1): v[e] = extPict
		else:
			m = re.match(r2, L)
			if m:
				v[Hex(m[1])] = extPict
			else:
				m = re.match(r3, L)
				if m:
					for e in range(Hex(m[1]), Hex(m[2]) + 1): v[e] = extend
				else:
					m = re.match(r4, L)
					if m: v[Hex(m[1])] = extend
	return v


def CharWidths ():
	cws = defaultdict(lambda: 1)

	# Use a default width of 1 for all character categories that are letter/symbol/number-like,
	# as well as for unassigned/private-use chars. This can be overridden by UAX 11 below,
	# but provides a useful nonzero fallback for new codepoints when a new Unicode version
	# has been released but Unifont hasn't been updated yet.

	# Categories that may contain zero-width chars
	zerowidth = set(('Mn','Mc','Me','Zl','Zp','Cc','Cf','Cs'))	# +'Sk' - see issue #167
	for c in uchars:
		if c.category in zerowidth:
			cws[c.code] = 0

	# Widths from UAX #11: East Asian Width
	# These take precedence for all codepoints listed explicitly as wide/full/narrow/half-width
	rx = re.compile(r'^([0-9A-F]{4,6})(\.\.([0-9A-F]{4,6}))?;([AFHNW]a?)(\s*#\s*Cn\W)?')
	for L in Cat("EastAsianWidth"):
		m = re.match(rx, L)
		if m:
			width  = m[4]
			cstart = Hex(m[1])
			cend   = Hex(m[3]) if m.end(3) > 0 else cstart
			for c in range(cstart, cend + 1):			# Assign widths
				if not m[5]:							# skip any with a `# Cn …` comment
					if width == 'W' or width == 'F':	# wide or full
						cws[c] = 2
					elif width == 'Na' or width == 'H':
						cws[c] = 1

	# A few exceptions to the above cases, found by manual comparison
	# to other wcwidth functions and similar checks.
	for ch in uchars:
		c   = ch.code
		cat = ch.category

			# Ensure format control chars (cat Cf) have width 0 (some of these, like U+0601,
			# can have a width in some cases but normally act like prepended combining marks.
			#  U+FFF9 etc. are also odd, but have zero width in typical terminal contexts)
		if cat == 'Cf':						cws[c] = 0

			# Unifont has nonzero width for a number of non-spacing combining characters,
			# e.g. (in 7.0.06): F84,17B4,17B5,180B,180D,2D7F and the variation selectors
		elif cat == 'Mn':					cws[c] = 0

			# We also assign width of one to unassigned and private-use codepoints
			# (Unifont includes ConScript Unicode Registry PUA fonts, but since these
			# are nonstandard it seems questionable to use Unifont metrics;
			# if they are printed as the replacement char U+FFFD they will have width 1).
		elif cat == 'Co' or cat == 'Cn':	cws[c] = 1

			# For some reason, Unifont has width-2 glyphs for ASCII control chars
		elif cat == 'Cc':					cws[c] = 0

	# Soft hyphen is typically printed as a hyphen (-) in terminals.
	cws[0x00AD] = 1

	# By definition, should have zero width (on the same line)
	cws[0x2028] = 0		# category: Zl, name: LINE SEPARATOR
	cws[0x2029] = 0		# category: Zp, name: PARAGRAPH SEPARATOR
	return cws


def CompExclusions (start, name):
	v = set()
	rx = re.compile(r'^[0-9A-F]+')
	for L in Sed("/^# " +start+ "/,/^# Total code points:/p", "CompositionExclusions"):
		m = re.match(rx, L)
		if m: v.add(Hex(m[0]))
	return v


def Exclusions ():
	v = CompExclusions("\(1\) Script Specifics", "Exclusions")
	# data_generator.rb erroneously adds `0` in lines 136 & 139 (for each comment line)
	if not gFix26: v.add(0)		#•••
	return v


def Precomposed ():
	return CompExclusions("\(2\) Post Composition Version precomposed characters", "Precomposed")


def CaseFolding ():
	v = {}
	rx = re.compile(r'^([0-9A-F]+); [CF]; ([0-9A-F ]+);')
	for L in Cat("CaseFolding"):
		m = re.match(rx, L)
		if m: v[Hex(m[1])] = HexArray(m[2])
	return v


udRE = re.compile(r'^'
		r'([0-9A-F]+);'			#  1: code
		r'([^;]+);'				#  2: name
		r'([A-Za-z]+);'	 		#  3: general category
		r'([0-9]+);' 			#  4: canonical combining class
		r'([A-Z]+);' 			#  5: bidi class
		r'(<([A-Za-z]*)>)?'		#  7: decomposition type
		r'((\ ?[0-9A-F]+)*);'	#  8: decompomposition mapping
		r'([0-9]*);'			# 10: decimal digit
		r'([0-9]*);'			# 11: digit
		r'([^;]*);'				# 12: numeric
		r'([YN]*);'				# 13: bidi mirrored
		r'([^;]*);'				# 14: unicode 1.0 name
		r'([^;]*);'				# 15: iso comment
		r'([0-9A-F]*);'			# 16: simple uppercase mapping
		r'([0-9A-F]*);'			# 17: simple lowercase mapping
		r'([0-9A-F]*)$')		# 18: simple titlecase mapping
uchar_hash = {}
pSequences = []
seqs_hash  = {}
pCaseMap   = []
cmap_hash  = {}

def push_seq (seq):
	key = str(seq)
	if key not in seqs_hash:
		idx = len(pSequences)
		seqs_hash[key] = idx
		pSequences.extend(seq)
		return idx
	return seqs_hash[key]

def push_cas (seq):
	if not gCMap: return push_seq(seq)
	key = str(seq)
	if key not in cmap_hash:
		idx = len(pCaseMap)
		cmap_hash[key] = idx
		pCaseMap.extend(seq)
		return idx
	return cmap_hash[key]

def to_u16 (seq):
	v = []
	for cp in seq:
		if cp <= 0xFFFF:
			if (cp >> 11) == 0x1B: Error("UTF-16 code: U+%06X" % cp)
			v.append(cp)
		else:
			v += [0xD800 | ((cp - 0x10000) >> 10), 0xDC00 | (cp & 0x03FF)]
	return v

def dm_index (seq):				# decomp_map sequence
	if not seq or len(seq) == 0: return g_
	lencode = len(seq) - 1		# no sequence has len 0, so we encode len 1 as 0, len 2 as 1, …
	seq = to_u16(seq)
	if lencode >= 7:			# we have 3 bits for length (which is cutting it close.
		seq.insert(0, lencode)	# May need to change it to 2 bits in future Unicode versions)
		lencode = 7
	idx = push_seq(seq)
	if idx > 0x1FFF: Error("decomp_map: pSequences[%d] out of bounds" % idx)
	return idx | (lencode << 13)

def cf_index (seq):				# case_fold sequence
	if not gCMap: return dm_index(seq)
	if not seq or len(seq) == 0: return g_
	lencode = len(seq) - 1		# no sequence has len 0, so we encode len 1 as 0, len 2 as 1, …
	seq = to_u16(seq)
	if lencode >= 7:			# we have 3 bits for length
		seq.insert(0, lencode)
		lencode = 7
	idx = push_cas(seq)
	if idx > 0x1FFF: Error("case_fold: pCaseMap[%d] out of bounds" % idx)
	return idx | (lencode << 13)

def case_map (cp):
	if not cp: return g_
	if cp > 0xFFFF:
		idx = push_cas([0xD800 | ((cp - 0x10000) >> 10), 0xDC00 | (cp & 0x03FF)])
	else:
		if (cp >> 11) == 0x1B: Error("UTF-16 code: U+%06X" % cp)
		idx = push_cas([cp])
	if idx >= 0xFFFF: Error("case_map: pCaseMap[%d] out of bounds" % idx);
	return idx

def S (s): return s if s != None else g_

def F (f): return gT if f else gF

def D0 (s): return 'UTF8PROC_DECOMP_TYPE_' + s.upper() if s else '0'
def D1 (s): return s.upper() if s else '0'

g_ = 'UINT16_MAX'; gT = 'true'; gF = 'false'; D = D0
gPF = "  {{UTF8PROC_CATEGORY_{}, {}, UTF8PROC_BIDI_CLASS_{}, {}, {}," \
	  " {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 0, UTF8PROC_BOUNDCLASS_{}}},"
if gFmt: g_ = '_'; gT = 'T'; gF = 'F'; D = D1; \
		 gPF = "\tP({},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{})"
ZZCC = set(('Zl', 'Zp', 'Cc', 'Cf'))

class UChar:
	def __init__ (self, line):
		m = re.match(udRE, line)
		if not m: Error("Could not parse input ‘%s’" % line)
		uc = Hex(m[1])
		self.code        = uc
		self.name        = m[2]
		self.category    = m[3]
		self.comb_class  = int(m[4])
		self.bidi_class  = m[5]
		self.decomp_type = m[7]
		self.decomp_map  = HexArray(m[8]) if m[8] else None
		self.decomp_idx  = g_
		self.bidi_mirror = (m[13] == 'Y')
		# issue #130: use nonstandard uppercase ß -> ẞ
		# issue #195: if character is uppercase but has no lowercase mapping,
		#             then make lowercase mapping = itself (vice versa for lowercase)
		U = Hex0(m[16]); L = Hex0(m[17]); T = Hex0(m[18])
		self.uppercase  = (0x1E9E if uc==0x00DF else (uc if L<0 and uc in lowercase \
														 else None)) if U<0 else U
		self.lowercase  = (uc if U<0 and uc in uppercase else None) if L<0 else L
		self.titlecase  = (0x1E9E if uc==0x00DF else None) if T<0 else T
		self.case_fold  = caseFolding.get(uc)
		self.centry_idx = None

	def ctrl_bound (self):
		return F((self.category in ZZCC) and (not gFix26 or (self.code != 0x200C and
															 self.code != 0x200D)))		#•••

	def c_entry (self):
		uc      = self.code
		com_idx = combIndexes.get(uc, g_)
		comp_ex = F(uc in exclusions or uc in exclVersion)
		ignore  = F(uc in ignorable)
		gbounds = grfmBounds[uc].upper()
		self.uppercase = case_map(self.uppercase)
		self.lowercase = case_map(self.lowercase)
		self.titlecase = case_map(self.titlecase)
		return gPF.format(
				self.category.upper(), self.comb_class, self.bidi_class,
				D(self.decomp_type), S(self.decomp_idx), S(self.case_fold),
				self.uppercase, self.lowercase, self.titlecase, com_idx, F(self.bidi_mirror),
				comp_ex, ignore, self.ctrl_bound(), charWidths[uc], gbounds)


def UnicodeData ():
	uchars = []
	r1 = re.compile(r'^([0-9A-F]+);<[^;>,]+, First>;')
	r2 = re.compile(r'^([0-9A-F]+);<([^;>,]+), Last>;')
	state = True
	for L in Cat("UnicodeData"):
		if state:
			m = re.match(r1, L)
			if m:
				first = Hex(m[1])
				state = False
			else:
				ch = UChar(L)
				uchar_hash[ch.code] = ch
				uchars.append(ch)
		else:
			m = re.match(r2, L)
			if not m: Error("No last character of sequence U+%04X … found" % first)
			name  = '<' + m[2] + '>'
			ch    = UChar(L)
			state = True
			for i in range(first, Hex(m[1]) + 1):
				ch_clone = copy.copy(ch)
				ch_clone.code = i
				ch_clone.name = name
				uchar_hash[i] = ch_clone
				uchars.append(ch_clone)
	if not state: Error("No last character of sequence U+%04X … found" % first)
	return uchars


def Sequences ():
	comb1st      = {}
	comb1st_keys = []			# Ordered list of comb1st’ keys.
	comb2nd      = {}
	comb2nd_keys = []
	comb2nd_long = set()
	comb_array   = {}
	for ch in uchars:
		dm = ch.decomp_map
		if not ch.decomp_type and dm and len(dm) == 2 and dm[0] in uchar_hash and \
				uchar_hash[dm[0]].comb_class == 0 and ch.code not in exclusions:
			dm0 = dm[0]
			dm1 = dm[1]
			if dm0 not in comb1st or comb1st[dm0] == None:
				comb1st[dm0] = c1i_dm0 = len(comb1st)
				comb1st_keys.append(dm0)
			else:
				c1i_dm0 = comb1st[dm0]
			if dm1 not in comb2nd or comb2nd[dm1] == None:
				comb2nd_keys.append(dm1)
				comb2nd[dm1] = len(comb2nd)
			if not comb_array.get(c1i_dm0):
				comb_array[c1i_dm0] = {}
			if comb2nd[dm1] in comb_array[c1i_dm0]:
				Error("Duplicate canonical mapping: U+%05X %d/%d" % (ch.code, dm0, dm1))
			comb_array[c1i_dm0][comb2nd[dm1]] = ch.code
			if ch.code > 0xFFFF: comb2nd_long.add(dm1)
		ch.decomp_idx = dm_index(dm)
		ch.case_fold  = cf_index(ch.case_fold)

	comb_idxs       = {}
	comb1st_offsets = {}			# (first, last) tuples
	cur_pos = 0
	for dm0 in comb1st_keys:		# Force comb1st build order
		index  = comb1st[dm0]
		comb_i = comb_array[index]
		first  = None
		last   = None
		offset = 0; b = -1
		for dm1 in comb2nd_keys:
			b += 1
			if b in comb_i and comb_i[b] != None:
				if first == None: first = offset
				last = offset
				if dm1 in comb2nd_long: last += 1
			offset += 1
			if dm1 in comb2nd_long: offset += 1
		comb1st_offsets[index] = (first, last)
		if dm0 in comb_idxs: Error("double index at %d" % dm0)
		comb_idxs[dm0] = cur_pos
		cur_pos += last - first + 1 + 2

	offset = 0
	for dm1 in comb2nd_keys:
		if dm1 in comb_idxs: Error("double index at %d" % dm1)
		comb_idxs[dm1] = 0x8000 | (comb2nd[dm1] + offset)
		if comb2nd[dm1] + offset > 0x4000: Error("too large comb index at %d" % dm1)
		if dm1 in comb2nd_long:
			comb_idxs[dm1] = comb_idxs[dm1] | 0x4000
			offset += 1

	class SStr:						# A string stream
		def __init__ (self):      self.s = "  "
		def __lshift__ (self, x): self.s += x; return self
		def i (self, x):          self.s += "%d, " % x; return self
	class SVec:						# A vector stream
		def __init__ (self):      self.s = []
		def __lshift__ (self, x): return self
		def i (self, x):          self.s.append(x); return self

	# Create string with original line breaking or array of ints
	i = 0; s = SVec() if gFmt else SStr()
	for a in range(len(comb1st)):
		o1 = comb1st_offsets[a]
		s.i(o1[0]).i(o1[1])
		offset = 0; b = -1
		for dm1 in comb2nd_keys:
			b += 1
			if offset > o1[1]: break
			if offset >= o1[0]:
				i += 1
				if i == 8: i = 0; s << "\n  "
				v = comb_array[a][b] if b in comb_array[a] else 0
				if dm1 in comb2nd_long: s.i((v & 0xFFFF0000) >> 16)
				s.i(v & 0xFFFF)
			offset += 2 if dm1 in comb2nd_long else 1
		s << "\n"
	return s.s, comb_idxs


def WriteIntArray (array, name, file, bytes = False):
	print("static const utf8proc_uint%d_t utf8proc_%s[] = {"
			% ((8 if bytes else 16), name), file=file)
	if isinstance(array, str):
		print(array, end='', file=file)
	elif gFmt:
		i = 0; pre = "\t"
		for e in array:
			if i and i % 16 == 0: pre = ",\n\t"
			i += 1
			print("%s%d" % (pre, e), end='', file=file)
			pre = ","
		print("", file=file)
	else:
		i = 0; print("  ", end='', file=file)
		for e in array:
			i += 1
			if i == 8: print("\n  ", end='', file=file); i = 0
			print("%d, " % e, end='', file=file)
	print("};\n", file=file)


def WriteData (stage1, stage2, props, combinations):
	f = open(Targ, mode='w')
	if gFmt: print("// Generated by %s v%s from UnicodeData version %s on %s.\n"
				   "// Options: --format=%d%s%s\n\n"
				   "#define	UNICODE_VERSION	\"%s\""
				   % (Me, Vers, UVers, datetime.datetime.utcnow().strftime("%F %T"), gFmt,
					  (" --fix26" if gFix26 else ""), (" --cmap" if gCMap else ""), UVers), file=f)
	WriteIntArray(pSequences, "sequences", f)
	if gCMap:
		print("#define	U8CASEMAP", file=f)
		WriteIntArray(pCaseMap, "casemap", f)
	WriteIntArray(stage1, "stage1table", f, gS1Byt)
	WriteIntArray(stage2, "stage2table", f)
	if gFmt:
		print("#define	P(C,c,B,D,ds,cs,us,ls,ts,x,m,e,i,cb,w,b)\t"
			  "{UTF8PROC_CATEGORY_##C,c,UTF8PROC_BIDI_CLASS_##B,\\\n\t\t"
			  "UTF8PROC_DECOMP_TYPE_##D,ds,cs,us,ls,ts,x,m,e,i,cb,w,0,UTF8PROC_BOUNDCLASS_##b},\n"
			  "enum { F = false, T = true, UTF8PROC_BIDI_CLASS_0 = 0,"
			  " UTF8PROC_DECOMP_TYPE_0 = 0, _ = UINT16_MAX };\n\n"
			  "static const utf8proc_property_t utf8proc_properties[] = {\n"
			  "	P(CN,0,0,0,_,_,_,_,_,_,F,F,F,F,1,OTHER)", file=f)
	else:
		u = "UINT16_MAX, "; b = "false,"
		print("static const utf8proc_property_t utf8proc_properties[] = {\n"
			  "  {0, 0, 0, 0, "+u+u+u+u+u+u+' '+b+b+b+b+" 1, 0, UTF8PROC_BOUNDCLASS_OTHER},", file=f)
	for c in props:
		print(c, file=f)
	print("};\n", file=f)
	WriteIntArray(combinations, "combinations", f)
	if gFmt: print("// End.", file=f)
	if gVerb: print("# Wrote ‘%s’." % Targ)


ignorable   = Ignorable()
uppercase   = Uppercase()
lowercase   = Lowercase()
grfmBounds  = GraphemeBounds()		# Grapheme bounds classes
exclusions  = Exclusions()
exclVersion = Precomposed()
caseFolding = CaseFolding()
uchars      = UnicodeData()
charWidths  = CharWidths()
(pCombinations, combIndexes) = Sequences()

prop_idxs  = {}
pProperties = []
for ch in uchars:
	centry = ch.c_entry()
	ch.centry_idx = prop_idxs.get(centry)
	if not ch.centry_idx:
		prop_idxs[centry] = ch.centry_idx = len(pProperties)
		pProperties.append(centry)

pStage1 = []		# stage1table
pStage2 = []		# stage2table
chunks2 = []
scale = 1 if gS1Byt else 0x100
for page in range(0, 0x110000, 0x100):
	stage2_entry = []
	for code in range(page, page + 0x100):
		stage2_entry.append(uchar_hash[code].centry_idx + 1 if code in uchar_hash else 0)
	if stage2_entry in chunks2:
		pStage1.append(chunks2.index(stage2_entry) * scale)
	else:
		pStage1.append(len(chunks2) * scale)
		pStage2.extend(stage2_entry)
		chunks2.append(stage2_entry)

WriteData(pStage1, pStage2, pProperties, pCombinations)

