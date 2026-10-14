#!/bin/sh
# The casing test data share a downloaded prerequisite. Recursive makes must
# not download it twice when these targets are requested in parallel.
set -eu

scratch=$(mktemp -d)
trap 'rm -rf "$scratch"' EXIT HUP INT TERM
mkdir "$scratch/data"
cp Makefile "$scratch/Makefile"
cp data/Makefile "$scratch/data/Makefile"

cat > "$scratch/curl" <<'EOF'
#!/bin/sh
set -eu
while [ "$1" != '-o' ]; do shift; done
shift
printf '%s\n' "$1" >> ../downloads.log
sleep 0.1
printf 'complete\n' > "$1"
EOF

cat > "$scratch/julia" <<'EOF'
#!/bin/sh
set -eu
test "$(cat DerivedCoreProperties.txt)" = complete
printf 'generated\n'
EOF
chmod +x "$scratch/curl" "$scratch/julia"

MAKEFLAGS= MFLAGS= ${MAKE:-make} -C "$scratch" -j8 data/Lowercase.txt data/Uppercase.txt \
    CURL="$scratch/curl" JULIA="$scratch/julia"

downloads=$(wc -l < "$scratch/downloads.log")
test "$downloads" -eq 1 || {
    echo "Expected one shared data download, got $downloads" >&2
    exit 1
}
test "$(cat "$scratch/data/Lowercase.txt")" = generated
test "$(cat "$scratch/data/Uppercase.txt")" = generated
