#!env/python
import matplotlib.pyplot

import reader
import banana

# Import histogram stuff
import quaternion_histogram


def main():
    # File to assess
    filepath = "./out/example/attitude_test/non_eval_deltas_expression.dd"
    output = filepath.replace(".dd", "-Sphere.pdf")

    # Read and get results
    dd_dict = reader.read_dd_file(filepath, verbose=True)

    # Now, call to plot
    v = banana.get_vector(dd_dict, idx=3, verbose=True)

    # Fix quaternion
    # TODO: find a proper definition for this step
    v[0] = [w + 1 for w in v[0]]

    # Plot 3D scatter
    # banana.plot_3d_vectors(v[0], v[1], v[2], unit_str="-", output=output)
    # banana.plot_3d_scatter(v[0], v[1], v[2], unit_str="rad", output=output)

    # Use quaternion plotter
    banana.plot_quaternion(v[0], v[1], v[2], v[3], unit_str="-", output=output)

    # Plot histogram
    quaternion_histogram.quaternion_histogram(v[0], v[1], v[2], v[3], output=output)


if __name__ == '__main__':
    main()
