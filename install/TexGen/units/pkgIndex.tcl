# Tcl package index file for units package
#
# This file is sourced either when an application starts up or by a
# "package unknown" script.  It invokes the "package ifneeded" command
# to set up package-related information so that packages will be
# loaded automatically in response to "package require" commands.
# When this script is sourced, the variable $dir must contain the full
# path name of this file's directory.
#
# This particular index file first looks for a binary shared library,
# then the Tcl-only implementation.  If the shared library is found,
# it is loaded.  Otherwise the (default) Tcl implementation is
# sourced.  This style is based on the BLT package.

proc units_makepkgindex { dir } {
    set version 1.2
    set suffix [info sharedlibextension]
    set binary_lib_file [file join $dir libunits${version}${suffix}]
    set tcl_lib_file [file join $dir units.tcl]
    if { [file exists $binary_lib_file] } {
	package ifneeded units $version [list load $binary_lib_file]
    } else {
	package ifneeded units $version [list source $tcl_lib_file]
    }
}

units_makepkgindex $dir
rename units_makepkgindex ""

