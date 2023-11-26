# evmone: Fast Ethereum Virtual Machine implementation
# Copyright 2023 The evmone Authors.
# SPDX-License-Identifier: Apache-2.0

add_library(evmone_precompiles STATIC)
add_library(evmone::precompiles ALIAS evmone_precompiles)
target_link_libraries(evmone_precompiles PUBLIC evmc::evmc_cpp PRIVATE evmone::evmmax)
target_sources(
    evmone_precompiles PRIVATE
        ../../include/evmone/precompiles/bn254.hpp
        bn254.cpp
        ../../include/evmone/precompiles/ecc.hpp
        secp256k1.cpp
        ../../include/evmone/precompiles/secp256k1.hpp
)
