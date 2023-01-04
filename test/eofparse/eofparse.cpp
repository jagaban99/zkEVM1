// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2023 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include <evmc/evmc.hpp>
#include <evmone/eof.hpp>
#include <iostream>
#include <string>

namespace
{
inline constexpr bool isalnum(char ch) noexcept
{
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

template <typename BaseIterator>
struct skip_nonalnum_iterator : evmc::filter_iterator<BaseIterator, isalnum>
{
    using evmc::filter_iterator<BaseIterator, isalnum>::filter_iterator;
};

template <typename BaseIterator>
skip_nonalnum_iterator(BaseIterator, BaseIterator) -> skip_nonalnum_iterator<BaseIterator>;

template <typename InputIterator>
std::optional<evmc::bytes> from_hex2(InputIterator begin, InputIterator end) noexcept
{
    evmc::bytes bs;
    if (!from_hex(skip_nonalnum_iterator{begin, end}, skip_nonalnum_iterator{end, end},
            std::back_inserter(bs)))
        return {};
    return bs;
}

[[maybe_unused]] auto from_hex2(std::string_view s) noexcept
{
    return from_hex2(s.begin(), s.end());
}

}  // namespace

int main()
{
    try
    {
        for (std::string line; std::getline(std::cin, line);)
        {
            if (line.empty() || line.starts_with('#'))
                continue;

            auto o = from_hex2(line);
            if (!o)
            {
                std::cout << "err: invalid hex" << std::endl;
                continue;
            }

            const auto eof = std::move(*o);
            const auto err = evmone::validate_eof(EVMC_SHANGHAI, eof);
            if (err != evmone::EOFValidationError::success)
            {
                std::cout << "err: " << evmone::get_error_message(err) << std::endl;
                continue;
            }

            const auto header = evmone::read_valid_eof1_header(eof);
            std::cout << "OK ";
            for (size_t i = 0; i < header.code_sizes.size(); ++i)
            {
                if (i != 0)
                    std::cout << ',';
                std::cout << evmc::hex(
                    evmc::bytes_view{&eof[header.code_offsets.at(i)], header.code_sizes[i]});
            }
            std::cout << std::endl;
        }
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
