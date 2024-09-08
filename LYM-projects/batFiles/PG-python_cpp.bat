echo off

cd C:\home\LYM-projects\
python C:\home\LYM-projects\Projects\cmd\PG_cpp_source_generator.py  --header ..\LYM-sources\Porphyrograph-src\include\pg-header.h --full_scenario Projects\scenarios\%3 --script_header_out ..\LYM-sources\Porphyrograph-src\include\pg_script_header_Core.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Core.cpp  --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Core.cpp --render_body_out ..\LYM-sources\Porphyrograph-src\pg_render_body_Core.cpp
