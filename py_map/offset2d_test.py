import os
import shutil
import sys
import re
import getopt
import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt
import matplotlib as mpl
import math as mth


# python new_binarize_map.py -p ..\\set\\19 --ss=64 --ps=25
if __name__=='__main__':
    opts, args = getopt.getopt(sys.argv[1:],
                               '-p:',
                               ['ps=','ss='])
    wrk_pth=''
    ref_pth=''
    trg_pth=''
    rows=0
    cols=0
    ptch_s=0
    srch_s=0
    baseline=0
    fmu=0.0
    wall=0
    cur_mod=0
    
    
    
    for key,value in opts:
        if key in ['-p']:
            wrk_pth=value
        if key in ['--ps']:
            ptch_s=int(value)
        if key in ['--ss']:
            srch_s=int(value)
    cfg_pth=wrk_pth+'/config.txt'
    f=open(cfg_pth,'r')
    cfg_lne=f.readlines()
    #print(cfg_lne)
    ref_pth=cfg_lne[0].rstrip('\n')
    trg_pth=cfg_lne[1].rstrip('\n')
    cur_mod=int(cfg_lne[2].rstrip('\n'))
    rows=int(cfg_lne[3].rstrip('\n'))
    cols=int(cfg_lne[4].rstrip('\n'))
    fmu=float(cfg_lne[5].rstrip('\n'))
    baseline=int(cfg_lne[6].rstrip('\n'))
    wall=int(cfg_lne[7].rstrip('\n'))
    f.close()
    
    print('wrk_pth=',wrk_pth)
    print('ref_pth=',ref_pth)
    print('trg_pth=',trg_pth)
    print('rows=',rows)
    print('cols=',cols)
    print('ptch_s=',ptch_s)
    print('srch_s=',srch_s)
    print('baseline=',baseline)
    print('fmu=',fmu)
    print('wall=',wall)
    print('cur_mod=',cur_mod)
    # ..\set\19 config.txt 64 25
    run_cmd=wrk_pth+' config.txt'\
            +(' %d '%srch_s)+('%d '%ptch_s)
    run_cmd='..\\x64\\Release\\offset2d_test.exe '+run_cmd
    print(run_cmd)
    os.system(run_cmd)
    f=open(wrk_pth+'/res/function.txt','r')
    fun_str=f.readlines();
    fun_str=fun_str[0].rstrip('\n')
    f.close()
    data=np.loadtxt(wrk_pth+'/res/offset2dpth.txt')
    plt.cla()
    plt.clf()
    plt.title(fun_str)
    plt.plot(data[:,0],data[:,1])
    plt.savefig(wrk_pth+'/res/offset2dpth.png')
    
    plt.cla()
    plt.clf()
    plt.title(fun_str)
    plt.plot(data[0:97,0],data[0:97,1])
    plt.savefig(wrk_pth+'/res/offset2dpth2.png')




















