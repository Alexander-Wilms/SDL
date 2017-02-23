project('mybuildtarget','cpp')

sdl2 = dependency('sdl2')

run_target('data',
	command : ['scripts/copy-data.sh'],
	build_by_default : true)

executable('mybuildtarget',
	'src/main.cpp',
	dependencies: sdl2,
	install : true)
