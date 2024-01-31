% Plot the result
figure;
plot3(pos_samples(1,:), pos_samples(2,:), pos_samples(3,:), '.');
hold on;
plot3(1e-3 * mission.Satellite.TimeseriesPosECEF.Data(:, 1), ...
    1e-3 *mission.Satellite.TimeseriesPosECEF.Data(:, 2), ...
    1e-3 *mission.Satellite.TimeseriesPosECEF.Data(:, 3), 'o');
hold on;

alpha(.5)

r_earth = 6378;
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

view(2)