project 'test-cmdline'
    kind 'ConsoleApp'

    includedirs { '../include' }
    files { '*.cpp', '*.h' }
    links { 'cmdline' }

    includedependencies
    {
        'bf',
    }
