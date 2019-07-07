set PATH=%PATH%;%~dp0/../dependencies/tools/bison/bin;%~dp0/../dependencies/tools/flex/bin
if not exist "%~dp0/../autogen/" (
    mkdir "%~dp0/../autogen/"
)

win_bison.exe -Wconflicts-sr --verbose "%~dp0/../flex-bison/specification.y" --defines="%~dp0/../autogen/parser.auto.h" --output="%~dp0/../autogen/parser.auto.cpp"
win_flex.exe -+ -wincompat -o"%~dp0/../autogen/scanner.auto.cpp" "%~dp0/../flex-bison/scanner.l"
