#pragma once

// This file isn't related to the Challenge
// This is for compatibility purposes

template <class _Ty>
const void* __stdcall __std_find_trivial_impl(const void* _First, const void* _Last, _Ty _Val) noexcept {
    auto _Ptr = static_cast<const _Ty*>(_First);
    while (_Ptr != _Last && *_Ptr != _Val) {
        ++_Ptr;
    }
    return _Ptr;
}

extern "C" {

    const void* __stdcall __std_find_trivial_1(const void* const _First, const void* const _Last, const uint8_t _Val) noexcept {
        return __std_find_trivial_impl(_First, _Last, _Val);
    }
}
