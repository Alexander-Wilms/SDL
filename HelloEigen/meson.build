project('mybuildtarget','cpp')

incdir = include_directories('/usr/include/eigen3')

run_target('data',
	command : ['scripts/copy-data.sh'],
	build_by_default : true)

executable('mybuildtarget',
	'src/main.cpp',
	include_directories : incdir,
	install : true)
