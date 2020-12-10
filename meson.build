# For all options, see meson_options.txt
project('ParTI', 'c', 'cpp',
    default_options : ['buildtype=debugoptimized', 'c_std=c99', 'cpp_std=c++03'])

# Guess CUDA_HOME if specified from the environment
cuda_home = run_command('sh', '-c', 'echo "$CUDA_HOME"', check : true).stdout().strip()
if cuda_home == ''
    cuda_home = '/usr/local/cuda'
endif
message('CUDA_HOME: ' + cuda_home)

# NVCC compiler is declared as a 'generator' to generate .o files from .cu files
nvcc = find_program(join_paths(cuda_home, 'bin/nvcc'), 'nvcc', required : get_option('use_cuda'))
nvcc_gen = generator(nvcc, output : '@PLAINNAME@.o', arguments : ['-c', '-o', '@OUTPUT@', '@INPUT@', '@EXTRA_ARGS@'])

# Although extremely not recommended by Meson developers, I will just want to be lazy so just glob the files
c_srcs = run_command('find', 'src', '-name', '*.c', '-not', '-path', '*/backup/*', check : true).stdout().strip().split('\n')
cuda_srcs = run_command('find', 'src', '-name', '*.cu', '-not', '-path', '*/backup/*', check : true).stdout().strip().split('\n')

inc = include_directories('include')

# c_args is eventually managed by Meson automatically
c_args = ['-Wextra', '-O3']
c_args += '-DPARTI_INDEX_TYPEWIDTH=32'
c_args += '-DPARTI_VALUE_TYPEWIDTH=32'
c_args += '-DPARTI_ELEMENT_INDEX_TYPEWIDTH=8'

# cuda_args is not managed by Meson, it will be passed to nvcc_gen manually
cuda_args = ['-std=c++03', '-Xcompiler', '-fPIC']
if get_option('buildtype') == 'debug'
    cuda_args += '-g'
elif get_option('buildtype') == 'debugoptimized'
    cuda_args += ['-O3', '-g']
elif get_option('buildtype') == 'release'
    cuda_args += ['-O3', '-Xcompiler', '-s']
elif get_option('buildtype') == 'minsize'
    cuda_args += ['-Xcompiler', '-Os', '-Xcompiler', '-s']
endif
cuda_args += '-I' + join_paths(meson.current_source_dir(), 'include')
cuda_args += '-D_BITS_FLOATN_H'  # Workaround __float128 bug
cuda_args += '-DPARTI_INDEX_TYPEWIDTH=32'
cuda_args += '-DPARTI_VALUE_TYPEWIDTH=32'
cuda_args += '-DPARTI_ELEMENT_INDEX_TYPEWIDTH=8'

# Give the user multiple ways to append arguments
cuda_args += run_command('sh', '-c', 'echo -n "$NVCCFLAGS"', check : true).stdout().split()
cuda_args += get_option('cuda_args')

# Find all the 3rd-party dependencies
libdl_dep = meson.get_compiler('c').find_library('dl', required : false)
libm_dep = meson.get_compiler('c').find_library('m', required : false)
librt_dep = meson.get_compiler('c').find_library('rt', required : false)
cudart_dep = meson.get_compiler('cpp').find_library('cudart_static',
    dirs : [join_paths(cuda_home, 'lib64'), join_paths(cuda_home, 'lib')],
    required : get_option('use_cuda'))
culibos_dep = meson.get_compiler('cpp').find_library('culibos',
    dirs : [join_paths(cuda_home, 'lib64'), join_paths(cuda_home, 'lib')],
    required : get_option('use_cuda'))
cublas_dep = meson.get_compiler('cpp').find_library('cublas_static',
    dirs : [join_paths(cuda_home, 'lib64'), join_paths(cuda_home, 'lib')],
    required : get_option('use_cuda'))
cusolver_dep = meson.get_compiler('cpp').find_library('cusolver_static',
    dirs : [join_paths(cuda_home, 'lib64'), join_paths(cuda_home, 'lib')],
    required : get_option('use_cuda'))
cusparse_dep = meson.get_compiler('cpp').find_library('cusparse_static',
    dirs : [join_paths(cuda_home, 'lib64'), join_paths(cuda_home, 'lib')],
    required : get_option('use_cuda'))
if build_machine.system() == 'darwin' and meson.get_compiler('c').get_id() == 'clang'
    openmp_lib = meson.get_compiler('c').find_library('omp', required : get_option('use_openmp'))
    if openmp_lib.found()
        openmp_dep = declare_dependency(
            compile_args : ['-Xpreprocessor', '-fopenmp'],
            dependencies : openmp_lib)
    else
        openmp_dep = disabler()
    endif
else
    openmp_dep = dependency('openmp', required : get_option('use_openmp'))
endif
blas_dep = dependency('blas', required : false)
lapack_dep = dependency('lapack', required : false)
openblas_dep = dependency('openblas', required : false)
magma_dep = dependency('magma', required : false)

# deps will be all the effective dependencies
deps = [libm_dep]

if get_option('use_cuda') and cudart_dep.found()
    c_args += '-DPARTI_USE_CUDA'
    cuda_args += '-DPARTI_USE_CUDA'
    deps += [cudart_dep, culibos_dep, cublas_dep, cusolver_dep, cusparse_dep, libdl_dep, librt_dep]
endif

if get_option('use_openmp') and openmp_dep.found()
    c_args += '-DPARTI_USE_OPENMP'
    cuda_args += '-DPARTI_USE_OPENMP'
    deps += openmp_dep
endif

if get_option('use_openblas') and openblas_dep.found()
    c_args += ['-DPARTI_USE_OPENBLAS', '-DPARTI_USE_LAPACK']
    cuda_args += ['-DPARTI_USE_OPENBLAS', '-DPARTI_USE_LAPACK']
    deps += openblas_dep
elif get_option('use_magma') and magma_dep.found()
    c_args += ['-DPARTI_USE_MAGMA', '-DPARTI_USE_LAPACK']
    cuda_args += ['-DPARTI_USE_MAGMA', '-DPARTI_USE_LAPACK']
    deps += magma_dep
elif blas_dep.found() and lapack_dep.found()
    c_args += '-DPARTI_USE_LAPACK'
    cuda_args += '-DPARTI_USE_LAPACK'
    deps += [blas_dep, lapack_dep]
else
    error('Unable to find a library providing BLAS/LAPACK routines')
endif

# Use the generator to compile CUDA source code
cuda_objs = []
if get_option('use_cuda')
    foreach f : cuda_srcs
        cuda_objs += nvcc_gen.process(f,
            extra_args : cuda_args,
            preserve_path_from : meson.current_source_dir())
    endforeach
endif

# Add all sources and objects to our libraries of both static and shared
parti = both_libraries('ParTI', c_srcs, cuda_objs,
    include_directories : inc,
    dependencies : deps,
    c_args : c_args)
parti_dep = declare_dependency(include_directories : inc,
    dependencies : deps,
    link_with : parti)

subdir('benchmark')
subdir('examples')

if get_option('build_tests')
    subdir('tests')
endif
