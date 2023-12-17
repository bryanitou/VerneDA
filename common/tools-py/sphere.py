#!env/python
import os
import sys

import reader
import banana

# Import histogram stuff
import quaternion_histogram

# Import functions from other scripts
from tools import parse_arguments


def main(args: [] or None):

    # Parse arguments
    args_dict = parse_arguments(args=args, verbose=True)

    # Get path
    type_of_rotation = args_dict["type"]
    fpath = args_dict[""]
    # File to assess
    output = fpath.replace(".dd", "-Sphere.pdf")

    # Read and get results
    dd_dict = reader.read_dd_file(fpath, verbose=True)

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
    main(sys.argv[1:] if len(sys.argv) > 1 else None)
