import json
import matplotlib.pyplot as plt
import numpy as np

if __name__ == '__main__':

    f = open("/home/bryan/Downloads/nli_series_rk4.json")
    f2 = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_16:29:20.csv", delimiter=",")
    json_dic = json.load(f)
    plt.plot(json_dic["output"]["time_series"], json_dic["output"]["nli_series"])
    plt.plot(f2[:,0], f2[:,1])
    plt.show()


