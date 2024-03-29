#!env/python
# System imports
import enum
import math
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


def plot_banana(deltas_dict: dict,
                output_prefix: [os.PathLike or str],
                plot_type: PlotType,
                metrics: str,
                centers_dict: dict = None,
                walls_dict: dict = None,
                output_format: str = "pdf",
                legend_fixed: bool = False,
                axis_fixed: bool = False,
                verbose: bool = False) -> None:
    """
    Plots the taylor
    :param deltas_dict: dictionary containing the coefficients and the order
    :param span: Span that the plot will cover
    :param output_prefix: where the plot should be saved.
    :param metrics: units to use: radians, degrees, meters, kilometers...
    :param plot_type: what should we plot? Attitude, translation...
    :param centers_dict: centers dictionary
    :param walls_dict: walls dictionary
    :param output_format: output format
    :param legend_fixed: legend fixed or not (best location)
    :param verbose: verbosity indicator
    :param axis_fixed: true or false (axis fixed set them to 1 and -1)
    :return: None
    """

    # Build x, y dictionary
    x = {}
    y = {}
    v_deltas = None
    # Get vectors
    if deltas_dict is not None:
        v_deltas = get_vector(deltas_dict, idx=2, verbose=verbose)

        x["deltas"] = v_deltas[0]
        y["deltas"] = v_deltas[1]

    if centers_dict is not None:
        v_centers = get_vector(centers_dict, idx=2, verbose=verbose)

        # Add centers
        x["centers"] = v_centers[0]
        y["centers"] = v_centers[1]

    if walls_dict is not None:
        v_patches = [[], []]

        for p in walls_dict:
            v_walls = get_vector(walls_dict[p], idx=2, verbose=verbose)
            v_patches[0].append(v_walls[0])
            v_patches[1].append(v_walls[1])

        # Add walls
        x["walls"] = v_patches[0]
        y["walls"] = v_patches[1]

    # Units
    unit_str = "-" if metrics == "" else metrics

    # Call to plots relying on PlotType class
    if plot_type == PlotType.attitude:
        plot_attitude(x, y, v_deltas[2], unit_str=unit_str, prefix=output_prefix, output_format=output_format, legend_fixed=legend_fixed, axis_fixed=axis_fixed)
    elif plot_type == PlotType.translation:
        plot_translation(x, y, None, unit_str=unit_str, prefix=output_prefix, output_format=output_format, legend_fixed=legend_fixed, axis_fixed=axis_fixed)
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


def plot_attitude(x: dict, y: dict, z: [float], unit_str: str, prefix: os.PathLike or str, output_format: str = "pdf", legend_fixed: bool = False, axis_fixed: bool = False) -> None:
    # Plot XY Projection
    plot_xy_projection(x, y, unit_str,  output=f"{prefix}-XY_projection.{output_format}", output_format=output_format, legend_fixed=legend_fixed, axis_fixed=axis_fixed)

    # Plot projections in three planes: XY, YZ, XZ
    plot_projections(x["deltas"], y["deltas"], z, unit_str, output=f"{prefix}-3D_projections.pdf")

    # Plot 3D vectors
    plot_3d_vectors(x["deltas"], y["deltas"], z, unit_str, output=f"{prefix}-3D_rotations.pdf")


def plot_translation(x: dict, y: dict, z: [float], unit_str: str, prefix: os.PathLike or str, output_format: str = "pdf", legend_fixed: bool = False, axis_fixed: bool = False) -> None:
    # Plot XY Projection
    plot_xy_projection(x, y, unit_str, output=f"{prefix}-XY_projection.{output_format}", output_format=output_format, legend_fixed=legend_fixed, axis_fixed=axis_fixed)

    # Plot projections in three planes: XY, YZ, XZ
    if z is not None:
        plot_projections(x["deltas"], y["deltas"], z, unit_str, output=f"{prefix}-3D_projections.pdf")


