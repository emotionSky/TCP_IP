@REM ����һЩ·��
set project_name=echo_client
set dreamsky_base=..\..\DreamSky\3d_part
set bin_base=..\..\bin\chapter_04\%project_name%

@REM ֻ��Ҫ�õ�dll��һЩ��Ŀ���
@REM һЩ����Ŀ����Ҫ��������
@REM ע���������µĿ���
set DREAMS="DreamCommon"^
		   "DreamSocket"

for %%a in (%DREAMS%) do (
	for /f "tokens=1" %%b in (%%a) do (
		copy %dreamsky_base%\%%b\lib\*.dll  %bin_base%\
	)
)