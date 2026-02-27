include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG v12.1.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)