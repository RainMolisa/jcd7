CALL conda activate lrn00

::python offset2d_test.py -p ..\set\19 --ss=64 --ps=25
python offset2d_test.py -p ..\set\20 --ss=64 --ps=25
CALL conda deactivate
pause