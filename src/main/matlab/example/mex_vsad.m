% Clear workspace and tabs
close all;
clear all;

% Inertia
inertia = [
    2040.0, 130.0, 25.0;
    130.0, 1670.0, -55.0;
    25.0, -55.0, 2570.0
    ];

% Order of rotation
order = "XYZ";

% Set initial state: ALREADY ADIMENTIONAL
euler = [0.0, 0.0, 0.01];
qZYX = eul2quat(euler, order);
vel = [0.01, 0.0, 0.0];
state_ini = [qZYX, vel];

% Stddev
stddev_eul = [0.03, 0.03, 0.03];
stddev_eul_err = stddev_ompl(stddev_eul);
stddev_q_err = eul2quat(stddev_eul_err, order);
stddev_vel = [0.0, 0.0, 0.0];
stddev = [stddev_q_err, stddev_vel];

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
% NOTE: Usually this propagation is trivial given that in FTMP the
% propagation of the quaternion is proportional (it is non non-linear).
% Inputs
% state_ini: quaternion and rad/s
% stddev: quaternion error and rad/s
b = mex_vsaod(state_ini, stddev, t, ci, nli, n_split_max, n_samples, "ftmp", inertia);

% Convert to degrees
b_deg = rad2deg(b);

figure(1);
tiledlayout(1,3)

xy_ax = nexttile;
plot(b_deg(1,:), b_deg(2,:), '*');
title(xy_ax,'XY')
xlabel(xy_ax,'deg')
grid on;

xz_ax = nexttile;
plot(b_deg(1,:), b_deg(3,:), '*');
title(xz_ax,'XZ')
xlabel(xz_ax,'deg')
grid on;

yz_ax = nexttile;
plot(b_deg(2,:), b_deg(3,:), '*');
title(yz_ax,'YZ')
xlabel(yz_ax,'deg')
grid on;
