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
% Noise stuff
% Set error of the sensor
stddev_sens = [0.1 / scaling_length, 0.1, 0.1];
betas_sens = stddev_sens;
sn_noise_DA = mex_get_DA_state(zeros(1, 3), betas_sens);


% Get DA vector
pv_state_DA = mex_get_DA_state(state, betas);
[az_stddev, el_stddev, range_stddev] = cart2sph(stddev(1), stddev(2), stddev(3));
[az_mu, el_mu, range_mu] = cart2sph(state_ini(1), state_ini(2), state_ini(3));
pred_meas_mu = [az_mu, el_mu, range_mu]; n_meas_mu = length(pred_meas_mu);
pred_meas_stddev = [az_stddev, el_stddev, range_stddev]; n_meas_stddev = length(pred_meas_stddev);
pred_state_stddev = stddev;

% Noise
ini_2nd_mom_noise = diag(stddev_sens .* stddev_sens);
nx_2nd_mom_noise = ini_2nd_mom_noise;

% Predict: spherical coordinates
ini_1st_mom_meas_pred = pred_meas_mu; % Adimensional, central
ini_2nd_mom_meas_pred = diag(pred_meas_stddev .* pred_meas_stddev ); % Adimensional, central
ini_3rd_mom_meas_pred = zeros(n_meas_stddev * n_meas_stddev, n_meas_stddev); % Adimensional, central
ini_4th_mom_meas_pred = get_4th_mom(pred_meas_stddev); % Adimensional, central
nx_1st_mom_meas_pred = ini_1st_mom_meas_pred; % % Adimensional, central
nx_2nd_mom_meas_pred = ini_2nd_mom_meas_pred; % % Adimensional, central
nx_3rd_mom_meas_pred = ini_3rd_mom_meas_pred; % % Adimensional, central
nx_4th_mom_meas_pred = ini_4th_mom_meas_pred; % % Adimensional, central

% Predict dy = predict - E{predict} + error_noise
ini_2nd_mom_meas_pred_dy = ini_2nd_mom_noise;
ini_3rd_mom_meas_pred_dy = zeros(n_meas_mu * n_meas_mu, n_meas_mu);
ini_4th_mom_meas_pred_dy = get_4th_mom(stddev_sens);
nx_2nd_mom_meas_pred_dy = ini_2nd_mom_noise;
nx_3rd_mom_meas_pred_dy = ini_3rd_mom_meas_pred_dy;
nx_4th_mom_meas_pred_dy = ini_4th_mom_meas_pred_dy;

% State: cartesian coordinates
nx_2nd_mom_state_pred = diag(pred_state_stddev .* pred_state_stddev );

% Joint second and third moments


while 1
    % Get the next state from the previous state
    nx_state_DA = mex_propagate_DA_state(pv_state_DA, betas, time, nli);
    
    % samples = mex_propagate_DA_and_get_samples(state_DA, stddev, betas, time, nli, n_samples);
    meas_predict = mex_get_DA_predict_spherical(nx_state_DA);

    % Get new 1st moment of the predict
    nx_1st_mom_meas_pred = mex_get_DA_first_moments(nx_1st_mom_meas_pred, meas_predict, sn_noise_DA);
    meas_predict_dy = mex_get_DA_err_variation(nx_1st_mom_meas_pred, meas_predict, sn_noise_DA);

    % Get the second moments of the predict
    nx_2nd_mom_state_pred = mex_get_DA_second_moments(nx_2nd_mom_state_pred, nx_state_DA);
    nx_2nd_mom_meas_pred = mex_get_DA_second_moments(nx_2nd_mom_meas_pred, meas_predict);
    nx_2nd_mom_meas_pred_dy = mex_get_DA_second_moments(nx_2nd_mom_meas_pred_dy, meas_predict_dy);
    nx_3rd_mom_meas_pred_dy = mex_get_DA_third_moments(nx_3rd_mom_meas_pred_dy, meas_predict_dy);
    nx_4th_mom_meas_pred_dy = mex_get_DA_fourth_moments(nx_4th_mom_meas_pred_dy, meas_predict_dy);

    % Get the third moments of the predict
    % nx_3rd_mom_meas_pred = 

    % Kalman GAIN
    aug_meas_pred_dy = mex_get_DA_aug_meas_dev2(meas_predict_dy, ...
                    nx_2nd_mom_meas_pred_dy, ...
                    nx_3rd_mom_meas_pred_dy, ...
                    nx_4th_mom_meas_pred_dy);
    
    % Get the nearest inverse
    psd_aug_meas_pred_dy = nearestPSD(aug_meas_pred_dy);
    inv_aug_meas_pred_dy = inv(psd_aug_meas_pred_dy);

    % Get experiment
    simulator;

    % Acquired position
    state_sens = [mission.Satellite.TimeseriesPosECEF.Data(end, :)',
        mission.Satellite.TimeseriesVelECEF.Data(end, :)'];
    state_sens = state_sens / 1000; % Pass to km and km/s
    state_sens_adimensional = [state_sens(1:3, :)' / scaling_length, state_sens(4:6, :)' / scaling_velocity];
    [az_stddev, el_stddev, range_stddev] = cart2sph(state_sens_adimensional(1),state_sens_adimensional(2),state_sens_adimensional(3));
    state_sens_adimensional_spherical = [range_stddev, az_stddev, el_stddev];

    % Standard deviaiton of the sensor: 0.1 m in range and 0.1 arcsec for
    % angles
    stddev_sens = [0.1 / scaling_length, 0.1, 0.1];
    % state_sens_noise = [state_sens(1:3) + randn(3, 1000)*1.5, % km
    %                     state_sens(4:6) + randn(3, 1000)*1.5];% km/sº
    meas_update = mex_get_DA_meas_update(meas_predict, sn_noise_DA, nx_2nd_mom_meas_pred)';
    meas_err_sens = mex_get_DA_err_variation(meas_predict, state_sens_adimensional_spherical, sn_noise_DA);
    meas_err_upd = mex_get_DA_err_variation(meas_predict, meas_update, sn_noise_DA);
    
    % Kalman gain
    mex_get_DA_aug_meas_dev(meas_predict, sn_noise_DA, nx_2nd_mom_state_pred,nx_1st_mom_meas_pred, nx_2nd_mom_meas_pred, nx_3rd_mom_meas_pred, nx_4th_mom_meas_pred)

    % Filter goes here
    state_sens_DA = mex_get_DA_state(state_sens_adimensional, state_sens_noise);
    
    % Filter
    mex_filter_DA(pv_state_DA, nx_state_DA, state_sens_DA, i);
    
    % Plot to see what's happening
    close all;
    plot_orbits;
    plot_noise;

    pause(2);

    % Increase iteration counter
    i = i + 1;
end


function r4th = get_4th_mom(stddev)
    len_stddev = length(stddev);
    r4th = zeros(len_stddev*len_stddev, len_stddev*len_stddev);

    for i = 1:len_stddev
        for j = 1:len_stddev
            for k = 1:len_stddev
                for l = 1:len_stddev
                    if i == j && j == k && k == l
                        r4th(len_stddev * (i - 1) + j, len_stddev * (k - 1) + l) = 3 * stddev(i) ^ 4;
                    elseif  i == j && k == l
                        r4th(len_stddev * (i - 1) + j, len_stddev * (k - 1) + l) = stddev(i) ^ 2 * stddev(k) ^ 2;
                    end
                end
            end
        end
    end
end