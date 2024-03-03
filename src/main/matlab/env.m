% Check root directory
root_dir_fpath = pwd;
root_dit_fpath_split = split(root_dir_fpath, '/');
root_dir_name = string(root_dit_fpath_split(end));

if (root_dir_name ~= 'VerneDA')
    fprintf('Error! Please set the root directory to VerneDA project. If you changed the project name, adapt this script to avoid error.\n')
end

% Directories to be added
mex_files = 'build/VerneDA-install-debug/lib';
matlab_files = 'src/main/matlab';
example_files = 'src/main/matlab/example';
propagation_files = 'src/main/matlab/propagation';

% Add to path
addpath(mex_files);
addpath(matlab_files);
addpath(example_files);
addpath(propagation_files);

% Notice user
fprintf("All paths added to Maltab environment! Pwd set to: %s\n", pwd)