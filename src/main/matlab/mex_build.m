% Clear in matlab cache/memory stuff related to old compiled mex functions
clear all;

% Set mex file
dace_lib_dir = 'build/VerneDA-install-debug/lib/dace/lib';
dace_include_arg = '-Ibuild/VerneDA-install-debug/lib/dace/include';
dace_lib_dir_arg = strcat('-L', dace_lib_dir);
verne_lib_dir = 'build/VerneDA-install-debug/lib';
verne_include_arg = '-Isrc/core';
verne_lib_dir_arg =  strcat('-L', verne_lib_dir);
mex_file_cpp = 'mex_vsaod.cpp';
mex_file_path_cpp = strcat('src/main/mex/', mex_file_cpp);
CXXFLAGS = 'CXXFLAGS=$CXXFLAGS -std=c++20 -Wall';
LDFLAGS = strcat('LDFLAGS=$LDFLAGS -Wl,-rpath,', dace_lib_dir,',-rpath,', verne_lib_dir);
CXX = 'CXX=/usr/bin/g++';
outdir = 'mexa64';
mex_file_path_mexa64 = strrep(strcat(outdir, "/", mex_file_cpp), '.cpp', '.mexa64');

% Make dir and clean files if existing
if ~exist(outdir, 'dir')
   mkdir(outdir)
else
    mex_file_path_mexa64 = strrep(strcat(outdir, "/", mex_file_cpp), '.cpp','.mexa64');
   if exist(mex_file_path_mexa64, 'file')
       delete(mex_file_path_mexa64)
       fprintf("Deleted .mexa64 existing file...: %s\n", mex_file_path_mexa64);
   end
end


% Build mex file
mex('-v', dace_include_arg, dace_lib_dir_arg, '-ldace', verne_include_arg, verne_lib_dir_arg, '-lcore', '-lads', '-ltools', '-lmex_utils', mex_file_path_cpp, '-outdir', outdir, CXX, CXXFLAGS, LDFLAGS)