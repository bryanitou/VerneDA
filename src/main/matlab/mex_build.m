% Set mex file
dace_lib_dir = 'build/VerneDA-install-debug/lib/dace/lib';
dace_include_arg = '-Ibuild/VerneDA-install-debug/lib/dace/include';
dace_lib_dir_arg = strcat('-L', dace_lib_dir);
verne_lib_dir = 'build/VerneDA-install-debug/lib';
verne_include_arg = '-Isrc/core';
verne_lib_dir_arg =  strcat('-L', verne_lib_dir);
mex_file = 'src/main/mex_vsaod.cpp';
CXXFLAGS = 'CXXFLAGS=$CXXFLAGS -std=c++20 -Wall';
LDFLAGS = strcat('LDFLAGS=$LDFLAGS -Wl,-rpath,', dace_lib_dir,',-rpath,', verne_lib_dir);
CXX = 'CXX=/usr/bin/g++';
outdir = 'mex';

% Make dir if non-existing
if ~exist(outdir, 'dir')
   mkdir(outdir)
end

% Build mex file
mex('-v', dace_include_arg, dace_lib_dir_arg, '-ldace', verne_include_arg, verne_lib_dir_arg, '-lcore', '-lads', '-ltools', mex_file, '-outdir', outdir, CXX, CXXFLAGS, LDFLAGS)