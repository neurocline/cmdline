project 'cmdline'
    kind 'StaticLib'
    
    includedirs { '../include' }

    files
    {
        '**.cpp', '**.h',
        '../include/**.h'
    }

    -- for Visual Studio, include the version number in the project name
    filter { 'action:vs*' }
        filename "%{prj.name}.%{string.sub(_ACTION, 3)}"
