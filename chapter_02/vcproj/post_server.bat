@REM 设置一些路径
set project_name=tcp_server
set dreamsky_base=..\..\DreamSky\3d_part
set bin_base=..\..\bin\chapter_02\%project_name%

@REM 只需要用到dll的一些库的拷贝
@REM 一些特殊的库就需要单独拷贝
@REM 注意调试情况下的拷贝
set DREAMS="DreamCommon v1.0.0"^
		   "DreamSocket v1.0.0"

for %%a in (%DREAMS%) do (
	for /f "tokens=1,2" %%b in (%%a) do (
		copy %dreamsky_base%\%%b\%%c\bin\*.dll  %bin_base%\
	)
)
