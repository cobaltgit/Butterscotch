project('butterscotch', 'c',
    version: '1.0',
    default_options: ['c_std=gnu99', 'warning_level=2', 'buildtype=debugoptimized']
)

cc = meson.get_compiler('c')
plat = get_option('platform')
desktop_backend = get_option('desktop_backend')
audio_backend = get_option('audio_backend')

# ==============================================================================
# Base Configurations & Overrides
# ==============================================================================

# WAD Versions
if not get_option('enable_wad14') and not get_option('enable_wad16') and not get_option('enable_wad17')
    error('You need to build Butterscotch with at least one WAD version enabled!')
endif

if get_option('enable_wad14')
    add_project_arguments('-DENABLE_WAD14', language: 'c')
endif
if get_option('enable_wad16')
    add_project_arguments('-DENABLE_WAD16', language: 'c')
endif
if get_option('enable_wad17')
    add_project_arguments('-DENABLE_WAD17', language: 'c')
endif

# GL Overrides
enable_legacy = get_option('enable_legacy_gl')
enable_modern = get_option('enable_modern_gl')
if get_option('enable_gles') and enable_legacy
    enable_legacy = false
endif

if enable_legacy
    add_project_arguments('-DENABLE_LEGACY_GL', language: 'c')
endif
if enable_modern
    add_project_arguments('-DENABLE_MODERN_GL', language: 'c')
endif

# Audio Backend Defaults
if audio_backend == 'auto'
    if plat == 'web'
        audio_backend = 'web'
    elif plat == 'web-meta'
        audio_backend = 'none'
    elif plat == 'ps3'
        audio_backend = 'openal'
    elif plat == 'ps2'
        audio_backend = 'ps2'
    else
        audio_backend = 'miniaudio'
    endif
endif

# Target-specific VM flags defaults
is_console = (plat == 'ps2' or plat == 'ps3')

vm_opts = [
    ['enable_vm_gml_profiler', 'ENABLE_VM_GML_PROFILER'],
    ['enable_vm_tracing', 'ENABLE_VM_TRACING'],
    ['enable_vm_opcode_profiler', 'ENABLE_VM_OPCODE_PROFILER'],
    ['enable_spatial_grid_logs', 'ENABLE_SPATIAL_GRID_LOGS'],
    ['enable_vm_stub_logs', 'ENABLE_VM_STUB_LOGS']
]

foreach opt : vm_opts
    val = get_option(opt[0])
    if val == 'true' or (val == 'auto' and not is_console)
        add_project_arguments('-D' + opt[1], language: 'c')
    endif
endforeach

add_project_arguments(
    '-DBUTTERSCOTCH_COMMIT_HASH="' + get_option('commit_hash') + '"',
    '-DBUTTERSCOTCH_COMMIT_DATE="' + get_option('commit_date') + '"',
    language: 'c'
)

# ==============================================================================
# Environment Variables Resolution
# ==============================================================================
py3 = import('python').find_installation('python3')

ps2sdk_dir = run_command(py3, '-c', 'import os; print(os.environ.get("PS2SDK", ""))', check: false).stdout().strip()
gskit_dir  = run_command(py3, '-c', 'import os; print(os.environ.get("GSKIT", ""))', check: false).stdout().strip()
ps2dev_dir = run_command(py3, '-c', 'import os; print(os.environ.get("PS2DEV", ""))', check: false).stdout().strip()
ps3dev_dir = run_command(py3, '-c', 'import os; print(os.environ.get("PS3DEV", ""))', check: false).stdout().strip()

# ==============================================================================
# Source Lists
# ==============================================================================
butterscotch_src = []
butterscotch_inc = [] 
butterscotch_deps = []
butterscotch_link_args = []
butterscotch_c_args = []

butterscotch_inc += include_directories('src', 'vendor/stb/ds', 'vendor/stb/image', 'vendor/stb/vorbis')

# Base utilities
butterscotch_src += files(
    'vendor/md5/md5.c',
    'vendor/sha1/sha1.c',
    'vendor/base64/base64.c'
)
butterscotch_inc += include_directories('vendor/md5', 'vendor/sha1', 'vendor/base64')

# bzip2 compilation
bzip2_src = files(
    'vendor/bzip2/blocksort.c', 'vendor/bzip2/bzlib.c', 'vendor/bzip2/compress.c',
    'vendor/bzip2/crctable.c', 'vendor/bzip2/decompress.c', 'vendor/bzip2/huffman.c',
    'vendor/bzip2/randtable.c'
)
bzip2_lib = static_library('bzip2', bzip2_src, include_directories: 'vendor/bzip2')
butterscotch_deps += declare_dependency(link_with: bzip2_lib, include_directories: 'vendor/bzip2')

