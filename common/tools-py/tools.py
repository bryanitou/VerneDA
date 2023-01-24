import os


# Format strings
# End of variable string
end_of_variable = "------------------------------------------------"

def parse_arguments(args: list[str], verbose: bool = False) -> dict:
    """
    Parse arguments, given a list.
    Reminder: Arguments should be this format:
    --<key_word> <value>
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

    # Set dictionary to return
    filedict = {}

    # Name function
    name_function = "function"

    # Function
    filedict["function"] = ""

    # Iterate through each line
    with open(filepath, "r") as f:
        # Read lines
        fread = f.readlines()

        # All lines are here, should iterate them
        for i, line in enumerate(fread):
            # Clean strings such as the next line '\n'
            line = line.replace("\n", "")

            # First line is the DA function
            if i == 0:
                filedict[name_function] = line.strip()
            elif i == 1:
                # Should separate each line by space
                # 2nd line are the headers: I COEFFICIENT ORDER EXPONENT
                headers = [h.strip() for h in line.split()]

                # Now, should name these headers into our dictionary
                for header in headers:
                    if header not in filedict:
                        filedict[header] = []

            else:
                # Check that the headers have been filled up
                if headers is None:
                    if verbose:
                        print("The header has not been found in this file! Avoiding read.")
                        exit(-1)
                else:
                    # Safety check that this is not the last line
                    if line == end_of_variable:
                        break

                    # Collect all the coefficients
                    rows = line.split()

                    # Fill in their place
                    for i, row in enumerate(rows):
                        # Get the header
                        header = headers[i]

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
                        filedict[header].append(coef)

    return filedict