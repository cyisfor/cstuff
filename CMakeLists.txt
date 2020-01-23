cmake_minimum_required(VERSION 3.14)
project(cstuff VERSION 0.1)

define_property(TARGET PROPERTY cstuff_source_dir
  BRIEF_DOCS where the f-ck cstuff is
  FULL_DOCS where the f-ck cstuff is)

define_property(TARGET PROPERTY cstuff_sources
  BRIEF_DOCS the sources already added to cstuff regardless of absolute path
  FULL_DOCS cmake is stupid, git is stupid, so cstuff might be a submodule and a submodule of a submodule, so it has to remember sources by their unresolved name to avoid multiple definition errors)

if(NOT TARGET cstuff)
#message(WARNING "Creating the cstuff target, yay!")
file(WRITE "${CMAKE_BINARY_DIR}/cmakesucks.c" "")
add_library(cstuff "${CMAKE_BINARY_DIR}/cmakesucks.c")
target_include_directories(cstuff PUBLIC "${cstuff_source_dir}")
endif()

set_property(TARGET cstuff PROPERTY cstuff_source_dir "${CMAKE_CURRENT_SOURCE_DIR}")

function(add_cstuff)
  get_property(cstuff_source_dir TARGET cstuff PROPERTY cstuff_source_dir)
  set(options itoa mmapfile pat aton become record ensure)
  cmake_parse_arguments(PARSE_ARGV 0 want "${options}" "" "")

  function(sourcederp)
	set(sources)
	get_target_property(cstuff_sources cstuff cstuff_sources)
	if(cstuff_sources STREQUAL "cstuff_sources-NOTFOUND")
	  set(cstuff_sources)
	endif()
	foreach(arg IN LISTS ARGV)
	  list(FIND cstuff_sources "${arg}" already_got)
	  if(already_got EQUAL -1)
		#message(WARNING "adding source ${arg} : ${cstuff_sources}")
		list(APPEND cstuff_sources "${arg}")
		#message(WARNING "added source ${arg} : ${cstuff_sources}")		
		get_filename_component(aarg "${arg}" ABSOLUTE
		  BASE_DIR "${cstuff_source_dir}")
		#message("AUGH ${cstuff_source_dir} ${aarg}")
		list(APPEND sources ${aarg})
	  else(already_got EQUAL -1)
		#message(WARNING "already got source ${arg} : ${cstuff_sources}")
	  endif(already_got EQUAL -1)
	endforeach()
	#message("SOURCSET ${sources}")
	if(sources)
	  set_property(TARGET cstuff
		PROPERTY cstuff_sources ${cstuff_sources})
	  target_sources(cstuff PRIVATE ${sources})
	endif(sources)
  endfunction()

  if(want_become)
	sourcederp(become.c)
  endif()
  if(want_record OR want_ensure)
	sourcederp(record.c itoa.c)
  endif()

  if(want_itoa)
	sourcederp(itoa.c)
  endif(want_itoa)
  if(want_aton)
	sourcederp(aton.c)
  endif(want_aton)  
  if(want_mmapfile)
	include(CheckSymbolExists)
	check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
	if(HAVE_MMAP)
	  sourcederp(mmapfile.c)
	else(HAVE_MMAP)
	  message(FATAL_ERROR "We don't seem to have mmap")
	endif(HAVE_MMAP)
  endif(want_mmapfile)
  if(want_pat)
	include(CheckLibraryExists)
	check_library_exists(pcre pcre_jit_exec /usr/lib HAVE_PCRE)
	if(HAVE_PCRE)
	  sourcederp(pat.c)
	  target_link_libraries(cstuff PUBLIC pcre)
	else(HAVE_PCRE)
	  message(FATAL_ERROR "Couldn't find the PCRE library")
	endif(HAVE_PCRE)
	# for g_slice_etc... should we just use malloc?
	include(FindPkgConfig)
	pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
	target_link_libraries(cstuff PUBLIC PkgConfig::GLIB)
  endif(want_pat)
  if(want_path)
	include(FindPkgConfig)
	pkg_check_modules(GLIB
	  REQUIRED IMPORTED_TARGET
	  glib-2.0)
	sourcederp(path.c)
	target_link_libraries(cstuff PUBLIC PkgConfig::GLIB)
  endif(want_path)
endfunction(add_cstuff)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  enable_testing()
  function (my_tests)
	foreach(name IN LISTS ARGV)
	  add_cstuff(${name})
	  add_executable(test_${name}
		test/test_${name}.c)
	  target_link_libraries(test_${name} PRIVATE cstuff)
	  add_test(${name} test_${name})
	endforeach(name)
  endfunction(my_test)
  my_tests(
	record
	pat
	aton
	itoa)
endif()