if plat == 'web-meta'
    # Meta specific sources
    butterscotch_src += files('src/data_win.c', 'src/binary_reader.c', 'src/web-meta/main.c', 'src/web-meta/stb_impl.c')
else
    # Standard Core Sources
    butterscotch_src += files(
        'src/binary_reader.c', 'src/data_win.c', 'src/data_win_print.c',
        'src/debug_overlay.c', 'src/event_table.c', 'src/gml_array.c',
        'src/gml_method.c', 'src/ini.c', 'src/input_recording.c',
        'src/instance.c', 'src/int_int_hashmap.c', 'src/int_rvalue_hashmap.c',
        'src/json_reader.c', 'src/json_writer.c', 'src/noop_audio_system.c',
        'src/noop_file_system.c', 'src/overlay_file_system.c', 'src/profiler.c',
        'src/runner.c', 'src/runner_gamepad.c', 'src/runner_keyboard.c',
        'src/runner_mouse.c', 'src/spatial_grid.c', 'src/string_builder.c',
        'src/vm.c', 'src/vm_builtins.c'
    )
endif

# ==============================================================================
# Audio Backend Switch
# ==============================================================================
if audio_backend == 'openal'
    butterscotch_src += files('src/audio/openal/al_audio_system.c', 'src/audio/openal/wave.c')
    butterscotch_inc += include_directories('src/audio/openal')
    butterscotch_c_args += ['-DUSE_OPENAL']
    
    if host_machine.system() == 'darwin'
        butterscotch_deps += dependency('appleframeworks', modules: ['OpenAL'])
    elif plat != 'ps3'
        butterscotch_deps += dependency('openal', required: true)
    endif
elif audio_backend == 'miniaudio'
    butterscotch_src += files('src/audio/miniaudio/ma_audio_system.c')
    butterscotch_inc += include_directories('vendor/miniaudio', 'src/audio/miniaudio')
    butterscotch_c_args += ['-DUSE_MINIAUDIO']
    butterscotch_deps += dependency('threads')
elif audio_backend == 'web'
    butterscotch_src += files('src/audio/web/web_audio_system.c', 'src/audio/miniaudio/ma_audio_system.c')
    butterscotch_inc += include_directories('vendor/miniaudio', 'src/audio/web', 'src/audio/miniaudio')
    butterscotch_c_args += ['-DUSE_WEB_AUDIO']
elif audio_backend == 'ps2'
    butterscotch_src += files('src/audio/ps2/ps2_audio_system.c')
    butterscotch_inc += include_directories('src/audio/ps2')
    butterscotch_c_args += ['-DUSE_PS2_AUDIO']
elif audio_backend != 'none'
    error('Invalid audio backend!')
endif

# ==============================================================================
# Platform Implementations
# ==============================================================================
m_dep = cc.find_library('m', required: false)
dl_dep = cc.find_library('dl', required: false)
if m_dep.found()
    butterscotch_deps += m_dep
endif

