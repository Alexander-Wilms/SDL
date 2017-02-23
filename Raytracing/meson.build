project('mybuildtarget','cpp')

sdl2 = dependency('sdl2')

incdir = include_directories('/usr/include/eigen3')

run_target('data',
	command : ['scripts/copy-data.sh'],
	build_by_default : true)

executable('mybuildtarget',
	['src/main.cpp', 'src/Display.h', 'src/Display.cpp'],
	include_directories : incdir,
	dependencies: sdl2,
	install : true)
