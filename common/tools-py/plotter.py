#!env/python
# Import packages
import os
import sys
import sympy
import matplotlib.pyplot as plt

# Self libraries
import tools


def get_variable_middle_point(taylor: dict) -> int or float:
    # Local variables
    result = None

    # Search for order 0 coef
    for i, order_str in enumerate(taylor["variable"]["param"]["ORDER"]):
        # Convert to integer
        order_fl = int(order_str)

        # Means it is the center
        if order_fl == 0:
            result = taylor["variable"]["param"]["COEFFICIENT"][i]
            result = float(result)

    # If nothing found, the function is centered in the center: x0 = 0
    if result is None:
        result = 0

    return result


def default_x_vector(span: int, middlepoint: int or float, addstop: bool = True, verbose: bool = False):
    # Final displaced span
    centered_span = [middlepoint - span / 2, middlepoint + span / 2]

    # Vector of x
    x = default_x_vector_core(*centered_span, addstop=addstop, verbose=verbose)

    return x


def default_x_vector_core(x_lower: float, x_upper: float, addstop: bool = True, verbose: bool = False) -> list:
    # Set step size
    step = 0.01

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
        x = default_x_vector(span=span, middlepoint=get_variable_middle_point(taylor), verbose=verbose)

    # Y values will be saved here
    y = []

    # Now, we should iterate through the x and sum each Taylor sum
    for xval in x:
        # Set this image value to zero
        yval = 0

        # Get the coefficients and the orders
        coeffs = taylor["function"]["param"]["COEFFICIENT"]
        orders = taylor["function"]["param"]["ORDER"]

        # Displacement
        x_0 = get_variable_middle_point(taylor)

        # Compute
        for i, coef in enumerate(coeffs):
            yval += coef * pow(xval - x_0, orders[i])

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
    function_str = taylor["function"]["name"]

    # Now, try to guess which one is
    function_rhs = function_str.split("=")[1].strip()

    # Convert our function into
    x_sym = sympy.symbols("x")
    expr = sympy.sympify(function_rhs)
    f = sympy.lambdify(x_sym, expr)

    # Images
    y = []

    # Compute
    for val in x:
        y.append(f(val))

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
    plt.legend([f"{taylor['function']['name']}", "Taylor expansion"])

    # Plt show grid
    plt.grid()

    # Show plot
    plt.savefig(output_path)

    # Info
    if verbose:
        print(f"The resultant figure has been saved in: {output_path}")



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
        txt_filename = os.path.split(parsed_dict['file'])[1].replace('txt', 'png')
        output_path = os.path.join(parent_folder, f"{txt_filename}")

        # Now, we should plot this Taylor polynomial, we have all the coefficients
        plot_taylor(taylor_dict, span=span, verbose=verbose, output_path=output_path)


if __name__ == '__main__':
    # Call to main running function
    main(args=sys.argv[1:], verbose=True)