if plat == 'desktop'
    if not enable_legacy and not enable_modern
        error('You must enable at least one renderer!')
    endif

    butterscotch_src += files(
        'src/desktop/main.c', 
        'src/desktop/stb_impl.c',
        'src/image/image_decoder.c',
        'src/gl_common/gl_common.c'
    )
    butterscotch_inc += include_directories('src/image', 'src/gl_common')

    if enable_legacy
        butterscotch_src += files('src/gl_legacy/gl_legacy_renderer.c')
        butterscotch_inc += include_directories('src/gl_legacy')
    endif
    if enable_modern
        butterscotch_src += files('src/gl/gl_renderer.c')
        butterscotch_inc += include_directories('src/gl')
    endif

    # Glad
    if get_option('enable_gles')
        butterscotch_c_args += ['-DENABLE_GLES']
        glad_src = files('vendor/glad-gles/src/glad.c')
        glad_inc = include_directories('vendor/glad-gles/include')
    else
        glad_src = files('vendor/glad/src/glad.c')
        glad_inc = include_directories('vendor/glad/include')
    endif
    glad_lib = static_library('glad', glad_src, include_directories: glad_inc)
    butterscotch_deps += declare_dependency(link_with: glad_lib, include_directories: glad_inc)

    # OS logic
    if host_machine.system() == 'windows'
        butterscotch_deps += [
            cc.find_library('opengl32'),
            cc.find_library('gdi32'),
            cc.find_library('winmm')
        ]
    elif host_machine.system() != 'haiku' and dl_dep.found()
        butterscotch_deps += dl_dep
    endif

    # ASAN override block
    asan_opt = get_option('enable_asan')
    enable_asan = asan_opt == 'true' or (asan_opt == 'auto' and get_option('buildtype') == 'debug')
    if enable_asan and host_machine.system() != 'windows' and host_machine.system() != 'haiku'
        butterscotch_c_args += ['-fsanitize=address', '-fno-omit-frame-pointer']
        butterscotch_link_args += ['-fsanitize=address']
    endif

    # Desk Backends
    if desktop_backend == 'glfw3' or desktop_backend == 'auto'
        butterscotch_c_args += ['-DUSE_GLFW3']
        butterscotch_deps += dependency('glfw3', fallback: ['glfw', 'glfw_dep'], required: true, default_options: ['default_library=static'])
        butterscotch_src += files('src/desktop/backends/glfw3.c')
        if host_machine.system() == 'windows'
            butterscotch_c_args += ['-D_WIN32_WINNT=0x0601', '-DNTDDI_VERSION=0x06010000', '-DWIN32_LEAN_AND_MEAN']
            butterscotch_link_args += ['-static']
        endif
        configure_file(input: 'vendor/gamecontrollerdb.txt', output: 'gamecontrollerdb.txt', copy: true)
    elif desktop_backend == 'glfw2'
        butterscotch_c_args += ['-DUSE_GLFW2']
        butterscotch_deps += dependency('libglfw', required: true)
        butterscotch_src += files('src/desktop/backends/glfw2.c')
    elif desktop_backend == 'sdl1'
        butterscotch_c_args += ['-DUSE_SDL1']
        butterscotch_deps += dependency('sdl', required: true)
        butterscotch_src += files('src/desktop/backends/sdl1.c')
    endif

elif plat == 'web'
    if not enable_modern
        error('Web requires modern gl!')
    endif
    
    butterscotch_src += files(
        'src/web/main.c',
        'src/web/stb_impl.c',
        'src/image/image_decoder.c',
        'src/gl_common/gl_common.c',
        'src/gl/gl_renderer.c'
    )
    butterscotch_inc += include_directories('src/image', 'src/gl_common', 'src/gl')
    butterscotch_c_args += ['-DENABLE_GLES', '-pthread']
    
    butterscotch_link_args += [
        '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=worker', '-sWASMFS=1',
        '-sEXPORTED_RUNTIME_METHODS=["HEAP8", "HEAPU8", "HEAPF32", "ccall", "cwrap", "specialHTMLTargets", "UTF8ToString"]',
        '-sEXPORTED_FUNCTIONS=["_main", "_mountOpfs", "_startRunner", "_stopRunner", "_malloc", "_free", "_getKeyDownPtr", "_getKeyUpPtr", "_getKeyCount", "_setAudioSampleRate", "_pullAudioFrames"]',
        '-sOFFSCREENCANVAS_SUPPORT=1', '-sOFFSCREEN_FRAMEBUFFER=1',
        '-sMIN_WEBGL_VERSION=2', '-sMAX_WEBGL_VERSION=2', '-sFULL_ES3=1',
        '-sALLOW_MEMORY_GROWTH', '-sSHARED_MEMORY=1', '-sPTHREAD_POOL_SIZE=4', '-pthread'
    ]

elif plat == 'web-meta'
    butterscotch_link_args += [
        '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=web',
        '-sEXPORTED_RUNTIME_METHODS=["HEAPU8", "ccall", "cwrap", "UTF8ToString", "FS"]',
        '-sEXPORTED_FUNCTIONS=["_main", "_parseDataWin", "_freeDataWin", "_getGameName", "_getGameDisplayName", "_getMajorVersion", "_getMinorVersion", "_getRelease", "_getBuild", "_getDefaultWindowWidth", "_getDefaultWindowHeight", "_malloc", "_free"]',
        '-sALLOW_MEMORY_GROWTH=1', '-sFORCE_FILESYSTEM=1'
    ]

