@REM ����һЩ·��
set project_name=signal
set dreamsky_base=..\..\DreamSky\3d_part
set bin_base=..\..\bin\chapter_10\%project_name%

@REM ֻ��Ҫ�õ�dll��һЩ��Ŀ���
@REM һЩ����Ŀ����Ҫ��������
@REM ע���������µĿ���
set DREAMS="DreamCommon v1.0.0"

for %%a in (%DREAMS%) do (
	for /f "tokens=1,2" %%b in (%%a) do (
		copy %dreamsky_base%\%%b\%%c\bin\*.dll  %bin_base%\
	)
)
