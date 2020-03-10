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

def graph_cdf(data1,bins_n,title_str,xlabel_str,savepath):
    plt.clf()
    plt.hist(data1,bins=bins_n, facecolor="blue", edgecolor="blue", alpha=0.7)
    plt.hist(data1,bins=bins_n, facecolor="red", edgecolor="red", alpha=0.7,histtype='step',cumulative=True)
    plt.xlabel(xlabel_str)
    plt.title(title_str)
    plt.savefig(savepath)
    return

if __name__=='__main__':
    os.system('..\\..\\x64\\Release\\jcd7_0 .\\ ref_640.bin 60cm_800x640-00001065-ir.bin 640 800 943 40 600 64 19')
    if(os.path.exists('res\\point\\point_result.txt')):
        pdata=np.loadtxt('res\\point\\point_result.txt')
        pt=np.loadtxt('res\\point\\point_best_x.txt')
        [pd_rows,pd_cols]=pdata.shape
        plt.clf()
        for i in range(pd_cols):
            plt.plot(pdata[:,i],label=str(i))
        for i in range(pd_cols):
            plt.plot(pt[i,0],pt[i,1],'*')
        plt.legend()
        plt.savefig('res\\point\\map1.png')
    else:
        pass
        #os.system('..\\..\\x64\\Release\\hand_point')
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
        pass
        #os.system('..\\..\\x64\\Release\\hand_line')
    if(os.path.exists('res\\line\\line_offset.txt')):
        i=0
        with open('res\\line\\line_offset.txt', "r") as f1:
            data = f1.readlines()
        for dt in (data):
            plt.clf()
            arr = dt.split(' ')
            
            a=strlist2arr(arr)
            plt.plot(a,label='offset'+str(i))
            #ax=plt.gca()
            #ax.yaxis.set_major_locator(MultipleLocator(2))
            plt.legend()
            plt.savefig('res\\line\\line_offset'+str(i)+'.png')
            i=i+1
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
        
        