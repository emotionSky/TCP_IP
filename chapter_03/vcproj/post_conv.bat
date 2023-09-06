@REM 设置一些路径
set project_name=endian_conv
set dreamsky_base=..\..\DreamSky\3d_part
set bin_base=..\..\bin\chapter_03\%project_name%

@REM 只需要用到dll的一些库的拷贝
@REM 一些特殊的库就需要单独拷贝
@REM 注意调试情况下的拷贝
set DREAMS="DreamCommon"

for %%a in (%DREAMS%) do (
	for /f "tokens=1" %%b in (%%a) do (
		copy %dreamsky_base%\%%b\lib\*.dll  %bin_base%\
	)
)
