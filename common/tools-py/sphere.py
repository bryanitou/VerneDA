#!env/python
import reader
import banana

# Import histogram stuff
import quaternion_histogram


def main():
    # File to assess
    filepath = "/home/bryan/CLionProjects/ISAE/research_project/getting2knowDace/out/attp/non_eval_deltas_expression.dd"
    output = filepath.replace(".dd", "-Sphere.png")

    # Read and get results
    dd_dict = reader.read_dd_file(filepath, verbose=True)

    # Now, call to plot
    v = banana.get_vector(dd_dict, idx=3, verbose=True)

    # Fix quaternion
    # TODO: find a proper definition for this step
    v[0] = [w + 1 for w in v[0]]

    # Use quaternion plotter
    banana.plot_quaternion(v[0], v[1], v[2], v[3], unit_str="-", output=output)

    # Plot histogram
    quaternion_histogram.quaternion_histogram(v[0], v[1], v[2], v[3], output=output)


if __name__ == '__main__':
    main()
