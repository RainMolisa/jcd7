import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.pyplot import MultipleLocator
import os
import sys
import getopt

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        pass
 
    try:
        import unicodedata
        unicodedata.numeric(s)
        return True
    except (TypeError, ValueError):
        pass
 
    return False

if __name__=='__main__':
    os.system('..\\..\\x64\\Release\\jcd7_0 .\\ ref_1280_0.bin 60cm_subpixel1_y1_x64-00000198-ir.bin 1280 800 846.67 42 600 64 25')
    if(os.path.exists('res\\point\\point_result.txt')):
        pdata=np.loadtxt('res\\point\\point_result.txt')
        [pd_rows,pd_cols]=pdata.shape
        plt.clf()
        for i in range(pd_cols):
            plt.plot(pdata[:,i],label=str(i))
        plt.legend()
        plt.savefig('res\\point\\map1.png')
    else:
        #os.system('..\\..\\x64\\Release\\hand_point')
        pass
    if(os.path.exists('res\\line\\line_depthF.txt') and os.path.exists('res\\line\\line_depthT.txt')):
        i=0
        with open('res\\line\\line_depthF.txt', "r") as f1:
            dataF = f1.readlines()
        with open('res\\line\\line_depthT.txt', "r") as f2:
            dataT = f2.readlines()
        for dtF,dtT in zip(dataF,dataT):
            plt.clf()
            arrF = dtF.split(' ')
            arrT = dtT.split(' ')
            def strlist2arr(strlist):
                res=[]
                for val in strlist:
                    if(is_number(val)):
                        v=float(val)
                        res.append(v)
                return res
            af=strlist2arr(arrF)
            at=strlist2arr(arrT)
            plt.plot(af,label='no_sub'+str(i))
            plt.plot(at,label='hv_sub'+str(i))
            ax=plt.gca()
            ax.yaxis.set_major_locator(MultipleLocator(2))
            plt.legend()
            plt.savefig('res\\line\\line_depth'+str(i)+'.png')
            i=i+1
    else:
        os.system('..\\..\\x64\\Release\\hand_line')
    