% Set initial state
state_ini = [0.5, 0.0, 0.0, 0.0,1.7320508075688774, 0.0];


% Probabilistic distribution
stddev = [7.487120281336031E-4, 0.007487120281336032, 0.0, 0.0, 0.0, 0.0];

ci = 3;

betas = stddev * ci;

% Test MEX executable
b = mex_vsod(state_ini, betas)