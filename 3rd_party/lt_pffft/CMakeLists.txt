project(lt_pffft)

add_library(${PROJECT_NAME})
add_library(lt::lt_pffft ALIAS ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC PFFFT_ENABLE_DOUBLE=1 PFFFT_ENABLE_FLOAT=1)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(${PROJECT_NAME}
    PRIVATE
        pffft.c
        pffft.h

        pffft_common.c
        pffft_priv_impl.h
        pffft.hpp

        simd/pf_float.h
        simd/pf_sse1_float.h
        simd/pf_altivec_float.h
        simd/pf_neon_float.h
        simd/pf_scalar_float.h

        pffft_double.c
        pffft_double.h
        simd/pf_double.h
        simd/pf_avx_double.h
        simd/pf_scalar_double.h
)

