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

def do_depth_analysis(wrk_pth,
                      rows,cols,
                      baseline,fmu,wall,
                      out_abs,out_rel,is_wall,trg_wall=0):
    
    
    depth_old=np.loadtxt(wrk_pth+'/res/soft_depth1.txt',dtype=np.float32)
    depth_new=np.loadtxt(wrk_pth+'/res/soft_depth6432.txt',dtype=np.float32)
    def wall_sis(depth,out_fdr):
        ref_d0=wall
        ref_temp=0
        trg_d0=trg_wall
        trg_temp=0
        depth2=depth.copy()
        out_map=np.zeros((rows,cols),dtype=np.float32)
    
        abs_error=np.zeros((rows,cols),dtype=np.float32)
        rel_error=np.zeros((rows,cols),dtype=np.float32)
    
        def mk_error():
            out_num=0.0
            out_min=0.0
            out_max=0.0
            for y in range(rows):
                for x in range(cols):
                    val=depth[y,x]
                    abs_v=val-float(trg_d0)
                    rel_v=abs_v/float(trg_d0)
                    if(abs(abs_v)<=out_abs and abs(rel_v)<=out_rel):
                        abs_error[y,x]=abs_v
                        rel_error[y,x]=rel_v
                    else:
                        depth2[y,x]=trg_d0
                        out_map[y,x]=1
                        out_num=out_num+1
                        if(abs_v<0):
                            out_min=out_min+1
                        else:
                            out_max=out_max+1
            return out_num,out_min,out_max
        out_num,out_min,out_max=mk_error()
        in_num=int(rows*cols-out_num)
        def mk_error_array():
            abse_set=np.zeros(in_num,dtype=np.float32)
            rele_set=np.zeros(in_num,dtype=np.float32)
            i=0
            for y in range(rows):
                for x in range(cols):
                    val=depth[y,x]
                    abs_v=val-float(trg_d0)
                    rel_v=abs_v/float(trg_d0)
                    if(abs(abs_v)<=out_abs and abs(rel_v)<=out_rel):
                        abse_set[i]=abs_error[y,x]
                        rele_set[i]=rel_error[y,x]
                        i=i+1
            return abse_set,rele_set
        abse_set,rele_set=mk_error_array()
        out_rate=(1000.0*out_num)/float(rows*cols)
        out_minR=(1000.0*out_min)/float(rows*cols)
        out_maxR=(1000.0*out_max)/float(rows*cols)
    
        srmse=0.0
        lst_len=abse_set.shape[0]
        for i in range(lst_len):
            srmse=srmse+abse_set[i]*abse_set[i]
        if(lst_len<=0):
            srmse=0
        else:
            srmse=srmse/float(lst_len)
        srmse=mth.sqrt(srmse)
    
        #abshx,abshy=mk_histgm(abse_set,1000)
        #relhx,relhy=mk_histgm(rele_set,1000)
        #abscdf=mk_cdf(abshy)
        #relcdf=mk_cdf(relhy)
    
        flog=open(out_fdr+'/slog.txt','w')
        flog.write("ref:d=%d temp=%d\n"%(ref_d0,ref_temp))
        flog.write("trg:d=%d temp=%d\n"%(trg_d0,trg_temp))
        flog.write("out_rate=%f per-thousand\n"%(out_rate))
        flog.write("out_minR=%f per-thousand\n"%(out_minR))
        flog.write("out_maxR=%f per-thousand\n"%(out_maxR))
        flog.write("srmse=%f\n"%(srmse))
        flog.close()
    
        jrjstr='rD=%d_rT=%d_tD=%d_tT=%d'%(ref_d0,ref_temp,trg_d0,trg_temp)
    
        plt.figure(1,figsize=(4.2, 4.8))
        plt.cla()
        plt.clf()
        #plt.figure(figsize=(2, 8))
        #mpl.rcParams['figure.figsize']=(5,4.8)
        plt.title(jrjstr+'_depth')
        plt.imshow(depth)
        plt.colorbar()
        plt.savefig(out_fdr+'/org_depth.png')
    
        plt.figure(2,figsize=(4.2, 4.8))
        plt.cla()
        plt.clf()
        plt.title(jrjstr+'depth2')
        plt.imshow(depth2)
        plt.colorbar()
        plt.savefig(out_fdr+'/mid_depth.png')
        
        plt.figure(3,figsize=(4.2, 4.8))
        plt.cla()
        plt.clf()
        plt.title(jrjstr+'Absolute error')
        plt.imshow(abs_error)
        plt.colorbar()
        plt.savefig(out_fdr+'/abs_error.png')
        
        plt.figure(4,figsize=(4.2, 4.8))
        plt.cla()
        plt.clf()
        plt.title(jrjstr+'Relative error')
        plt.imshow(rel_error)
        plt.colorbar()
        plt.savefig(out_fdr+'/rel_error.png')
        
        plt.figure(5)
        plt.cla()
        plt.clf()
        plt.title(jrjstr+'Absolute error histrogram')
        #plt.plot(abshx,abshy,'*-')
        plt.hist(abse_set,bins='auto',histtype='step')
        plt.xlabel('Absolute error(mm)')
        plt.grid(True)
        plt.savefig(out_fdr+'/abs_e_his.png')
    
        plt.figure(6)
        plt.cla()
        plt.clf()
        plt.title(jrjstr+'Absolute error cdf')
        #plt.plot(abshx,abscdf,'*-')
        plt.hist(abse_set,cumulative=True,density=True,bins='auto',histtype='step',linewidth=2.5)
        plt.xlabel('Absolute error(mm)')
        plt.grid(True)
        plt.savefig(out_fdr+'/abs_e_cdf.png')
        
        plt.figure(7)
        plt.cla()
        plt.clf()
        plt.title(jrjstr+'Relative error histrogram')
        plt.hist(rele_set,bins='auto',histtype='step')
        plt.xlabel('error')
        plt.grid(True)
        plt.savefig(out_fdr+'/rel_e_his.png')
    
        plt.figure(8)
        plt.cla()
        plt.clf()
        plt.title(jrjstr+'Relative error cdf')
        plt.hist(rele_set,cumulative=True,density=True,bins='auto',histtype='step',linewidth=2.5)
        plt.xlabel('Relative error(mm)')
        plt.grid(True)
        plt.savefig(out_fdr+'/rel_e_cdf.png')
    
        plt.figure(9,figsize=(4.5, 4.8))
        plt.cla()
        plt.clf()
        plt.title(jrjstr+'_Outlier distribution')
        plt.imshow(out_map, cmap='gray')
        plt.colorbar()
        plt.savefig(out_fdr+'/outer_map.png')
        return
    if(is_wall):
        os.system("mkdir " + wrk_pth+'\\res\\wallsis')
        os.system("mkdir " + wrk_pth+'\\res\\wallsis\\old')
        os.system("mkdir " + wrk_pth+'\\res\\wallsis\\new')
        wall_sis(depth_old,wrk_pth+'\\res\\wallsis\\old')
        wall_sis(depth_new,wrk_pth+'\\res\\wallsis\\new')
    

