set(_FPTRU_OBJS "")

if(OQS_ENABLE_KEM_fptru_761)
    # 添加FPTRU-761 C语言源文件
    add_library(fptru_761 OBJECT
        kem_fptru_761.c
    )
    
    # 包含头文件目录
    target_include_directories(fptru_761 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU
        ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU_c_batch_ref
        ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU_ref
    )
    
    set(_FPTRU_OBJS ${_FPTRU_OBJS} $<TARGET_OBJECTS:fptru_761>)
endif()

if(OQS_ENABLE_KEM_fptru_761_ref_batch)
    add_library(fptru_761_ref_batch OBJECT kem_fptru_761.c FPTRU_c_batch_ref/poly_mul_n761q/radix_ntt_n761.c FPTRU_c_batch_ref/cbd.c FPTRU_c_batch_ref/coding.c FPTRU_c_batch_ref/cpucycles.c FPTRU_c_batch_ref/fips202.c FPTRU_c_batch_ref/inverse.c FPTRU_c_batch_ref/kem.c FPTRU_c_batch_ref/pack.c FPTRU_c_batch_ref/pke.c FPTRU_c_batch_ref/poly.c FPTRU_c_batch_ref/randombytes.c FPTRU_c_batch_ref/reduce.c FPTRU_c_batch_ref/speed.c FPTRU_c_batch_ref/batchkeygen.c) #这里不能够有斜杠
    target_compile_options(fptru_761_ref_batch PUBLIC -DFPTRU_N=761)
    target_include_directories(fptru_761_ref_batch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU_c_batch_ref/)
    target_include_directories(fptru_761_ref_batch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU_c_batch_ref/poly_mul_n761q)
    set(_FPTRU_OBJS ${_FPTRU_OBJS} $<TARGET_OBJECTS:fptru_761_ref_batch>)
endif()

if(OQS_ENABLE_KEM_fptru_761_ref)
    add_library(fptru_761_ref OBJECT kem_fptru_761.c FPTRU_ref/poly_mul_n761q/radix_ntt_n761.c FPTRU_ref/cbd.c FPTRU_ref/coding.c FPTRU_ref/cpucycles.c FPTRU_ref/fips202.c FPTRU_ref/inverse.c FPTRU_ref/kem.c FPTRU_ref/pack.c FPTRU_ref/pke.c FPTRU_ref/poly.c FPTRU_ref/randombytes.c FPTRU_ref/reduce.c FPTRU_ref/speed.c) #这里不能够有斜杠
    target_compile_options(fptru_761_ref PUBLIC -DFPTRU_N=761)
    target_include_directories(fptru_761_ref PRIVATE ${CMAKE_CURRENT_LIST_DIR}/FPTRU_ref)
    target_include_directories(fptru_761_ref PRIVATE ${CMAKE_CURRENT_LIST_DIR}/FPTRU_ref/poly_mul_n761q)
    set(_FPTRU_OBJS ${_FPTRU_OBJS} $<TARGET_OBJECTS:fptru_761_ref>)
endif()

if(OQS_ENABLE_KEM_fptru_761_cuda)
    # 添加FPTRU-761 CUDA源文件
    add_library(fptru_761_cuda OBJECT
        kem_fptru_761_cuda.cu
        FPTRU/cbd.cu
        FPTRU/coding.cu
        FPTRU/kem.cu
        FPTRU/kernel.cu
        FPTRU/pack.cu
        FPTRU/poly.cu
        FPTRU/random.cu
        FPTRU/reduce.cu
        FPTRU/tensor.cu
        FPTRU/main.cu
        FPTRU/fips202_ws/device.cu
        FPTRU/fips202_ws/global.cu
        FPTRU/poly_mul_n761q/n761.cu
        FPTRU/poly_mul_n653q/n653.cu
        FPTRU/poly_mul_n1277q/n1277.cu
    )
    
    set_property(TARGET fptru_761_cuda PROPERTY CUDA_ARCHITECTURES OFF)
    target_compile_options(fptru_761_cuda PRIVATE 
        $<$<COMPILE_LANGUAGE:CUDA>:-std=c++17>
        $<$<COMPILE_LANGUAGE:CUDA>:-arch=compute_86 -code=sm_86>
        $<$<COMPILE_LANGUAGE:CUDA>:-rdc=true>
        $<$<COMPILE_LANGUAGE:CUDA>:-lcuda -lnvToolsExt>
        $<$<COMPILE_LANGUAGE:CUDA>:-prec-div=false>
        $<$<COMPILE_LANGUAGE:CUDA>:-O3>
    )
    target_compile_definitions(fptru_761_cuda PRIVATE FPTRU_N=761 NEW=1)
    
    # 包含头文件目录
    target_include_directories(fptru_761_cuda PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU
        ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU/fips202_ws
        ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU/poly_mul_n653q
        ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU/poly_mul_n761q
        ${CMAKE_CURRENT_SOURCE_DIR}/FPTRU/poly_mul_n1277q
    )
    
   set(_FPTRU_OBJS ${_FPTRU_OBJS} $<TARGET_OBJECTS:fptru_761_cuda>)
endif()
set(FPTRU_OBJS ${_FPTRU_OBJS} PARENT_SCOPE)