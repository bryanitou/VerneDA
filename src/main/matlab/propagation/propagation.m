% Clear everything
bdclose all; % clear all libraries out of memory ( supposedly )
clear all;   % clear all workspace variables, mex, etc. ( supposedly )
rehash;      % cause all .m files to be reparsed when invoked again
% Set environment
env;

% Orbit Period
orbit_period_sec = 15361.68181049502; % seconds

% Get scaling parameters
mex_scaling;

% Set the initial state, standard deviation and CI
state_ini_dim = [6.6781*1.0e+03 , 0.0, 0.0, 0, 9.4621, 0.0164];
state_ini = [state_ini_dim(1:3) / scaling_length, state_ini_dim(4:6) / scaling_length];
stddev_dim = [10.0, 100.0, 0.0, 0.0, 0.0, 0.0437];
stddev = [stddev_dim(1:3) / scaling_length, stddev_dim(4:6) / scaling_length];
ci = 3;
betas = ci * stddev;

% Get DA vector
state_DA_ini = mex_get_DA_state(state_ini, betas);
fprintf("%s", string(state_DA_ini))

% Propagation time
t0 = 0.0;
tf = 1000;
dt = 10;
t = [t0, tf, dt] / scaling_time;

% NLI
nli = 0.02;

% Propagate DA vector
state_DA_fin = mex_propagate_DA_state(state_DA_ini, betas, t, nli);



