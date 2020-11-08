@echo off
for %%f IN (%*) DO (
    .\delete %%f
)