project('mybuildtarget','cpp')

sdl2 = dependency('sdl2')

cc = meson.get_compiler('cpp')
sdl2_image = cc.find_library('SDL2_image')

run_target('data',
	command : ['scripts/copy-data.sh'],
	build_by_default : true)

executable('mybuildtarget',
	'src/main.cpp',
	dependencies: [sdl2, sdl2_image],
	install : true)
