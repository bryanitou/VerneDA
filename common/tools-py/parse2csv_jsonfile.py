import matplotlib.pyplot as plt
import numpy as np

if __name__ == '__main__':
    f1 = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_no_splitting.csv", delimiter=",")

    while True:
        f2 = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_10:03:59.csv", delimiter=",")
        plt.plot(f1[:,0], f1[:,1]*0.02/max(f1[:,1]))
        plt.scatter(f2[:,0], f2[:,1], s=0.0005, c="r")
        plt.grid(True)
        plt.show(block=False)
        plt.pause(10)
        plt.close()
        plt.cla()
        plt.clf()


    exit(0)

    f = open("/home/bryan/Downloads/nli_series_rk4.json")
    f2 = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_22:45:35.csv", delimiter=",")
    json_dic = json.load(f)
    plt.plot(json_dic["output"]["time_series"], json_dic["output"]["nli_series"])
    plt.scatter(f2[:,0], f2[:,1], s=0.0005)
    plt.show()


