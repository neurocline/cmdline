project 'cmdline'
    kind 'StaticLib'
    
    includedirs { '../include' }

    files
    {
        '**.cpp',
        '../include/**.h'
    }
