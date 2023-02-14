# System imports
import os
import sys

# Project libraries
import tools
import reader

# Import matplotlib stuff
import matplotlib.pyplot as plt


def plot_banana(taylor: dict, output_path: os.PathLike or str, verbose: bool = False) -> None:
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
    x = []
    y = []
    z = []
    for delta in taylor:
        for var in taylor[delta]:
            if var == "0" or var == "1" or var == "2":
                # Get index and coef
                val = taylor[delta][var]["1"]["coef"] / 1000  # to km
                x.append(val) if var == "0" else (y.append(val) if var == "1" else z.append(val))

    # Initialise the subplot function using number of rows and columns
    figure, ax = plt.subplots(1, 3, figsize=(10, 7))

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

    # Enable grid
    ax[0].grid(True)
    ax[1].grid(True)
    ax[2].grid(True)

    # Set subtitle
    plt.suptitle("Final position distribution")

    # Show plot
    plt.show()

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
    plt.show()

    # Show plot
    plt.savefig(output_path)


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
        txt_filename = os.path.split(parsed_dict['file'])[1].replace('dd', 'png')
        output_path = os.path.join(parent_folder, f"{txt_filename}")

        # Now, we should plot this Taylor polynomial, we have all the coefficients
        plot_banana(taylor_dict, verbose=verbose, output_path=output_path)


if __name__ == '__main__':
    # Call to main running function
    main(args=sys.argv[1:], verbose=True)
