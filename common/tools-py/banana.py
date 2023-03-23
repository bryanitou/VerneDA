# System imports
import enum
import os
import sys

# Project libraries
import tools
import reader
import euler

# Import matplotlib stuff
import matplotlib.pyplot as plt

# Import numpy
import numpy as np


def get_usage():
    return "Usage:\n" \
           "python3 banana.py --file <string or os.PathLike> --plot_type <string> --metrics <string> --silent <bool:[" \
           "OPTIONAL]>\n" \
           "Example for translation plot:\n" \
           "->\t python3 banana.py --file example.dd --plot_type translation --metrics km --silent False\n" \
           "Example for attitude plot:\n" \
           "->\t python3 banana.py --file example.dd --plot_type orbit --metrics deg"


class PlotType(enum.Enum):
    translation = 0
    attitude = 1


def plot_banana(taylor: dict, output_prefix: [os.PathLike or str], plot_type: PlotType, metrics: str,
                verbose: bool = False) -> None:
    """
    Plots the taylor
    :param taylor: dictionary containing the coefficients and the order
    :param span: Span that the plot will cover
    :param output_prefix: where the plot should be saved.
    :param metrics: units to use: radians, degrees, meters, kilometers...
    :param plot_type: what should we plot? Attitude, translation...
    :param verbose: verbosity indicator
    :return: None
    """

    # Get vectors
    v = get_vector(taylor, idx=2, verbose=verbose)

    # Units
    unit_str = "-" if metrics == "" else metrics

    # Call to plots relying on PlotType class
    if plot_type == PlotType.attitude:
        plot_attitude(v[0], v[1], v[2], unit_str=unit_str, prefix=output_prefix)
    elif plot_type == PlotType.translation:
        plot_translation(v[0], v[1], v[2], unit_str=unit_str, prefix=output_prefix)
    else:
        print("I can only do Translation and Attitude plots!")
        exit(-1)


def get_vector(taylor: dict, idx: int, verbose: bool = False) -> [[float]]:
    # Safety check
    if not isinstance(taylor, dict):
        if verbose:
            print("Expected input 'taylor' is not a dictionary!")
            exit(-1)

    # Now, should collect X-Y coordinates
    # TODO: Make this work for quaternion AND for orbit
    idx = idx + 1
    v = [[] for i in range(idx)]
    var2retrieve = [str(i) for i in range(idx)]

    for delta in taylor:
        for var in taylor[delta]:
            if var in var2retrieve:
                # Get index and coef
                val = taylor[delta][var]["1"]["coef"]

                # Expanding this line
                v[int(var)].append(val)

    return v


def plot_attitude(x: [float], y: [float], z: [float], unit_str: str, prefix: os.PathLike or str) -> None:
    # Plot XY Projection
    plot_xy_projection(x, y, unit_str, output=f"{prefix}-XY_projection.png")

    # Plot projections in three planes: XY, YZ, XZ
    plot_projections(x, y, z, unit_str, output=f"{prefix}-3D_projections.png")

    # Plot 3D vectors
    plot_3d_vectors(x, y, z, output=f"{prefix}-3D_rotations.png")


def plot_translation(x: [float], y: [float], z: [float], unit_str: str, prefix: os.PathLike or str) -> None:
    # Plot XY Projection
    plot_xy_projection(x, y, unit_str, output=f"{prefix}-XY_projection.png")

    # Plot projections in three planes: XY, YZ, XZ
    plot_projections(x, y, z, unit_str, output=f"{prefix}-3D_projections.png")


def plot_xy_projection(x: [float], y: [float], unit_str: str, output: os.PathLike or str):
    # Set the size
    fig = plt.figure(figsize=(16, 9))

    # Finally, we can plot everything
    plt.scatter(x, y, s=0.4)
    plt.xlabel(f"x [{unit_str}]")
    plt.ylabel(f"y [{unit_str}]")

    # Write legend
    # plt.legend([f"{taylor['function']['name']}", "Taylor expansion"])

    # Plt show grid
    plt.grid(True)

    # Save fig
    plt.savefig(output)

    # Clear
    plt.clf()
    plt.cla()
    plt.close(fig)


def plot_quaternion(w: [float], x: [float], y: [float], z: [float], unit_str: str, output: os.PathLike or str):
    # Convert from quaternion to Euler angles
    [roll, pitch, yaw] = euler.euler_from_quaternion(x, y, z, w)

    # Print from there
    plot_projections(roll, pitch, yaw, unit_str, output)

def plot_projections(x: [float], y: [float], z: [float], unit_str: str, output: os.PathLike or str):
    # Initialise the subplot function using number of rows and columns
    fig, ax = plt.subplots(1, 3, figsize=(16, 9))

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

    # Save fig
    plt.savefig(output)

    # Clear
    plt.clf()
    plt.cla()
    plt.close(fig)


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

    # Save fig
    plt.show()
    plt.savefig(output)

    # Close stuff
    plt.clf()
    plt.cla()
    plt.close(fig)


def main(args: list = None, verbose: bool = False) -> None:
    """
    Main running function

    :param args: given arguments to the function
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

        # Re-set verbosity
        if "silent" in parsed_dict:
            verbose = False if str(parsed_dict["silent"]).lower() == "true" else True
        if "metrics" in parsed_dict:
            metrics = parsed_dict["metrics"]
        else:
            print("No <metrics> passed as argument!")
            print(get_usage())
            exit(-1)

        # Check type
        if "plot_type" in parsed_dict:
            plot_type = PlotType.attitude if parsed_dict["plot_type"].lower() == "attitude" else PlotType.translation
        else:
            print("No <plot_type> passed as argument!")
            print(get_usage())
            exit(-1)

        # Now, we should get the information from the file
        taylor_dict = reader.read_dd_file(parsed_dict["file"], verbose=verbose)

        # From the file, get the parent folder and save it
        parent_folder = os.path.abspath(os.path.dirname(parsed_dict["file"]))
        output_prefix = os.path.join(parent_folder, "attitude" if plot_type == PlotType.attitude else "translation")

        # Now, we should plot this Taylor polynomial, we have all the coefficients
        plot_banana(taylor_dict, output_prefix=output_prefix, plot_type=plot_type, metrics=metrics, verbose=verbose)


if __name__ == '__main__':
    # Call to main running function
    main(args=sys.argv[1:], verbose=True)
