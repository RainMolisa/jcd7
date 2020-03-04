import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib.pyplot import MultipleLocator
from matplotlib import cm
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

def strlist2arr(strlist):
    res=[]
    for val in strlist:
        if(is_number(val)):
            v=float(val)
            res.append(v)
    return res

if __name__=='__main__':
    os.system('..\\..\\x64\\Release\\jcd7_0 .\\ ref_640.bin 40cm_800x640-00001395-ir.bin 640 800 943 40 600 64 19')
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
    if(os.path.exists('res\\rect\\config.txt')):
        #i=0
        with open('res\\rect\\config.txt', "r") as f1:
            data = f1.readlines()
        n=int(data[0])
        #print(n)
        bins_n=60
        def process0(k,file_str):
            plt.clf()
            for i in range(n):
                strf='res\\rect\\peak_sis'+('%02d'%(i))+'.txt'
                #print(strf)
                dt_strf=np.loadtxt(strf)
                #plt.hist(dt_strf[:,k],bins=bins_n, alpha=0.7,density=True,label=str(i))
                plt.hist(dt_strf[:,k],bins=bins_n, alpha=0.7,histtype='step',cumulative=True,density=True,label=str(i))
            plt.title(file_str)
            plt.legend()
            plt.savefig('res\\rect\\peak_sis_'+file_str+'.png')
            return
        process0(0,'max0')
        process0(1,'max1')
        process0(2,'min0')
        process0(3,'min1')