#!/bin/sh

install_dir=`mktemp -d`
if [ -z "$install_dir" ]; then
  echo "install_uninstall error: could not create temporary directory"  >&2
  exit 1
fi

# test installation
if ! make install DESTDIR="$install_dir" prefix=""; then
	echo "FAILED: make install" >&2
	rm -rf "$install_dir"
	exit 1
fi

cut -d ' ' -f 1 MANIFEST.new | while read -r installed_file; do
	if [ ! -e "$install_dir/$installed_file" ]; then
		echo "FAILED: make install of "$installed_file"" >&2
		rm -rf "$install_dir"
		exit 1
	fi
done

# test uninstallation
if ! make uninstall DESTDIR="$install_dir" prefix=""; then
	echo "FAILED: make uninstall" >&2
	rm -rf "$install_dir"
	exit 1
fi

cut -d ' ' -f 1 MANIFEST.new | while read -r installed_file; do
  [ -d "$install_dir/$installed_file" ] && continue
	if [ -f "$install_dir/$installed_file" ]; then
		echo "FAILED: make install of "$installed_file"" >&2
		rm -rf "$install_dir"
		exit 1
	fi
done

echo "install_uninstall tests SUCCEEDED"
