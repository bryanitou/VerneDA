% Set mex file
dace_include = '-Ibuild/VerneDA-install-debug/lib/dace/include';
dace_lib_dir = '-Lbuild/VerneDA-install-debug/lib/dace/lib';
core_include = '-Isrc/core';
mex_file = 'src/main/mex_vsod.cpp';
CXXFLAGS = 'CXXFLAGS=$CXXFLAGS -std=c++20 -Wall';
CXX = 'CXX=/usr/bin/g++';
outdir = 'mex';
mex('-v', dace_include, dace_lib_dir, '-ldace', core_include, mex_file, '-outdir', outdir, CXX, CXXFLAGS)