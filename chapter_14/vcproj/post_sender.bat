@REM ����һЩ·��
set project_name=news_sender
set dreamsky_base=..\..\DreamSky\3d_part
set bin_base=..\..\bin\chapter_14\%project_name%

copy news.txt %bin_base%\

@REM ֻ��Ҫ�õ�dll��һЩ��Ŀ���
@REM һЩ����Ŀ����Ҫ��������
@REM ע���������µĿ���
set DREAMS="DreamCommon 1.0.0"^
		   "DreamSocket 1.0.0"

for %%a in (%DREAMS%) do (
	for /f "tokens=1,2" %%b in (%%a) do (
		copy %dreamsky_base%\%%b\v%%c\bin\*.dll  %bin_base%\
	)
)
