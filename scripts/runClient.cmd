cd ..

:while1 
timeout /nobreak 30
debug\scorchedc.exe -connect 192.168.0.107 -connectwithdefaults -exittime 300
goto while1


