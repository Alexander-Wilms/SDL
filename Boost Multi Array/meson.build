project('mybuildtarget','cpp')

boost_dep = dependency('boost', modules : 'multi_array')

executable('mybuildtarget',
	'src/main.cpp',
	dependencies: boost_dep,
	install : true)
