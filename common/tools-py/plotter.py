# Import packages
import os
import sys
import matplotlib.pyplot as plt
import math

# Self libraries
import tools


def default_x_vector(which: int, addstop: bool = True, verbose: bool = False):
    # Vectors to return
    x = None

    # Here a switch case will be
    if which == 1:
        x = default_x_vector_core(-10, 10, addstop=addstop, verbose=verbose)
    elif which == 2:
        x = default_x_vector_core(-5, 5, addstop=addstop, verbose=verbose)
    elif which == 3:
        x = default_x_vector_core(-1, 1, addstop=addstop, verbose=verbose)

    # Safety check that something is about to be returned
    if x is None:
        if verbose:
            print(f"Could not find which '{which}' default vector to return! "
                  f"Revise call to function 'default_x_vector'")

        # Exit code
        exit(-1)

    return x


def default_x_vector_core(x_lower: float, x_upper: float, addstop: bool = True, verbose: bool = True) -> list:
    # Set step size
    step = 0.1

    # Safety check validity
    if x_upper <= x_lower:
        if verbose:
            print(f"Lower bound ('{x_lower}') cannot be greater than the upper ('{x_upper}') bound!")

    # Set limits
    x_upper = (x_upper + step) if addstop else x_upper

    # Compute num of elems
    dist = x_upper - x_lower
    n_val = int(dist / step)

    x = [x_lower + step * i for i in range(0, n_val)]

    # Info
    if verbose:
        print("Default x set to:")
        print(f"x_lower = {x_lower}")
        print(f"x_upper = {x_upper}")
        print(f"step = {step}")

    # Return the vector
    return x


def get_images_from_taylor(taylor: dict, x: list = None, verbose: bool = True) -> dict:
    """
    Gets the x and y from the taylor dictionary
    :param taylor: dictionary containing the coefficients and the order
    :param verbose: verbosity indicator
    :return: dict{x} = [], dict{y} = []
    """
    # If the x list is None, generate one by default
    if x is None:
        if verbose:
            print(f"You have not passed an x vector!, A default one will be generated.")

        # Get the default vector
        x = default_x_vector(2)

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

    # Vector Y to return
    y = []

    # Now, try to guess what do we have
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
        for val in x:
            y.append(method2use(val))

    # XY dict
    xydict = {"x": x, "y": y}

    # Return result
    return xydict


def plot_taylor(taylor: dict, verbose: bool = False) -> None:
    """
    Plots the taylor
    :param taylor: dictionary containing the coefficients and the order
    :param verbose: verbosity indicator
    :return: None
    """

    # Safety check
    assert (taylor, dict)

    # Now we should get the x and y from the Taylor polynomial information
    xytaylor = get_images_from_taylor(taylor, verbose=verbose)

    # Get original
    xyoriginal = get_original(taylor, xytaylor["x"], verbose=verbose)

    # Finally, we can plot everything
    plt.plot(xytaylor["x"], xytaylor["y"])
    plt.plot(xyoriginal["x"], xyoriginal["y"])

    # Write legend
    plt.legend(["Taylor expansion", f"{taylor['function']}"])

    # Plt show grid
    plt.grid()

    # Show plot
    plt.show()


# Main running function
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
            print("No arguments given.")
        exit(-1)
    else:
        # Parse given arguments
        parsed_dict = tools.parse_arguments(args, verbose=verbose)

        # Now, we should get the information from the file
        taylor_dict = tools.get_dict_from_file(parsed_dict["file"], verbose=verbose)

        # Now, we should plot this Taylor polynomial, we have all the coefficients
        plot_taylor(taylor_dict, verbose=verbose)


if __name__ == '__main__':
    # Call to main running function
    main(args=sys.argv[1:], verbose=True)