def plot_xy_projection(x: dict, y: dict, unit_str: str, output: os.PathLike or str, output_format: str = "pdf", legend_fixed: bool = False, axis_fixed: bool = False) -> None:
    # Auxiliary variable
    legend_aux = []

    # Set the size
    fig = plt.figure(figsize=(16, 9))
    # plt.show()
    # plt.ion()
    # plt.ylim([-1.25, 1.25])
    # plt.xlim([-1.5, 1.5])
    # # Finally, we can plot everything
    if "deltas" in x and "deltas" in y:
        plt.scatter(x["deltas"], y["deltas"], s=10, marker="x", color="grey", linewidths=0.5)
        # Add to legend
        legend_aux.append("MC Samples")
    if "centers" in x and "centers" in y:
        plt.scatter(x["centers"], y["centers"], s=5, marker="o", color="orange")
        # Add to legend
        legend_aux.append("Patch centers")
    if "walls" in x and "walls" in y:
        # Iterate through every wall
        # patches_x = np.array(x["walls"]).ravel()
        # patches_y = np.array(y["walls"]).ravel()
        for p in range(0, len(x["walls"])):
            patch_x = x["walls"][p]
            patch_y = y["walls"][p]
            plt.plot(patch_x, patch_y, linewidth=1, color="black")
            # patch_x = x["walls"][p][:8]
            # patch_y = y["walls"][p][:8]
            # for it in range(0, len(patch_x) - 1):
            #     plt.plot(patch_x[it:it+2], patch_y[it:it+2], linewidth=0.2, color="black")

        # Add to legend
        legend_aux.append("Patch bounds")

    # Labels
    plt.xlabel(f"{'roll' if unit_str in ['rad', 'deg'] else 'x'}  [{unit_str}]")
    plt.ylabel(f"{'pitch' if unit_str in ['rad', 'deg'] else 'y'}  [{unit_str}]")

    # Axis fixed?
    if axis_fixed:
        plt.xlim([-1.1, 1.1])
        plt.ylim([-1.1, 1.1])

    # Write legend
    plt.legend(legend_aux, loc=f"upper right" if legend_fixed else "best")

    # Plt show grid
    plt.grid(True)

    # plt.show()
    # Save fig
    plt.savefig(output, format=f"{output_format}", bbox_inches="tight")

    # Clear
    plt.clf()
    plt.cla()
    plt.close(fig)


def plot_quaternion(w: [float], x: [float], y: [float], z: [float], unit_str: str, output: os.PathLike or str):
    # Convert from quaternion to Euler angles
    roll, pitch, yaw = [], [], []

    for i in range(len(w) - 1):
        [roll_val, pitch_val, yaw_val] = euler.euler_from_quaternion(x[i], y[i], z[i], w[i])
        roll.append(roll_val), pitch.append(pitch_val), yaw.append(yaw_val)

    # Print from there
    plot_3d_vectors(roll, pitch, yaw, unit_str, output)


def plot_projections(x: [float], y: [float], z: [float], unit_str: str, output: os.PathLike or str) -> object:
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
    plt.savefig(output, format="pdf", bbox_inches="tight")

    # Clear
    plt.clf()
    plt.cla()
    plt.close(fig)


def rotate(roll, pitch, yaw, unit_str):
    # The initial orientation was: [x: 0, y: 0, z: 1]
    initial_vector_x = np.array([1, 0, 0]).transpose()  # Initial attitude
    initial_vector_y = np.array([0, 1, 0]).transpose()  # Initial attitude
    initial_vector_z = np.array([0, 0, 1]).transpose()  # Initial attitude

    # Final vectors
    final_attitude_x = []
    final_attitude_y = []
    final_attitude_z = []

    for i in range(0, len(roll)):
        # Get rotation matrix
        rot_matrix = euler.rotation_matrix(roll[i], pitch[i], yaw[i], order="xyz", unit=unit_str)

        # Get rotation angles
        # rot_angles = euler.rotation_angles(rot_matrix, order="zyx")
        # Test zone
        # rot_matrix_rot1 = euler.rotation_matrix(0.0, 0.0, math.pi, order="xyz", unit=unit_str)
        # rot_matrix_rot2 = euler.rotation_matrix(0.0, math.pi, 0.0, order="xyz", unit=unit_str)
        # rot_matrix_rot3 = euler.rotation_matrix(math.pi, 0.0,0.0, order="xyz", unit=unit_str)
#
        # final_vector_x1 = np.matmul(rot_matrix_rot1, initial_vector_x)
        # final_vector_x2 = np.matmul(rot_matrix_rot2, initial_vector_x)
        # final_vector_x3 = np.matmul(rot_matrix_rot3, initial_vector_x)

        # Final vector
        final_vector_x = np.matmul(rot_matrix, initial_vector_x)  # Rotated vector: x = [1, 0, 0]' (3x1)
        final_vector_y = np.matmul(rot_matrix, initial_vector_y)  # Rotated vector: y = [0, 1, 0]' (3x1)
        final_vector_z = np.matmul(rot_matrix, initial_vector_z)  # Rotated vector: z = [0, 0, 1]' (3x1)

        # Append result
        final_attitude_x.append(final_vector_x)
        final_attitude_y.append(final_vector_y)
        final_attitude_z.append(final_vector_z)

    return final_attitude_x, final_attitude_y, final_attitude_z


