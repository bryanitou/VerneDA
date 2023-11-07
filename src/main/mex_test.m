% Clear workspace and tabs
close all;
clear all;

% Set initial state
state_ini = [0.5, 0.0, 0.0, 0.0,1.7320508075688774, 0.003];

% Set time constraints
t0 = 0.0;
tf = 9.42477796076938 / 2 * 1.3;
dt = 0.004090167590170333;
t = [t0, tf, dt];

% Stddev
stddev = [7.487120281336031E-4, 0.007487120281336032, 0.0, 0.0, 0.0, 0.008];
ci = 3;

% LOADS configuration
nli = 0.02;
n_split_max = int8(10);

% Test MEX executable
b = mex_vsod(state_ini, stddev, t, ci, nli, n_split_max);

% Earth data
r_earth = 6378;
r_earth = r_earth / 13356.27;

% Plot the result
plot3(b(1,:), b(2,:), b(3,:), '*')
[x ,y , z ] = sphere ;
x = x * r_earth ;
y = y * r_earth ;
z = z * r_earth ;
hold on;
surf (x ,y , z ) ;
axis equal ;
xlabel (" x [ km ]") ;
ylabel (" y [ km ]") ;
zlabel (" z [ km ]") ;