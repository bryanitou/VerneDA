#!env/python
import math
import os

# Format strings
# End of variable string
end_of_variable = "------------------------------------------------"


def parse_arguments(args: list[str], verbose: bool = False) -> dict:
    """
    Parse arguments, given a list.
    Reminder: Arguments should be this format: --<key_word> <value>
    :param args: given arguments
    :param verbose: verbosity boolean
    :return: dictionary with arguments
    """

    # Create empty dictionary
    parsed_dict = {}

    # Iterate through the list of arguments and their values
    for i, arg in enumerate(args):
        if "--" in arg:
            key = arg.replace("--", "")
            parsed_dict[key] = ""

            # Then, we should add the value
            # Safety check if it exists
            if i + 1 > len(args) - 1:
                if verbose:
                    print(f"Error when parsing argument '{arg}', no value found.")
                exit(-1)
            else:
                # Assign value
                parsed_dict[key] = args[i + 1]
                continue
        else:
            continue

    # Return successful
    return parsed_dict


def get_dict_from_file(filepath: os.PathLike or str, verbose: bool = False) -> dict:
    """
    Get the parameters from the file.
    :param filepath: path to the file.
    :param verbose: verbosity indicator
    :return: dict
    """
    # Auxiliary variables
    headers = None
    elem_set = 0

    # Set dictionary to return
    filedict = {}

    # Name function
    name_function = "function"
    name_variable = "variable"

    # Function
    filedict[name_function] = {"name": None, "param": {}}
    filedict[name_variable] = {"name": None, "param": {}}

    # Separator
    separator = False

    # Iterate through each line
    with open(filepath, "r") as f:
        # Read lines
        fread = f.readlines()

        # All lines are here, should iterate them
        for i, line in enumerate(fread):
            # Clean strings such as the next line '\n'
            line = line.replace("\n", "")

            # If there's nothing, jump next line
            if len(line) == 0:
                # Nothing to parse
                continue

            # First line is the DA function
            if (i == 0 or separator) and elem_set < 2:
                filedict[name_function if separator else name_variable]["name"] = line.strip()
                elem_set += 1
            # TODO: This is a total nyapa, please change this
            elif "I  COEFFICIENT              ORDER EXPONENTS" in line:
                # Should separate each line by space
                # 2nd line are the headers: I COEFFICIENT ORDER EXPONENT
                headers = [h.strip() for h in line.split()]

                # Now, should name these headers into our dictionary
                for header in headers:
                    if header not in filedict:
                        filedict[name_function if separator else name_variable]["param"][header] = []

            else:
                # Check that the headers have been filled up
                if headers is None:
                    if verbose:
                        print("The header has not been found in this file! Avoiding read.")
                        exit(-1)
                else:
                    # Safety check that this is not the last line
                    if line == end_of_variable:
                        separator = True
                        continue

                    # Collect all the coefficients
                    rows = line.split()

                    # Fill in their place
                    for j, row in enumerate(rows):
                        # Get the header
                        header = headers[j]

                        # Try to convert it to a number
                        try:
                            coef = float(row)
                        except Exception as e:
                            if verbose:
                                print(f"The string {row} could not be parsed as a float!")
                                print(f"Error:")
                                print(f"{e.__str__()}")
                                exit(-1)

                        # Append back in their place
                        filedict[name_function if separator else name_variable]["param"][header].append(coef)

    return filedict


def get_chars2remove() -> str:
    """
    Miscellaneous function that returns a list of characters to be removed from somewhere.
    :return: chars
    """
    return "\\(|=|\\)|\\s"


def get_generalized_powered_meth(x: float, n: int, math_meth: math.sin or math.cos) -> float:
    """
    Generalized way to get a function to the power of n.
    :param x: input to the math function.
    :param n: to the power of...
    :param math_meth: mathematical method.
    :return: y value
    """
    return math.pow(math_meth(x), n)
