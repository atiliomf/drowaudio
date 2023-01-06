setlocal enabledelayedexpansion

set VS_WHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere
echo %VS_WHERE%

for /f "usebackq tokens=*" %%i in (`"%VS_WHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  set MSBUILD_EXE=%%i
)
echo %MSBUILD_EXE%

echo on
cd "%~dp0..\..%"
set ROOT=%cd%

git clone https://github.com/WeAreROLI/JUCE.git --branch develop --single-branch juce

cd "%ROOT%\juce\extras\Projucer\Builds\VisualStudio2022"
"%MSBUILD_EXE%" Projucer.sln /p:VisualStudioVersion=17.0 /m /t:Build /p:Configuration=Release /p:Platform=x64 /p:PreferredToolArchitecture=x64 
if %errorlevel% neq 0 exit /b %errorlevel%

.\x64\Release\App\Projucer.exe --set-global-search-path windows defaultJuceModulePath "%ROOT%\juce\modules" 
.\x64\Release\App\Projucer.exe --resave "%ROOT%\demo\dRowAudio Demo.jucer"

cd "%ROOT%\demo\Builds\VisualStudio2022"
"%MSBUILD_EXE%" "dRowAudio Demo.sln" /p:VisualStudioVersion=17.0 /m /t:Build /p:Configuration=Release /p:Platform=x64 /p:PreferredToolArchitecture=x64 
if %errorlevel% neq 0 exit /b %errorlevel%

mkdir "%ROOT%\ci\win\bin"
copy ".\x64\Release\App\dRowAudio Demo.exe" "%ROOT%\ci\win\bin"