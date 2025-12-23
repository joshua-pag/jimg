project('jimg', 'c')

incdir = include_directories('include')

add_project_link_arguments('-lm', language : 'c')
add_project_link_arguments('-lSDL3', language : 'c')
executable('jimg', 'jimg.c', include_directories : incdir)
