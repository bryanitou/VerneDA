# System imports
import math
import os
import sys

# Project libraries
import tools
import reader

# Import matplotlib stuff
import matplotlib.pyplot as plt


def plot_banana(taylor: dict, output_path: [os.PathLike or str], verbose: bool = False) -> None:
    """
    Plots the taylor
    :param taylor: dictionary containing the coefficients and the order
    :param span: Span that the plot will cover
    :param output_path: where the plot should be saved.
    :param verbose: verbosity indicator
    :return: None
    """

    # Safety check
    if not isinstance(taylor, dict):
        if verbose:
            print("Expected input 'taylor' is not a dictionary!")
            exit(-1)

    # Now, should collect X-Y coordinates
    # TODO: Make this work for quaternion AND for orbit
    w = []
    x = []
    y = []
    z = []
    for delta in taylor:
        for var in taylor[delta]:
            if var == "0" or var == "1" or var == "2" or var == "3":
                # Get index and coef
                val = taylor[delta][var]["1"]["coef"]  # to km
                w.append(val) if var == "0" else (x.append(val) if var == "1" else (y.append(int(var)) if var == "2" else z.append(val)))
        print(f"Norm: {w[-1]*w[-1] + x[-1]*x[-1] + y[-1]*y[-1] + z[-1]*z[-1]}")

    if len(z) > 0:
        # Initialise the subplot function using number of rows and columns
        figure, ax = plt.subplots(1, 3, figsize=(16, 9))

        # Creating plot
        ax[0].scatter(x, y, color="green", s=0.4)
        ax[1].scatter(y, z, color="green", s=0.4)
        ax[2].scatter(x, z, color="green", s=0.4)

        # Labels
        ax[0].set_xlabel("x [km]")
        ax[0].set_ylabel("y [km]")
        ax[1].set_xlabel("y [km]")
        ax[1].set_ylabel("z [km]")
        ax[2].set_xlabel("x [km]")
        ax[2].set_ylabel("z [km]")

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

    # Finally, we can plot everything
    plt.scatter(x, y, s=0.4)
    plt.xlabel("x [km]")
    plt.ylabel("y [km]")

    # Write legend
    # plt.legend([f"{taylor['function']['name']}", "Taylor expansion"])

    # Plt show grid
    plt.grid(True)

    # Save second plot
    plt.savefig(output_path[1])


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

        # Re-set verbosity
        if "silent" in parsed_dict:
            verbose = False if str(parsed_dict["silent"]).lower() == "true" else True
        if "span" in parsed_dict:
            span = int(parsed_dict["span"])

        # Now, we should get the information from the file
        taylor_dict = reader.read_dd_file(parsed_dict["file"], verbose=verbose)

        # From the file, get the parent folder and save it
        parent_folder = os.path.abspath(os.path.dirname(parsed_dict["file"]))
        txt_filename1 = os.path.split(parsed_dict['file'])[1].replace('.dd', '_1.png')
        txt_filename2 = os.path.split(parsed_dict['file'])[1].replace('.dd', '_2.png')
        output_path1 = os.path.join(parent_folder, f"{txt_filename1}")
        output_path2 = os.path.join(parent_folder, f"{txt_filename2}")

        # Now, we should plot this Taylor polynomial, we have all the coefficients
        plot_banana(taylor_dict, verbose=verbose, output_path=[output_path1, output_path2])


if __name__ == '__main__':
    # Call to main running function
    main(args=sys.argv[1:], verbose=True)
