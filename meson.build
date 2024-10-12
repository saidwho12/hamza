project('hamza', 'c',
  version : '0.4.0',
  license : ['MIT'],
  meson_version : '>= 1.2.0',
  default_options : [
    'c_std=c18',
    'buildtype=debugoptimized',
    'prefer_static=true' # prefer static linking
  ])

cc = meson.get_compiler('c')
src = ['hz/hz.c']
args = []
ldargs = []

args += '-DHZ_ENDIAN_LITTLE=@0@'.format([ host_machine.endian() == 'little' ? '1' : '0' ])

deps = []
ocl = dependency('opencl-headers', required : false)
if get_option('opencl-render') and ocl.found()
  deps += ocl
endif

if get_option('build-tiny')
  # strip unused code from the binary
  args += [
    '-Oz', '-flto',
    '-ffunction-sections', '-fdata-sections' # Give functions and data unique sections which allows linker to then cull unused code
  ]

  if cc.get_linker_id() == 'lld-link' or cc.get_linker_id() == 'link'
    ldargs += ['-Wl,/OPT:REF', '-Wl,/VERBOSE']
  else
    ldargs += ['-Wl,--gc-sections', '-Wl,--print-gc-sections', '-Gs999999'] # increase stack commit size
  endif
endif

if get_option('crt-less')
  args += [
    '-DHZ_NOSTDLIB=1',
    '-nostdlib','-nodefaultlibs',  
    '-fno-builtin', # tells compiler not to include memcpy, memmove or memset implicitly
    '-ffast-math', '-fno-math-errno', # Remove math error checks
    '-fdelete-null-pointer-checks', # This will remove any null pointer checks, usually, this is enabled by default with any -O but not embedded
    '-fno-stack-protector',
    '-fno-threadsafe-statics' # Disable some safety checks for threads, this will help even in the case where we don't use any threading.
  ]

  if cc.get_linker_id() == 'lld-link' or cc.get_linker_id() == 'link'
    src += ['hz/hz_msvc_rt_less.c']
    ldargs += ['-Wl,/nodefaultlib']
  endif
endif

if host_machine.system() == 'windows'
elif host_machine.system() == 'linux' or host_machine.system() == 'macos'
  src += ['hz/hz_linux.c']
endif


windows = import('windows')
if get_option('build-shared') and host_machine.system() == 'windows'
  src += windows.compile_resources('./resources/manifest.rc')
  args += ['-DHZ_BUILD_SHARED=1']
  ldargs += ['-Wl,/incremental:no', '-Wl,/manifest:embed']
else
  args += ['-DHZ_BUILD_SHARED=0']
endif

if get_option('build-shared')
  hamza = shared_library('hamza', src, link_language: 'c', version: '0.4.0', soversion: '0', c_args: args, link_args: ldargs, override_options: ['c_winlibs='], dependencies: deps, install: false)
  # hamza = shared_library('hamza', src, link_language: 'c', version: '0.4.0', soversion: '0', c_args: args, link_args: ldargs, install: false)
else
  hamza = static_library('hamza', src, link_language: 'c', c_args: args, override_options: ['c_winlibs='], dependencies: deps, install: false)
endif

