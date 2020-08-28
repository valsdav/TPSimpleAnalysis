#!/bin/bash

cmsRun test/tp_analysis_cfg.py inputFiles=file:../../ecallocalrunreco/data/run10000${1}/streamA/data/run10000${1}_ls0001_streamA_StorageManager.dat analysisOutput=output/output_run${1}.root
