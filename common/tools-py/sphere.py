import reader
import banana


def main():
    # File to assess
    filepath = "/home/bryan/CLionProjects/ISAE/research_project/getting2knowDace/out/attp/non_eval_deltas_expression.dd"
    output = filepath.replace(".dd", "-Sphere.png")
    # Read and get results
    dd_dict = reader.read_dd_file(filepath, verbose=True)

    # Now, call to plot
    v = banana.get_vector(dd_dict, idx=3, verbose=True)

    # Use quaternion plotter
    banana.plot_quaternion(v[0], v[1], v[2], v[3], unit_str="-", output=output)


if __name__ == '__main__':
    main()
