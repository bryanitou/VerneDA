% Clear workspace and tabs
close all;
clear all;

% Orbit Period
orbit_period_sec = 15361.68181049502; % seconds

% Scaling
mex_scaling;

% Set initial state: ALREADY ADMINETIONAL
state_ini = [0.5, 0.0, 0.0, 0.0,1.7320508075688774, 0.003];

% Stddev
stddev = [7.487120281336031E-4, 0.007487120281336032, 0.0, 0.0, 0.0, 0.008];

% Confidence interval
ci = 3;

% LOADS configuration
nli = 0.02;
n_split_max = int16(10);
n_samples = int16(10000);

% Periods to simulate
orbits_period_sec_vec = [0.5, 0.75, 1] * orbit_period_sec;

% Set time constraints
t0 = 0.0;

% Save the results here
N = length(orbits_period_sec_vec);
b = zeros(6, n_samples + 1, N);

for i = 1:length(orbits_period_sec_vec)
    orbit_period_sec =  orbits_period_sec_vec(i);
    orbit_period_und = orbit_period_sec * 1 / scaling_time;
    tf = orbit_period_und * 0.75;
    dt = 10 / scaling_time;
    t = [t0, tf, dt];
    
    % Test MEX executable
    b(:,:,i) = mex_vsaod(state_ini, stddev, t, ci, nli, n_split_max, n_samples, "tbp");
end

figure(1);
% Plot the result
for i = 1:length(orbits_period_sec_vec)
    r = b(:,:,i);
    plot3(r(1,:), r(2,:), r(3,:), '*');
    hold on;
end
r_earth = 6378 / scaling_length;
[x ,y , z ] = sphere;
x = x * r_earth;
y = y * r_earth;
z = z * r_earth;
hold on;
surf (x ,y , z ) ;
axis equal ;
xlabel (" x [ km ]") ;
ylabel (" y [ km ]") ;
zlabel (" z [ km ]") ;
grid on;