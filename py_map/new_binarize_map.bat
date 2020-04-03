CALL conda activate lrn00

python new_binarize_map.py -p ..\set\11 --ss=64 --ps=25 --out_abs=100 --out_rel=0.1

python new_binarize_map.py -p ..\set\17 --ss=64 --ps=25 --out_abs=100 --out_rel=0.1 --tw=600

python new_binarize_map.py -p ..\set\15 --ss=64 --ps=25 --out_abs=100 --out_rel=0.1

python new_binarize_map.py -p ..\set\18 --ss=64 --ps=25 --out_abs=100 --out_rel=0.1 --tw=860

python new_binarize_map.py -p ..\set\19 --ss=64 --ps=25 --out_abs=100 --out_rel=0.1

CALL conda deactivate
pause