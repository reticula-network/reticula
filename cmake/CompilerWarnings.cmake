function(enable_project_warnings tgt)
  if (NOT RETICULA_ENABLE_WARNINGS)
    return()
  endif()

  if (MSVC)
    target_compile_options(${tgt} PRIVATE
      /bigobj /W4 /permissive-
      /w14242 /w14254 /w14263 /w14265 /w14287
      /we4289 /w14296 /w14311 /w14545 /w14546
      /w14547 /w14549 /w14555 /w14619 /w14640
      /w14826 /w14905 /w14906 /w14928
    )
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(${tgt} PRIVATE
      -Werror -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic
      -Wnull-dereference -Wold-style-cast -Wcast-align -Wunused
      -Woverloaded-virtual -Wimplicit-fallthrough -Wmisleading-indentation
      $<$<CXX_COMPILER_ID:GNU>:
        -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast>
    )
  endif()
endfunction()
