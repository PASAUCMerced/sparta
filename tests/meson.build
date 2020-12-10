tests_c_srcs = run_command('find', '.', '-maxdepth', '1', '-name', '*.c', check : true).stdout().strip().split('\n')
tests_cuda_srcs = run_command('find', '.', '-maxdepth', '1', '-name', '*.cu', check : true).stdout().strip().split('\n')

foreach f : tests_c_srcs
    base_name = run_command('basename', f, '.c').stdout().strip()
    exe = executable(base_name, f, dependencies : parti_dep)
    test(base_name, exe)
endforeach

if get_option('use_cuda')
    #foreach f : tests_cuda_srcs
    #    base_name = run_command('basename', f, '.cu').stdout().strip()
    #    obj = nvcc_gen.process(f,
    #        extra_args : cuda_args,
    #        preserve_path_from : meson.current_source_dir())
    #    exe = executable(base_name, obj, dependencies : parti_dep)
    #    test(base_name, exe)
    #endforeach
endif
