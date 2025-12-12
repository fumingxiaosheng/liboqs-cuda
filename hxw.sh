# #使用一般的方式
# cd /home/hxw/TLS/liboqs-0.13.0/build_ref && cmake .. -DOQS_USE_CUPQC=OFF -DOQS_DIST_BUILD=OFF -DOQS_OPT_TARGET=generic
# make -j4

# #cuda方式
# cd /home/hxw/TLS/liboqs-0.13.0/build_cuda && cmake .. -DOQS_USE_CUPQC=ON -DOQS_USE_AVX2_INSTRUCTIONS=OFF -DOQS_DIST_X86_64_BUILD=OFF -DCMAKE_PREFIX_PATH=/home/hxw/TLS/cupqc/cupqc-pkg-0.2.0
# make -j4

#批处理cuda方式
rm -r /home/hxw/TLS/liboqs-0.13.0/build_fptru
mkdir build_fptru
cd /home/hxw/TLS/liboqs-0.13.0/build_fptru
cmake .. -DOQS_USE_CUPQC=ON -DOQS_USE_AVX2_INSTRUCTIONS=OFF -DOQS_DIST_X86_64_BUILD=OFF -DCMAKE_PREFIX_PATH=/home/hxw/TLS/cupqc/cupqc-pkg-0.2.0 -DOQS_ENABLE_KEM_fptru_761_cuda=ON -DOQS_ENABLE_KEM_fptru_761_ref_batch=ON -DOQS_ENABLE_KEM_fptru_761_ref=OFF -DOQS_ENABLE_KEM_ntruprime_sntrup761_avx2=ON
make -j4