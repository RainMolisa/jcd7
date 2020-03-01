import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import os
import sys
import getopt

if __name__=='__main__':
    os.system('..\\..\\x64\\Release\\jcd7_0 .\\ ref_640.bin 60cm_800x640-00001065-ir.bin 640 800 943 40 600 64 19')
    if(os.path.exists('res\\point\\point_result.txt')):
        pdata=np.loadtxt('res\\point\\point_result.txt')
        [pd_rows,pd_cols]=pdata.shape
        plt.clf()
        for i in range(pd_cols):
            plt.plot(pdata[:,i],label=str(i))
        plt.legend()
        plt.savefig('res\\point\\map1.png')
    else:
        os.system('..\\..\\x64\\Release\\hand_point')