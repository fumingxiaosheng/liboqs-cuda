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
    )
    
    set(_FPTRU_OBJS ${_FPTRU_OBJS} $<TARGET_OBJECTS:fptru_761>)
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