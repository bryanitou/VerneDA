% Clear everything
% bdclose all; % clear all libraries out of memory ( supposedly )
% clear all;   % clear all workspace variables, mex, etc. ( supposedly )
% close all;   % Close all windows
% rehash;      % cause all .m files to be reparsed when invoked again
% Set environment
pause on;
env;

% Orbit Period
orbit_period_sec = 15361.68181049502; % seconds

% Get scaling parameters
mex_scaling;

% Set the initial state, standard deviation and CI
state_ini_dim = [6.6781*1.0e+03 , 0.0, 0.0, 0, 9.4621, 0.0164];
state_ini = [state_ini_dim(1:3) / scaling_length, state_ini_dim(4:6) / scaling_velocity];
stddev_dim = [10.0, 100.0, 0.0, 0.0, 0.0, 0.0437];
stddev = [stddev_dim(1:3) / scaling_length, stddev_dim(4:6) / scaling_velocity];
ci = 3;
betas = ci * stddev;

% Propagation time
t0 = 0.0;
tf = orbit_period_sec*0.2;
dt = 10;
time = [t0, tf, dt] / scaling_time;

% NLI
nli = 0.02;

% Get samples after propagation
n_samples = 10000;


% Set initial state
state_dim = state_ini_dim;
state = state_ini;

% Sensors
state_sens = [0];

while 1
    % Get DA vector
    state_DA = mex_get_DA_state(state, betas);
    % fprintf("%s", string(state_DA_ini))

    nx_state_DA = mex_propagate_DA_state(state_DA, betas, time, nli);
    samples = mex_propagate_DA_and_get_samples(state_DA, stddev, betas, time, nli, n_samples);
    
    % Filter goes here
    % TODO: Workaraound
    pos_samples = samples(1:3, :) * scaling_length;
    vel_samples = samples(4:6, :) * scaling_velocity;
    
    % Set next state
    state = [samples(1:3, end)',samples(4:6, end)'];
    state_dim = [state(1:3) * scaling_length, state(4:6)*scaling_velocity];

    % Get experiment
    simulator;

    % Acquired position
    state_sens = [mission.Satellite.TimeseriesPosECEF.Data(end, :)',
        mission.Satellite.TimeseriesVelECEF.Data(end, :)'];
    state_sens = state_sens / 1000; % Pass to km and km/s

    % Add some noise
    state_sens_noise = [state_sens(1:3) + randn(1, 1000)*1.5, % km
                        state_sens(4:6) + randn(1, 1000)*1.5];% km/s

    % Plot to see what's happening
    close all;
    plot_orbits;
    plot_noise;

    pause(2);
end
