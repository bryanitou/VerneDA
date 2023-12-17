function [stddev_out] = stddev_ompl(stddev_in)
    stddev_out = zeros(1, length(stddev_in));
    for i = 1:length(stddev_in)
        stddev_out(i) = 2 * stddev_in(i) / sqrt(3);
    end
end