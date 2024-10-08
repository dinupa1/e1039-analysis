# e1039-analysis/RecoData2024/work_ana

The officially-reconstructed data are stored under `/pnfs/e1039/persistent/users/spinquestpro/commisioning_recodata/CoarseFalse/KMagOn/Occu3` as of 2024-09-01.
These data are analyzed in this directory, as selected by `DIR_DST` in `gridsub.sh`.


## Test of Analysis Using a Few Runs

The procedure below will analyze the 4th good run to draw basic dimuon variables.

```
cd /path/to/your/e1039-analysis/RecoData2024/work_ana
source ../setup.sh
./gridsub.sh -j 4-4
root -b 'AnaEventTree.C("scratch/ana")'
display result/h1_m.png &
```

Here are what happens in each step:
- `gridsub.sh` sets up a subdirectory (per run) to analyze DST files of given runs.
    - The option `-j 4-4` selects the 4th run (listed in `list_run_spill.txt`).
    - Analyzing one run is directed by `gridrun.sh` and `Fun4All.C`.
    - A SubsysReco module, `AnaDimuon`, is registered by default.
    - The analysis runs on local computer by default (i.e. when `-g` is not given).
    - Extracted dimuon parameters are stored in `scratch/ana/run_*/out/output.root` as TTree.
- `AnaEventTree.C` reads the TTree files to draw histograms.
    - Histograms are saved under `result/`.


## Analysis Using All Runs

You can analyze other runs by adjusting the `-j` option of `gridsub.sh`.
The `-g` option let you use the Grid computing.
You first submit jobs for the first 10 runs;
```
kinit
./gridsub.sh -g -j 1-10
```

You might execute `jobsub_q_mine` to monitor the job status, which is an alias of `jobsub_q --group spinquest --user=$USER` defined in `setup.sh`.
Once you confirm that the jobs are running fine, you submit more jobs for the remaining runs;
```
./gridsub.sh -g -j 11-
```

The output files are stored under `data/ana/` in case Grid is used.
You can analyze them once (almost) all jobs finish;
```
root -b AnaEventTree.C
display result/h1_m.png &
```


## Analysis Condition

The analysis condition is programmed in `src/AnaDimuon.(h|cc)`.
Thus you need look into these files to understand the default condition.
You might modify these files as you need.
You need execute `make-this` after modification.
