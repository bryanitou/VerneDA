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

% Iteration
i = int32(0);

%
%% Initialization
% Get DA vector
pv_state_DA = mex_get_DA_state(state, betas);
nx_2nd_mom = stddev .* stddev + state_ini .* state_ini; % Adimensional

% Set error of the sensor
stddev_sens = [100 / scaling_length, 0.1, 0.1];
betas_sens = stddev_sens;
sn_state_DA_noise = mex_get_DA_state(zeros(1, 3), betas_sens);

while 1
    % Get the next state from the previous state
    nx_state_DA = mex_propagate_DA_state(pv_state_DA, betas, time, nli);
    
    % samples = mex_propagate_DA_and_get_samples(state_DA, stddev, betas, time, nli, n_samples);
    predict = mex_get_DA_predict_spherical(nx_state_DA);

    % Extracting samples for viz
    % pos_samples = samples(1:3, :) * scaling_length;
    % vel_samples = samples(4:6, :) * scaling_velocity;
    
    % % Set next state
    % state = [samples(1:3, end)',samples(4:6, end)'];
    % state_dim = [state(1:3) * scaling_length, state(4:6)*scaling_velocity];

    % Get experiment
    simulator;

    % Acquired position
    state_sens = [mission.Satellite.TimeseriesPosECEF.Data(end, :)',
        mission.Satellite.TimeseriesVelECEF.Data(end, :)'];
    state_sens = state_sens / 1000; % Pass to km and km/s
    state_sens_adimensional = [state_sens(1:3, :)' / scaling_length, state_sens(4:6, :)' / scaling_velocity];
    [az, el, range] = cart2sph(state_sens_adimensional(1),state_sens_adimensional(2),state_sens_adimensional(3));
    state_sens_adimensional_spherical = [range, az, el];

    % Standard deviaiton of the sensor: 0.1 m in range and 0.1 arcsec for
    % angles
    stddev_sens = [0.1 / scaling_length, 0.1, 0.1];
    % state_sens_noise = [state_sens(1:3) + randn(3, 1000)*1.5, % km
    %                     state_sens(4:6) + randn(3, 1000)*1.5];% km/sº
    meas_update = mex_get_DA_meas_update(predict, sn_state_DA_noise, nx_2nd_mom(1:3))';
    meas_err_sens = mex_get_DA_err_variation(predict, state_sens_adimensional_spherical, sn_state_DA_noise);
    meas_err_upd = mex_get_DA_err_variation(predict, meas_update, sn_state_DA_noise);
    
    % Kalman gain
    mex_get_DA_aug_meas_dev(predict, sn_state_DA_noise, nx_2nd_mom(1:3))

    % Filter goes here
    state_sens_DA = mex_get_DA_state(state_sens_adimensional, state_sens_noise);
    
    % Filter
    mex_filter_DA(pv_state_DA, nx_state_DA, state_sens_DA, i);
    
    % Plot to see what's happening
    close all;
    plot_orbits;
    plot_noise;

    pause(2);

    % Get the second moments of the predicted state
    nx_2nd_mom = mex_get_DA_second_moments(nx_2nd_mom, nx_state_DA)';

    % Increase iteration counter
    i = i + 1;
end
