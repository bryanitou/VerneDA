import math
import statistics
import matplotlib.pyplot as plt
import numpy as np
import json

if __name__ == '__main__':
    # f1 = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_no_splitting.csv", delimiter=",")
#
    # while True:
    #     f2 = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_10:03:59.csv", delimiter=",")
    #     plt.plot(f1[:,0], f1[:,1]*0.02/max(f1[:,1]))
    #     plt.scatter(f2[:,0], f2[:,1], s=0.0005, c="r")
    #     plt.grid(True)
    #     plt.show(block=False)
    #     plt.pause(10)
    #     plt.close()
    #     plt.cla()
    #     plt.clf()


    #f = open("/home/bryan/Downloads/nli_series_rk4.json")
    #f2 = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_17:12:18.csv", delimiter=",")
    #json_dic = json.load(f)
    #plt.plot(np.array(json_dic["output"]["time_series"]) / (2*math.pi), json_dic["output"]["nli_series"])
    #plt.xlabel("Revolutions")
    #plt.ylabel("NLI [-]")
    #plt.grid(True)
    #plt.savefig('nli.pdf', bbox_inches='tight')
    ## plt.scatter(f2[:,0], f2[:,1], s=1)
    ## plt.show()
    #plt.close()
#
#
    ## Num. of. splits plots
    #f3 = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/n_split_vs_time_0.01.csv", delimiter=",")
    #plt.plot(np.sort(np.array(f3[:,0])), f3[:,1])
    #plt.xlabel("Revolutions")
    #plt.ylabel("# splits")
    #plt.grid(True)
    #plt.savefig('/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/n_split_vs_time_0.01.pdf', bbox_inches='tight')
    ## plt.scatter(f2[:,0], f2[:,1], s=1)
    ## plt.show()
    #plt.close()

    # Num. of. splits plots
    f4  = np.loadtxt("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_23:32:59.csv", delimiter=",")
    nli = np.array(f4[:,1])
    t = np.array(f4[:,0]) / (math.pi * 2)
    nli_a = statistics.mean(nli)
    plt.plot([0.0, 3.0], [0.01, 0.01], "r")

    nli_patch = np.array(f4[:,2])
    nli_groups = []
    t_groups = []
    i = 0
    i_max =  int(f4[:,2][-1])
    for j in range(0, i_max):
        nli_group = []
        t_group = []
        while (nli_patch[i] == nli_patch[i+1]):
            nli_group.append(nli[i])
            t_group.append(t[i])
            i += 1
        i += 1
        nli_groups.append(nli_group)
        t_groups.append(t_group)

        plt.scatter(t_group[0], nli_group[0], s=50, color = "tab:blue", marker= ">")
        plt.plot(t_group, nli_group, "tab:blue")

    plt.xlabel("Revolutions")
    plt.ylabel("NLI [-]")
    plt.grid(True)
    plt.savefig('/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/nli_23:32:59.pdf', bbox_inches='tight')
    # plt.scatter(f2[:,0], f2[:,1], s=1)
    plt.show()
    plt.close()