def plot_3d_vectors(roll: [float], pitch: [float], yaw: [float], unit_str: str, output: str):
    # Make rotation
    [f_att_x, f_att_y, f_att_z] = rotate(roll, pitch, yaw, unit_str=unit_str)

    # Triplets of vectors
    xx, yx, zx = zip(*f_att_x)
    xy, yy, zy = zip(*f_att_y)
    xz, yz, zz = zip(*f_att_z)

    # Figure and its size
    fig = plt.figure(figsize=(16, 9))

    # Add subplot
    ax = fig.add_subplot(111, projection='3d')

    # From the center, print vectors.
    ax.quiver([0] * len(roll), [0] * len(roll), [0] * len(roll), xx, yx, zx, color="r")
    ax.quiver([0] * len(roll), [0] * len(roll), [0] * len(roll), xy, yy, zy, color="b")
    ax.quiver([0] * len(roll), [0] * len(roll), [0] * len(roll), xz, yz, zz, color="y")
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])

    # Save fig
    plt.savefig(output, format="pdf", bbox_inches="tight")

    # Close stuff
    plt.clf()
    plt.cla()
    plt.close(fig)

    # Plot also scatter if needed
    plot_3d_scatter(xx, yx, zx, unit_str=unit_str, output=output)


def plot_3d_scatter(x: [float], y: [float], z: [float], unit_str: str, output: os.PathLike or str):
    # Set the size
    fig = plt.figure(figsize=(16, 9))
    ax = fig.add_subplot(projection='3d')

    # Plot the sphere
    u, v = np.mgrid[0:2 * np.pi:30j, 0:np.pi:20j]
    xs = np.cos(u) * np.sin(v)
    ys = np.sin(u) * np.sin(v)
    zs = np.cos(v)

    # Plot surface
    ax.plot_surface(xs, ys, zs, cmap=plt.cm.YlGnBu_r, alpha=.4)

    # Do scatter
    ax.scatter(x, y, z, marker='x', c='r', linewidths=0.2)

    # Set labels
    ax.set_xlabel(f"x [{unit_str}]")
    ax.set_ylabel(f"y [{unit_str}]")
    ax.set_zlabel(f"z [{unit_str}]")

    # Set axes limits
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])

    # Change gimbal
    ax.view_init(10, -10)
    # plt.show()

    # Save plot
    plt.savefig(output.replace(".pdf", "-scatter.pdf"), format="pdf", bbox_inches="tight")

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

        # Default format
        output_format = "pdf"

        # Check format to be plot
        if "output_format" in parsed_dict:
            output_format = parsed_dict["output_format"]

        # Default legend
        legend_fixed = False

        # Legend details
        if "legend_fixed" in parsed_dict:
            legend_fixed = str(parsed_dict["legend_fixed"]).lower() == "true"

        # Axis bounds
        axis_fixed = False

        # Legend details
        if "axis_fixed" in parsed_dict:
            axis_fixed = str(parsed_dict["axis_fixed"]).lower() == "true"

        # Now, we should get the information from the file
        eval_deltas_dict = reader.read_dd_file(parsed_dict["file"], verbose=verbose) if "file" in parsed_dict else None
        eval_centers_dict = reader.read_dd_file(parsed_dict["centers"], verbose=verbose) if "centers" in parsed_dict else None
        eval_walls_dict = reader.read_dd_file(parsed_dict["walls"], verbose=verbose, walls=True) if "walls" in parsed_dict else None

        # From the file, get the parent folder and save it
        if "output_prefix" in parsed_dict:
            output_prefix = parsed_dict["output_prefix"]
        else:
            print("No <output_dir> passed as argument!")
            print(get_usage())
            exit(-1)

        # If it is not a directory, add hyphen
        if not os.path.isdir(output_prefix):
            output_prefix += "-"

        # Add whether attitude of translation
        output_prefix += "attitude" if plot_type == PlotType.attitude else "translation"

        # Build prefix relying on passed information
        output_prefix += "-walls" if eval_walls_dict is not None else ""
        output_prefix += "-centers" if eval_centers_dict is not None else ""
        output_prefix += "-samples" if eval_deltas_dict is not None else ""

        # Now, we should plot this Taylor polynomial, we have all the coefficients
        plot_banana(eval_deltas_dict,
                    output_prefix=output_prefix,
                    plot_type=plot_type,
                    metrics=metrics,
                    centers_dict=eval_centers_dict,
                    walls_dict=eval_walls_dict,
                    output_format=output_format,
                    legend_fixed=legend_fixed,
                    axis_fixed=axis_fixed,
                    verbose=verbose)


if __name__ == '__main__':
    # Call to main running function
    main(args=sys.argv[1:], verbose=True)
