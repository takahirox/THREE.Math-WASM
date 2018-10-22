#/bin/bash

emcc -O2 -o ./Math-wasm.js ./Math.c --memory-init-file 0 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS="['_Object3D_init', '_Object3D_updateMatrixWorld', '_getObjectSize']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"

