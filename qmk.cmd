@echo off
setlocal
set "QMK_HOME=E:\GitHubCode\01Main\KeyBrorad\vial-qmk"
py -3.14 "E:\GitHubCode\01Main\KeyBrorad\vial-qmk\qmk_run.py" %*
