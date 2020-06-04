@echo off
cl palettize.c /MT /O2 /Oi /link /incremental:no /opt:ref
IF %ERRORLEVEL% == 0 (
	palettize.exe
)
