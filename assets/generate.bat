
del assets /s /q
rd /s /q assets
mkdir assets

call ..\tools\Packager\bin\Packager.exe

del ..\android\assets\assets.dat /s /q
xcopy assets "..\android\assets\" /s /y
