% Clear workspace and tabs
close all;
clear all;

% Inertia
inertia = [ 2040.0, 130.0, 25.0;
        130.0, 1670.0, -55.0;
        25.0, -55.0, 2570.0];

% Set initial state: ALREADY ADMINETIONAL
euler = [0.0, 0.0, 0.0];
qZYX = eul2quat(euler);
state_ini = [qZYX, 0.01, 0.0, 0.0];

% Stddev
stddev_pos = [0.0, 0.0, 0.0];
stddev_eul = stddev_ompl(stddev_pos);
stddev_qerr = eul2quat(stddev_eul);
stddev_vel = [0.03, 0.03, 0.03];
stddev = [stddev_qerr, stddev_vel];

% Confidence interval
ci = 1;

% LOADS configuration
nli = 0.02;
n_split_max = int16(10);
n_samples = int16(10000);

% Set time constraints
t0 = 0.0;
tf = 1000;
dt = 1;
t = [t0, tf, dt];

% Test MEX executable
b = mex_vsaod(state_ini, stddev, t, ci, nli, n_split_max, n_samples, "ftmp", inertia);
