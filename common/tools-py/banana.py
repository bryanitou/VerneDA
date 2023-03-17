# System imports
import math
import os
import sys

# Project libraries
import tools
import reader
import euler

# Import matplotlib stuff
import matplotlib.pyplot as plt

# 3d printer
from mpl_toolkits.mplot3d import Axes3D

# Import numpy
import numpy as np


def plot_banana(taylor: dict, output_path: [os.PathLike or str], metrics: str, verbose: bool = False) -> None:
    """
    Plots the taylor
    :param taylor: dictionary containing the coefficients and the order
    :param span: Span that the plot will cover
    :param output_path: where the plot should be saved.
    :param metrics: what should we plot? Attitude, translation...
    :param verbose: verbosity indicator
    :return: None
    """

    # Safety check
    if not isinstance(taylor, dict):
        if verbose:
            print("Expected input 'taylor' is not a dictionary!")
            exit(-1)

    # Units
    unit_str = "-" if metrics == "" else metrics

    # Now, should collect X-Y coordinates
    # TODO: Make this work for quaternion AND for orbit
    x = []
    y = []
    z = []
    for delta in taylor:
        for var in taylor[delta]:
            if var == "0" or var == "1" or var == "2":
                # Get index and coef
                val = taylor[delta][var]["1"]["coef"]

                # Expanding this line
                if var == "0":
                    x.append(val)
                elif var == "1":
                    y.append(val)
                else:
                    z.append(val)

    if len(z) > 0:
        # Initialise the subplot function using number of rows and columns
        figure, ax = plt.subplots(1, 3, figsize=(16, 9))

        # Creating plot
        ax[0].scatter(x, y, color="green", s=0.4)
        ax[1].scatter(y, z, color="green", s=0.4)
        ax[2].scatter(x, z, color="green", s=0.4)

        # Labels
        ax[0].set_xlabel(f"x [{unit_str}]")
        ax[0].set_ylabel(f"y [{unit_str}]")
        ax[1].set_xlabel(f"y [{unit_str}]")
        ax[1].set_ylabel(f"z [{unit_str}]")
        ax[2].set_xlabel(f"x [{unit_str}]")
        ax[2].set_ylabel(f"z [{unit_str}]")

        # Titles
        ax[0].set_title("XY Projection")
        ax[1].set_title("YZ Projection")
        ax[2].set_title("XZ Projection")

        # Tilt labels
        [ticks.set_rotation(45) for ticks in ax[0].get_xticklabels()]
        [ticks.set_rotation(45) for ticks in ax[1].get_xticklabels()]
        [ticks.set_rotation(45) for ticks in ax[2].get_xticklabels()]

        # Don't show y label for middle plot but maintain the grids Refer to:
        # https://stackoverflow.com/questions/20416609/remove-the-x-axis-ticks-while-keeping-the-grids-matplotlib, for
        # the code explanation
        for tick in ax[1].yaxis.get_major_ticks():
            tick.tick1line.set_visible(False)
            tick.tick2line.set_visible(False)
            tick.label1.set_visible(False)
            tick.label2.set_visible(False)

        # Set y-axis labels of the right plot to the right
        ax[2].yaxis.tick_right()

        # Enable grid
        ax[0].grid(True)
        ax[1].grid(True)
        ax[2].grid(True)

        # Set subtitle
        plt.suptitle("Final position distribution")

        # Save plot
        plt.savefig(output_path[0])

        # Clear
        plt.close(figure)

    # Set the size
    plt.figure(figsize=(16, 9))

    # Finally, we can plot everything
    plt.scatter(x, y, s=0.4)
    plt.xlabel(f"x [{unit_str}]")
    plt.ylabel(f"y [{unit_str}]")

    # Write legend
    # plt.legend([f"{taylor['function']['name']}", "Taylor expansion"])

    # Plt show grid
    plt.grid(True)

    # Save second plot
    plt.savefig(output_path[1])

    # Plot 3D vectors
    plot_3d_vectors(x, y, z, output=output_path[2])


