% Set initial state
state_ini = [0.5, 0.0, 0.0, 0.0,1.7320508075688774, 0.0];

% Test MEX executable
b = mex_vsod(state_ini)