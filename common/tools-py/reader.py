#!env/python
import csv
import os


def read_avd_file(fpath: os.PathLike or str, verbose: bool = False) -> dict:
    # First row
    firstrow = True

    # Resultant dictionary
    result = {}

    # Desired structure: example for one variable
    # ___________________________________________
    # | result = {                              |
    # |           "0":{                         |
    # |               "1":{"coef": float,       |
    # |                    "order": int,        |
    # |                    "exponents": [int]}  |
    # |               "2":{"coef": float,       |
    # |                    "order": int,        |
    # |                    "exponents": [int]}  |
    # |               }                         |
    # |           "1":{                         |
    # |               "1":{"coef": float,       |
    # |                    "order": int,        |
    # |                    "exponents": [int]}  |
    # |               "2":{"coef": float,       |
    # |                    "order": int,        |
    # |                    "exponents": [int]   |
    # |                   }                     |
    # |               }                         |
    # |          }                              |
    # |_________________________________________|

    # Check if path exist
    if not os.path.exists(fpath):
        if verbose:
            print(f"Cannot open file, check file exists: '{fpath}'")
        exit(-1)

    # Open file: avd (Algebraic Vector Dump) is a CSV formatted file
    with open(fpath, "r") as f:
        readcsv = csv.reader(f)

        # Iterate through the lines
        for row in readcsv:
            if firstrow:
                firstrow = False
                continue

            # Avoid empty rows
            if len(row) == 0:
                continue

            # Here we parse the document
            var_ = str(row[0]).strip()
            idx_ = str(row[1]).strip()
            coe_ = str(row[2]).strip()
            ord_ = str(row[3]).strip()
            exp_ = str(row[4]).strip().replace("[", "").replace("]", "")

            # Create dictionary for this variable...
            if var_ not in result:
                result[var_] = {}

            # If in this var, this idx is not present...
            if idx_ not in result[var_]:
                result[var_][idx_] = {"coef": None, "order": None, "exponents": []}

            # Fill in the info...
            result[var_][idx_]["coef"] = float(coe_)
            result[var_][idx_]["order"] = int(ord_)
            result[var_][idx_]["exponents"] = [int(e) for e in exp_.split(" ")]

    return result


def read_dd_file(fpath: os.PathLike or str, verbose: bool = False, walls: bool = False) -> dict:
    # First row
    firstrow = True

    # Resultant dictionary
    result = {}

    # Desired structure: example for one variable
    # TODO: Fix this
    # ___________________________________________
    # | result = {                                   |
    # |           "delta_id": {                      |
    #                   "0":{                        |
    # |                    "1":{"coef": float,       |
    # |                         "order": int,        |
    # |                         "exponents": [int]}  |
    # |                    "2":{"coef": float,       |
    # |                         "order": int,        |
    # |                         "exponents": [int]}  |
    # |                    }                         |
    # |                "1":{                         |
    # |                    "1":{"coef": float,       |
    # |                         "order": int,        |
    # |                         "exponents": [int]}  |
    # |                    "2":{"coef": float,       |
    # |                         "order": int,        |
    # |                         "exponents": [int]   |
    # |                        }                     |
    # |                    }                         |
    # |          }                                   |
    # |______________________________________________|

    # Check if path exist
    if not os.path.exists(fpath):
        if verbose:
            print(f"Cannot open file, check file exists: '{fpath}'")
        exit(-1)

    # Fix index in case of walls
    offset_idx = 0 if walls else -1

    # Open file: avd (Algebraic Vector Dump) is a CSV formatted file
    with open(fpath, "r") as f:
        readcsv = csv.reader(f)

        # Iterate through the lines
        for row in readcsv:
            if firstrow:
                firstrow = False
                continue

            # Avoid empty rows
            if len(row) == 0:
                continue

            # Here we parse the document
            if walls:
                pat_ = str(row[0]).strip()
            del_ = str(row[1 + offset_idx]).strip()
            var_ = str(row[2 + offset_idx]).strip()
            idx_ = str(row[3 + offset_idx]).strip()
            coe_ = str(row[4 + offset_idx]).strip()
            ord_ = str(row[5 + offset_idx]).strip()
            exp_ = str(row[6 + offset_idx]).strip().replace("[", "").replace("]", "")

            if walls:
                if pat_ not in result:
                    result[pat_] = {}
                if del_ not in result[pat_]:
                    result[pat_][del_] = {}
            else:
                if del_ not in result:
                    result[del_] = {}

            # Create dictionary for this variable...
            if walls:
                if var_ not in result[pat_][del_]:
                    result[pat_][del_][var_] = {}
            else:
                if var_ not in result[del_]:
                    result[del_][var_] = {}

            # If in this var, this idx is not present...
            if walls:
                if idx_ not in result[pat_][del_][var_]:
                    result[pat_][del_][var_][idx_] = {"coef": None, "order": None, "exponents": []}
            else:
                if idx_ not in result[del_][var_]:
                    result[del_][var_][idx_] = {"coef": None, "order": None, "exponents": []}

            # Fill in the info...
            if walls:
                result[pat_][del_][var_][idx_]["coef"] = float(coe_)
                result[pat_][del_][var_][idx_]["order"] = int(ord_)
                result[pat_][del_][var_][idx_]["exponents"] = [int(e) for e in exp_.split(" ")]
            else:
                result[del_][var_][idx_]["coef"] = float(coe_)
                result[del_][var_][idx_]["order"] = int(ord_)
                result[del_][var_][idx_]["exponents"] = [int(e) for e in exp_.split(" ")]

    return result
