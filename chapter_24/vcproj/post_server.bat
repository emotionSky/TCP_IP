@REM ����һЩ·��
set project_name=web_server
set dreamsky_base=..\..\DreamSky\3d_part
set bin_base=..\..\bin\chapter_24\%project_name%

copy ..\web\index.html %bin_base%\

@REM ֻ��Ҫ�õ�dll��һЩ��Ŀ���
@REM һЩ����Ŀ����Ҫ��������
@REM ע���������µĿ���
set DREAMS="DreamCommon 1.0.0"^
		   "DreamSocket 1.0.0"^
		   "DreamThread 1.0.0"^
		   "pthread 3.0.0"

for %%a in (%DREAMS%) do (
	for /f "tokens=1,2" %%b in (%%a) do (
		copy %dreamsky_base%\%%b\v%%c\bin\*.dll  %bin_base%\
	)
)