def plot_3d_vectors(roll: [float], pitch: [float], yaw: [float], output: str):
    # The initial orientation was: [x: 0, y: 0, z: 1]
    initial_vector_x = np.array([1, 0, 0]).transpose()  # Initial attitude
    initial_vector_y = np.array([0, 1, 0]).transpose()  # Initial attitude
    initial_vector_z = np.array([0, 0, 1]).transpose()  # Initial attitude

    # Final vectors
    final_attitude_x = []
    final_attitude_y = []
    final_attitude_z = []

    # Get size
    size = len(roll)

    for i in range(0, size):
        # Get rotation matrix
        rot_matrix = euler.rotation_matrix(roll[i], pitch[i], yaw[i], order="zyx")

        # Final vector
        final_vector_x = np.matmul(rot_matrix, initial_vector_x)
        final_vector_y = np.matmul(rot_matrix, initial_vector_y)
        final_vector_z = np.matmul(rot_matrix, initial_vector_z)

        # Append result
        final_attitude_x.append(final_vector_x)
        final_attitude_y.append(final_vector_y)
        final_attitude_z.append(final_vector_z)

    # Triplets of vectors
    xx, yx, zx = zip(*final_attitude_x)
    xy, yy, zy = zip(*final_attitude_y)
    xz, yz, zz = zip(*final_attitude_z)

    # Figure and its size
    fig = plt.figure(figsize=(16, 9))

    # Add subplot
    ax = fig.add_subplot(111, projection='3d')

    # From the center, print vectors.
    ax.quiver([0] * size, [0] * size, [0] * size, xx, yx, zx, color="r")
    ax.quiver([0] * size, [0] * size, [0] * size, xy, yy, zy, color="b")
    ax.quiver([0] * size, [0] * size, [0] * size, xz, yz, zz, color="y")
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])
    plt.show()
    plt.savefig(output)


def main(args: list = None, span: int = 1, verbose: bool = False) -> None:
    """
    Main running function

    :param args: given arguments to the function
    :param span: span that the plot will cover
    :param verbose: verbosity boolean
    :return:
    """

    # Safety check the given arguments
    if args is None:
        if verbose:
            print("ERROR: No arguments given!")
        exit(-1)
    else:
        # Parse given arguments
        parsed_dict = tools.parse_arguments(args, verbose=verbose)

        # Metrics of the plot
        metrics = ""

        # Re-set verbosity
        if "silent" in parsed_dict:
            verbose = False if str(parsed_dict["silent"]).lower() == "true" else True
        if "span" in parsed_dict:
            span = int(parsed_dict["span"])
        if "metrics" in parsed_dict:
            metrics = parsed_dict["metrics"]

        # Now, we should get the information from the file
        taylor_dict = reader.read_dd_file(parsed_dict["file"], verbose=verbose)

        # From the file, get the parent folder and save it
        parent_folder = os.path.abspath(os.path.dirname(parsed_dict["file"]))
        txt_filename1 = os.path.split(parsed_dict['file'])[1].replace('.dd', '_1.png')
        txt_filename2 = os.path.split(parsed_dict['file'])[1].replace('.dd', '_2.png')
        txt_filename3 = os.path.split(parsed_dict['file'])[1].replace('.dd', '_3d.png')
        output_path1 = os.path.join(parent_folder, f"{txt_filename1}")
        output_path2 = os.path.join(parent_folder, f"{txt_filename2}")
        output_path3 = os.path.join(parent_folder, f"{txt_filename3}")

        # Now, we should plot this Taylor polynomial, we have all the coefficients
        plot_banana(taylor_dict, output_path=[output_path1, output_path2, output_path3], metrics=metrics, verbose=verbose)


if __name__ == '__main__':
    # Call to main running function
    main(args=sys.argv[1:], verbose=True)
