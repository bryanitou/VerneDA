# Import packages
import os
import re
import sys
import math
import matplotlib.pyplot as plt

# Self libraries
import tools


def default_x_vector(span: int, addstop: bool = True, verbose: bool = False):
    # Vectors to return
    x = None

    # Here a switch case will be
    if span == 1:
        x = default_x_vector_core(-10, 10, addstop=addstop, verbose=verbose)
    elif span == 2:
        x = default_x_vector_core(-5, 5, addstop=addstop, verbose=verbose)
    elif span == 3:
        x = default_x_vector_core(-1, 1, addstop=addstop, verbose=verbose)

    # Safety check that something is about to be returned
    if x is None:
        if verbose:
            print(f"Could not find which '{span}' default vector to return! "
                  f"Revise call to function 'default_x_vector'")

        # Exit code
        exit(-1)

    return x


def default_x_vector_core(x_lower: float, x_upper: float, addstop: bool = True, verbose: bool = False) -> list:
    # Set step size
    step = 0.1

    # Safety check validity
    if x_upper <= x_lower:
        if verbose:
            print(f"Lower bound ('{x_lower}') cannot be greater than the upper ('{x_upper}') bound!")

    # Compute num of elems
    dist = x_upper - x_lower
    n_val = int(dist / step)

    x = [x_lower + step * i for i in range(0, (n_val + 1) if addstop else n_val)]

    # Info
    if verbose:
        print("-> Default x set to:")
        print(f"-> x_lower = {x_lower}")
        print(f"-> x_upper = {x_upper}")
        print(f"-> step = {step}")

    # Return the vector
    return x


def get_images_from_taylor(taylor: dict, span: int, x: list = None, verbose: bool = False) -> dict:
    """
    Gets the x and y from the taylor dict
    :param taylor: dictionary containing the coefficients and the order
    :param span: span that the plot will cover
    :param x: vector of x values
    :param verbose: verbosity indicator
    :return: dict{x: [], y: []}
    """
    # If the x list is None, generate one by default
    if x is None:
        if verbose:
            print(f"You have not passed an x vector!, A default one will be generated.")

        # Get the default vector
        x = default_x_vector(span=span, verbose=verbose)

    # Y values will be saved here
    y = []

    # Now, we should iterate through the x and sum each Taylor sum
    for xval in x:
        # Set this image value to zero
        yval = 0

        # Get the coefficients and the orders
        coeffs = taylor["COEFFICIENT"]
        orders = taylor["ORDER"]

        # Compute
        for i, coef in enumerate(coeffs):
            yval += coef * pow(xval, orders[i])

        # Now, should append this new value
        y.append(yval)

    # Dictionary to return
    xydict = {"x": x, "y": y}

    # Return result
    return xydict


def get_original(taylor: dict, x: list, verbose: bool = False) -> dict:
    """
    Get originals
    :param taylor: dictionary containing the coefficients and the order
    :param x: vector of x values
    :param verbose: verbosity indicator
    :return:
    """

    # Extract the name of the function, which usually is like 'y = sin(x)'
    function_str = taylor["function"]

    # Now, try to guess which one is
    function_rhs = function_str.split("=")[1].strip()

    # Recognized functions
    recognized_fncts = ["sin(x)", "cos(x)", "exp(x)"]
    recognized_meth = [math.sin, math.cos, math.exp]

    # Method to use
    method2use = None
    powered_num = None

    # Vector Y to return
    y = []

    # Now, try to guess what do we have
    if "^" in function_rhs:  # TODO: Robustify code here
        powered_num = int(str(function_rhs).split("^")[1])
        function_rhs = function_rhs.split("^")[0]

    if function_rhs in recognized_fncts:
        for i, rec in enumerate(recognized_fncts):
            if function_rhs in rec:
                method2use = recognized_meth[i]
    else:
        if verbose:
            print(f"Could not recognize the given function '{function_str}' !")
            exit(-1)

    # Only if got
    if method2use is not None:
        if powered_num is not None:
            for val in x:
                y.append(tools.get_generalized_powered_meth(val, powered_num, method2use))
        else:
            for val in x:
                y.append(method2use(val))

    # XY dict
    xydict = {"x": x, "y": y}

    # Return result
    return xydict


def plot_taylor(taylor: dict, span: int, output_path: os.PathLike or str, verbose: bool = False) -> None:
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

    # Now we should get the x and y from the Taylor polynomial information
    xytaylor = get_images_from_taylor(taylor, span=span, verbose=verbose)

    # Get original
    xyoriginal = get_original(taylor, xytaylor["x"], verbose=verbose)

    # Finally, we can plot everything
    plt.plot(xyoriginal["x"], xyoriginal["y"])
    plt.plot(xytaylor["x"], xytaylor["y"])

    # Write legend
    plt.legend([f"{taylor['function']}", "Taylor expansion"])

    # Plt show grid
    plt.grid()

    # Show plot
    plt.savefig(output_path)


# Main running function
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
        taylor_dict = tools.get_dict_from_file(parsed_dict["file"], verbose=verbose)

        # From the file, get the parent folder and save it
        parent_folder = os.path.abspath(os.path.dirname(parsed_dict["file"]))
        txt_filename = os.path.split(parsed_dict['file'])[1].replace('txt', 'png')  # TODO: Robustify this
        output_path = os.path.join(parent_folder, f"{txt_filename}")

        # Now, we should plot this Taylor polynomial, we have all the coefficients
        plot_taylor(taylor_dict, span=span, verbose=verbose, output_path=output_path)


if __name__ == '__main__':
    # Call to main running function
    main(args=sys.argv[1:], verbose=True)
