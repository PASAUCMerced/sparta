examples_c_srcs = run_command('find', meson.current_source_dir(), '-maxdepth', '1', '-name', '*.c', check : true).stdout().strip().split('\n')
examples_cuda_srcs = run_command('find', meson.current_source_dir(), '-maxdepth', '1', '-name', '*.cu', check : true).stdout().strip().split('\n')

foreach f : examples_c_srcs
    base_name = run_command('basename', f, '.c').stdout().strip()
    executable(base_name, f, dependencies : parti_dep)
endforeach

if get_option('use_cuda')
    foreach f : examples_cuda_srcs
        base_name = run_command('basename', f, '.cu').stdout().strip()
        obj = nvcc_gen.process(f,
            extra_args : cuda_args,
            preserve_path_from : meson.current_source_dir())
        executable(base_name, obj, dependencies : parti_dep)
    endforeach
endif