elif plat == 'ps2'
    butterscotch_src += files(
        'src/ps2/main.c', 'src/ps2/stb_impl.c', 'src/ps2/debug_font_renderer.c',
        'src/ps2/gs_renderer.c', 'src/ps2/ps2_file_system.c', 'src/ps2/ps2_gamepad.c',
        'src/ps2/ps2_overlay.c', 'src/ps2/ps2_utils.c',
        'src/debug_font/debug_font.c', 'src/debug_font/debug_font_4bpp.c'
    )
    butterscotch_inc += include_directories('src/debug_font')

    butterscotch_c_args += [
        '-DUSE_FLOAT_REALS', '-DNO_RVALUE_INT64', '-DPLATFORM_PS2',
        '-fsingle-precision-constant', '-Werror=double-promotion',
        '-fomit-frame-pointer', '-ffast-math'
    ]

    if get_option('enable_ps2_renderer_logs')
        butterscotch_c_args += ['-DENABLE_PS2_RENDERER_LOGS']
    endif

    if ps2sdk_dir != ''
        butterscotch_inc += include_directories(
            ps2sdk_dir / 'ee/include', ps2sdk_dir / 'common/include',
            ps2dev_dir / 'ee/include', gskit_dir / 'include', is_system: true
        )
        butterscotch_link_args += ['-L' + ps2sdk_dir / 'ee/lib', '-L' + gskit_dir / 'lib']
    endif
    
    # Required static libs from PS2 SDK
    foreach lib : ['gskit', 'dmakit', 'audsrv', 'pad', 'mc', 'kbd', 'patches', 'kernel', 'c', 'm']
        butterscotch_deps += cc.find_library(lib, required: false)
    endforeach

    # Embed ps2sdk IRX modules via bin2c
    bin2c = find_program(ps2sdk_dir / 'bin/bin2c', required: false)
    if bin2c.found()
        irx_modules = ['freesio2', 'mcman', 'mcserv', 'freepad', 'usbd', 'ps2kbd']
        if audio_backend == 'ps2'
            irx_modules += ['freesd', 'audsrv']
        endif
        
        if get_option('enable_gprof_profiling')
            butterscotch_c_args += ['-DGPROF_PROFILING', '-pg']
            butterscotch_link_args += ['-pg']
            irx_modules += ['bdm', 'bdmfs_fatfs', 'usbmass_bd']
        endif

        foreach mod : irx_modules
            irx_name = mod + '_irx'
            irx_c = custom_target(
                irx_name + '_c',
                input: ps2sdk_dir / 'iop/irx' / mod + '.irx',
                output: irx_name + '.c',
                command: [bin2c, '@INPUT@', '@OUTPUT@', irx_name]
            )
            butterscotch_src += irx_c
        endforeach
    endif

elif plat == 'ps3'
    if not enable_legacy
        error('PS3 requires legacy gl!')
    endif
    
    butterscotch_src += files(
        'src/ps3/main.c', 'src/ps3/stb_impl.c', 'src/ps3/paletted_fpo.c',
        'src/ps3/ps3_overlay.c', 'src/ps3/ps3_textures.c',
        'src/gl_common/gl_common.c', 'src/gl_legacy/gl_legacy_renderer.c',
        'src/image/image_decoder.c', 'vendor/ps3gl/ffp_shader_fpo.c',
        'vendor/ps3gl/ffp_shader_vpo.c', 'vendor/ps3gl/ps3gl.c',
        'vendor/ps3gl/rsxutil.c', 'vendor/mojoal/mojoal.c',
        'src/debug_font/debug_font.c', 'src/debug_font/debug_font_4bpp.c'
    )
    butterscotch_inc += include_directories(
        'src/gl_legacy', 'src/image', 'src/debug_font',
        'vendor/ps3gl', 'vendor/mojoal', 'src/ps3', 'src/gl_common'
    )

    butterscotch_c_args += ['-DPLATFORM_PS3', '-DIS_BIG_ENDIAN', '-DAL_LIBTYPE_STATIC']
    
    if ps3dev_dir != ''
        butterscotch_link_args += [
            '-mhard-float', '-fmodulo-sched', '-Wl,--gc-sections',
            '-L' + ps3dev_dir / 'ppu/lib', '-L' + ps3dev_dir / 'portlibs/ppu/lib'
        ]
    endif
    
    butterscotch_deps += dependency('sdl2', required: true)
    
    foreach lib : ['gcm_sys', 'rsx', 'sysutil', 'io', 'audio', 'rt', 'lv2']
        butterscotch_deps += cc.find_library(lib, required: false)
    endforeach
endif

# ==============================================================================
# Final Target Generation
# ==============================================================================
exe_name = (plat == 'web-meta') ? 'butterscotch-meta' : 'butterscotch'
suffix = (plat == 'web' or plat == 'web-meta') ? 'mjs' : []

executable(
    exe_name,
    butterscotch_src,
    include_directories: butterscotch_inc,
    dependencies: butterscotch_deps,
    c_args: butterscotch_c_args,
    link_args: butterscotch_link_args,
    name_suffix: suffix,
    install: true
)
