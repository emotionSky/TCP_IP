@REM 设置一些路径
set project_name=stdcpy
set dreamsky_base=..\..\DreamSky\3d_part
set bin_base=..\..\bin\chapter_15\%project_name%

copy news.txt %bin_base%\

@REM 只需要用到dll的一些库的拷贝
@REM 一些特殊的库就需要单独拷贝
@REM 注意调试情况下的拷贝
set DREAMS="DreamCommon v1.0.0"

for %%a in (%DREAMS%) do (
	for /f "tokens=1,2" %%b in (%%a) do (
		copy %dreamsky_base%\%%b\%%c\bin\*.dll  %bin_base%\
	)
)