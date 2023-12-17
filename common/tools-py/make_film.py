import os
import reader
import banana


def main():
    dirpath = "/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/translation_loads_RK4_validation_nli_0.02_time_4.71238898038469/film2"
    filenames = os.listdir(dirpath)
    filenames.sort()

    for fname in filenames:
        if "output" in fname:
            continue
        fpath = os.path.join(dirpath, fname)
        fpath_numberpart = fname.split(".")[0]

        # Parse file
        eval_walls_dict = reader.read_dd_file(fpath, verbose=True, walls=True)

        banana.plot_banana(None, os.path.join(dirpath, "output_png", fpath_numberpart), banana.PlotType.translation, "-", None, eval_walls_dict)


if __name__ == '__main__':
    main()