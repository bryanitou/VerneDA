%list_VerneDA_so = dir('build/VerneDA-install-debug/lib/*.so');
%list_DACE_so = dir('build/VerneDA-install-debug/lib/dace/lib/*.so');
%libstdc = '/lib/x86_64-linux-gnu/libstdc++.so.6';

%for i = 1:length(list_VerneDA_so)
%    setenv('LD_PRELOAD', fullfile(list_VerneDA_so(i).folder, list_VerneDA_so(i).name))
%end

%for i = 1:length(list_DACE_so)
%    setenv('LD_PRELOAD', fullfile(list_DACE_so(i).folder, list_DACE_so(i).name))
%end


% Set mex file
dace_include = '-Ibuild/VerneDA-install-debug/lib/dace/include';
dace_lib_dir = '-Lbuild/VerneDA-install-debug/lib/dace/lib';
verne_include = '-Isrc/core';
verne_lib_dir = '-Lbuild/VerneDA-install-debug/lib';
mex_file = 'src/main/mex_vsod.cpp';
CXXFLAGS = 'CXXFLAGS=$CXXFLAGS -std=c++20 -Wall';
CXX = 'CXX=/usr/bin/g++';
outdir = 'mex';
mex('-v', dace_include, dace_lib_dir, '-ldace', verne_include, verne_lib_dir, '-lcore', '-lads', '-ltools', mex_file, '-outdir', outdir, CXX, CXXFLAGS)