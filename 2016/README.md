This directory contains reference proton reconstruction:
 * `alignment.h` : routines for loading alignment constants
 * `fill_info.h` : fill-run-alignment mapping
 * `track_lite.h` : lite track object (will be soon replaced by miniAOD format)
 * `proton_reconstruction.h` : proton reconstruction code
 * `reference_reconstruction_example.cc`, `makefile` : usage example

The code is tested with CMSSW_8_0_12.

The code currently reads in AOD data for the "di-muon" candidates from CMS AN-16-481.

The example can be run by following these steps:
 1. initialise the CMSSW environment
 1. run `make`
 1. run `./reference_reconstruction_example`