if __name__=='__main__':
    opts, args = getopt.getopt(sys.argv[1:],
                               'p:l:r:',
                               ['ps=','tw=','oa=','or='])
    wrk_pth=''
    ref_pth=''
    trg_pth=''
    rows=0
    cols=0
    ptch_s=0
    l=0
    r=0
    baseline=0
    fmu=0.0
    wall=0
    cur_mod=0
    tg_wall=0
    out_rel=0
    out_abs=0
    
    #print(opts)
    #print(args)
    for key,value in opts:
        if key in ['-p']:
            wrk_pth=value
        if key in ['--ps']:
            ptch_s=int(value)
        if key in ['-l']:
            l=int(value)
        if key in ['-r']:
            r=int(value)
        if key in ['--tw']:
            tg_wall=int(value)
        if key in ['--oa']:
            out_abs=int(value)
        if key in ['--or']:
            out_rel=float(value)
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
    print('l=',l)
    print('r=',r)
    print('baseline=',baseline)
    print('fmu=',fmu)
    print('wall=',wall)
    print('cur_mod=',cur_mod)
    print('tg_wall=',tg_wall)
    print('out_rel=',out_rel)
    print('out_abs=',out_abs)
    
    run_cmd=wrk_pth+' config.txt'\
            +(' %d '%l)+('%d '%r)+('%d '%ptch_s)
    run_cmd='..\\x64\\Release\\jcd7_6432.exe '+run_cmd
    print(run_cmd)
    os.system(run_cmd)
    do_depth_analysis(wrk_pth,
                      rows,cols,
                      baseline,fmu,wall,
                      out_abs,out_rel,True,tg_wall)



























