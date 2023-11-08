% Set mex file
dace_include = '-Ibuild/VerneDA-install-debug/lib/dace/include';
dace_lib_dir = '-Lbuild/VerneDA-install-debug/lib/dace/lib';
verne_include = '-Isrc/core';
verne_lib_dir = '-Lbuild/VerneDA-install-debug/lib';
mex_file = 'src/main/mex_vsaod.cpp';
CXXFLAGS = 'CXXFLAGS=$CXXFLAGS -std=c++20 -Wall';
CXX = 'CXX=/usr/bin/g++';
outdir = 'mex';

% Make dir if non-existing
if ~exist(outdir, 'dir')
   mkdir(outdir)
end

% Build mex file
mex('-v', dace_include, dace_lib_dir, '-ldace', verne_include, verne_lib_dir, '-lcore', '-lads', '-ltools', mex_file, '-outdir', outdir, CXX, CXXFLAGS)