% Create plots
figure;
t = tiledlayout(6,1);
x = 1:length(state_sens_noise(1,:));

for i = 1:length(state_sens_noise(:,1))
    nexttile;
    plot(x, state_sens_noise(i,:));
    grid on;
end

