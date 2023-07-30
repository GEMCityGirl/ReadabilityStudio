::Build RS help (you should build "ReadabilityStudio" bookdown help manually to review any issues)
DEL /Q /S "Z:\Projects\Readability Studio\ReadabilityStudioDocs\"
XCOPY /Y /R /S "Z:\Docs\ReadabilityStudio\docs" "Z:\Projects\Readability Studio\ReadabilityStudioDocs\"

::Build RS API documentation (you should build "ReadabilityStudioAPI" bookdown help manually to review any issues)
DEL /Q /S "Z:\Projects\Readability Studio\ReadabilityStudioAPI\"
XCOPY /Y /R /S "Z:\ReadabilityStudio\Docs\ReadabilityStudioAPI\docs" "Z:\Projects\Readability Studio\ReadabilityStudioAPI\"

::Lua Reference
DEL /Q /S "Z:\Projects\Readability Studio\LuaManual\"
XCOPY /Y /R /S "Z:\src\lua\doc\*.*" "Z:\Projects\Readability Studio\LuaManual\"

::Lua scripting files
XCOPY /Y /R Z:\ReadabilityStudio\Resources\Scripting\RS* "z:\Projects\Readability Studio\"

::create the resources file
"C:\Program Files\7-Zip\7z" a -tzip -mx0 Z:\ReadabilityStudio\Resources\res.zip -r Z:\ReadabilityStudio\Resources\Images\*.jpg Z:\ReadabilityStudio\Resources\Images\*.png Z:\ReadabilityStudio\Resources\Images\*.svg Z:\ReadabilityStudio\Resources\Images\*.xrc
"C:\Program Files\7-Zip\7z" t Z:\ReadabilityStudio\Resources\res.zip *.* -r

MOVE /Y Z:\ReadabilityStudio\Resources\res.zip Z:\ReadabilityStudio\Resources\res.wad
XCOPY /Y /R Z:\ReadabilityStudio\Resources\res.wad "z:\Projects\Readability Studio\vc_mswud\"
XCOPY /Y /R Z:\ReadabilityStudio\Resources\res.wad "z:\Projects\Readability Studio\vc_mswu\"
XCOPY /Y /R Z:\ReadabilityStudio\Resources\res.wad "z:\Projects\Readability Studio\"
XCOPY /Y /R Z:\ReadabilityStudio\Resources\res.wad "Z:\Installers\Windows\release\"
DEL Z:\ReadabilityStudio\Resources\res.wad

::create the word and phrase file
"C:\Program Files\7-Zip\7z" a -tzip -mx0 Z:\ReadabilityStudio\Resources\Words.zip -r Z:\ReadabilityStudio\Resources\Words\*.txt -x!BaseEnglishDictionary.txt -x!NonPersonalBase.txt -x!PersonalBase.txt
"C:\Program Files\7-Zip\7z" t Z:\ReadabilityStudio\Resources\Words.zip *.* -r

MOVE /Y Z:\ReadabilityStudio\Resources\Words.zip Z:\ReadabilityStudio\Resources\Words.wad
XCOPY /Y /R Z:\ReadabilityStudio\Resources\Words.wad "z:\Projects\Readability Studio\vc_mswud\"
XCOPY /Y /R Z:\ReadabilityStudio\Resources\Words.wad "z:\Projects\Readability Studio\vc_mswu\"
XCOPY /Y /R Z:\ReadabilityStudio\Resources\Words.wad "z:\Projects\Readability Studio\"
XCOPY /Y /R Z:\ReadabilityStudio\Resources\Words.wad "Z:\Installers\Windows\release\"
DEL Z:\ReadabilityStudio\Resources\Words.wad

::copy over production program files
XCOPY /Y /R "Z:\Projects\Readability Studio\vc_mswu\*.exe" "Z:\Installers\Windows\release\"
XCOPY /Y /R "Z:\SRC\lua\src\*.dll" "Z:\Installers\Windows\release\"
XCOPY /Y /R "Z:\SRC\curl\builds\libcurl-vc15-x86-release-dll-ipv6-sspi-winssl\bin\*.dll" "Z:\Installers\Windows\release\"
