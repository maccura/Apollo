md .\target\Debug
md .\target\Release
md .\target\Debug\config
md .\target\Release\config
xcopy /y /s /e .\config .\target\Release\config
xcopy /y /s /e .\config .\target\Debug\config
