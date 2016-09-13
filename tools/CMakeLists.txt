add_executable (secho secho.cc)
target_link_libraries (secho empcore)

add_executable (dump dump.cc)
target_link_libraries (dump empcore)

add_executable (reqack reqack.cc)
target_link_libraries (reqack empcore)

