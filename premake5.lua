newoption { trigger = 'to', value = 'path', description = 'Set the output location for the generated files' }

if not _OPTIONS['to'] then
    _OPTIONS['to'] = 'build'
end

workspace 'cmdline'
    configurations { 'Debug', 'Release' }
    platforms { 'x32', 'x64' }
    location ( _OPTIONS['to'] )

    warnings 'Extra'
    flags { 'C++14' }
    startproject 'test-cmdline'

    import {
        bf = '../bf',
        stdish = '../stdish',
    }

    filter { 'action:vs*' }
        filename '%{wks.name}.%{string.sub(_ACTION, 3)}'

    filter { 'configurations:Debug' }
        symbols 'On'
        optimize 'Off'

    filter { 'configurations:Release' }
        flags { 'NoBufferSecurityCheck' }
        defines { 'NDEBUG' }
        optimize 'On'

    filter { 'action:vs*' }
        defines { '_ITERATOR_DEBUG_LEVEL=0', '_CRT_SECURE_NO_WARNINGS', '_SCL_SECURE_NO_WARNINGS' }

    filter { 'action:vs2015' }
        defines { '_HAS_EXCEPTIONS=0' }

include 'source'
include 'test'
