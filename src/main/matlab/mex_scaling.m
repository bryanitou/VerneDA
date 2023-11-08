%%%%%%%%%%%%%%%%%%%
%% Scaling %%%%%%%%
%%%%%%%%%%%%%%%%%%%
% Scaling paramters are those one that makes mu = 1 km ^ 3 / s ^ 2, and it
% is computed directly from the orbit period
mu = 3.986004418000000e+05;
scaling_time = (orbit_period_sec * orbit_period_sec / (4 * pi * pi)) ^ (1 / 2);
scaling_length = (orbit_period_sec * orbit_period_sec * mu / (4 * pi * pi)) ^ (1 / 3);
scaling_velocity = scaling_length / scaling_time;