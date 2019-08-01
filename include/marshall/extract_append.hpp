/** @file
 *
 *  @ingroup marshall_module
 *
 *  @brief Extract fundamental integral values in native endianness from a byte buffer;
 *  conversely, given a fundamental type, append it to a buffer of bytes in network endian
 *  order (big endian).
 *
 *  The functions in this file are low-level, handling fundamental integral types and 
 *  extracting or appending to @c std::byte buffers. It is meant to be the lower layer
 *  of marshalling utilities, where the next layer up provides stream facilities,
 *  sequences, overloads for specific types such as @c std::string and @c bool, and generic
 *  buffer types.
 *
 *  @note When C++ 20 @c std::endian is available, many of these functions can be made
 *  @c constexpr and evaluated at compile time. Until then, run-time endian detection and 
 *  copying is performed.
 *
 *  @note The variable sized integer functions (@c extract_var_int, @c append_var_int) 
 *  support the variable byte integer type in MQTT (Message Queuing Telemetry Transport),
 *  a commonly used IoT protocol. The code in this header is adapted from a 
 *  Techoverflow.net article by Uli Koehler and published under the CC0 1.0 Universal 
 *  license:
 *  https://techoverflow.net/2013/01/25/efficiently-encoding-variable-length-integers-in-cc/
 *
 *  @author Cliff Green, Roxanne Agerone, Uli Koehler
 *
 *  Copyright (c) 2019 by Cliff Green, Roxanne Agerone
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef EXTRACT_APPEND_HPP_INCLUDED
#define EXTRACT_APPEND_HPP_INCLUDED

#include "utility/cast_ptr_to.hpp"

#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint32_t, etc
#include <type_traits> // std::is_integral

namespace chops {

namespace detail {

// since function template partial specialization is not supported (yet) in C++, overload the 
// extract_val_swap and noswap function templates with a parameter corresponding to the 
// sizeof the value; no data is passed in the second parameter, only using the type for overloading

template <unsigned int>
struct size_tag { };

// if char / byte, no swap needed
template <typename T>
T extract_val_swap(const std::byte* buf, const size_tag<1u>*) noexcept {
  return static_cast<T>(*buf); // static_cast needed to convert std::byte to char type
}
template <typename T>
T extract_val_noswap(const std::byte* buf, const size_tag<1u>*) noexcept {
  return static_cast<T>(*buf); // see note above
}

template <typename T>
T extract_val_swap(const std::byte* buf, const size_tag<2u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+1);
  *(p+1) = *(buf+0);
  return tmp;
}

template <typename T>
T extract_val_noswap(const std::byte* buf, const size_tag<2u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+0);
  *(p+1) = *(buf+1);
  return tmp;
}

template <typename T>
T extract_val_swap(const std::byte* buf, const size_tag<4u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+3);
  *(p+1) = *(buf+2);
  *(p+2) = *(buf+1);
  *(p+3) = *(buf+0);
  return tmp;
}

template <typename T>
T extract_val_noswap(const std::byte* buf, const size_tag<4u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+0);
  *(p+1) = *(buf+1);
  *(p+2) = *(buf+2);
  *(p+3) = *(buf+3);
  return tmp;
}

template <typename T>
T extract_val_swap(const std::byte* buf, const size_tag<8u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+7);
  *(p+1) = *(buf+6);
  *(p+2) = *(buf+5);
  *(p+3) = *(buf+4);
  *(p+4) = *(buf+3);
  *(p+5) = *(buf+2);
  *(p+6) = *(buf+1);
  *(p+7) = *(buf+0);
  return tmp;
}

template <typename T>
T extract_val_noswap(const std::byte* buf, const size_tag<8u>*) noexcept {
  T tmp{};
  std::byte* p = cast_ptr_to<std::byte>(&tmp);
  *(p+0) = *(buf+0);
  *(p+1) = *(buf+1);
  *(p+2) = *(buf+2);
  *(p+3) = *(buf+3);
  *(p+4) = *(buf+4);
  *(p+5) = *(buf+5);
  *(p+6) = *(buf+6);
  *(p+7) = *(buf+7);
  return tmp;
}

template <typename T>
std::size_t append_val_swap(std::byte* buf, const T& val, const size_tag<1u>*) noexcept {
  *buf = static_cast<std::byte>(val); // static_cast needed to convert char to std::byte
  return 1u;
}

template <typename T>
std::size_t append_val_noswap(std::byte* buf, const T& val, const size_tag<1u>*) noexcept {
  *buf = static_cast<std::byte>(val); // see note above
  return 1u;
}

template <typename T>
std::size_t append_val_swap(std::byte* buf, const T& val, const size_tag<2u>*) noexcept {
  const std::byte* p = cast_ptr_to<std::byte>(&val);
  *(buf+0) = *(p+1);
  *(buf+1) = *(p+0);
  return 2u;
}

template <typename T>
std::size_t append_val_noswap(std::byte* buf, const T& val, const size_tag<2u>*) noexcept {
  const std::byte* p = cast_ptr_to<std::byte>(&val);
  *(buf+0) = *(p+0);
  *(buf+1) = *(p+1);
  return 2u;
}

template <typename T>
std::size_t append_val_swap(std::byte* buf, const T& val, const size_tag<4u>*) noexcept {
  const std::byte* p = cast_ptr_to<std::byte>(&val);
  *(buf+0) = *(p+3);
  *(buf+1) = *(p+2);
  *(buf+2) = *(p+1);
  *(buf+3) = *(p+0);
  return 4u;
}

template <typename T>
std::size_t append_val_noswap(std::byte* buf, const T& val, const size_tag<4u>*) noexcept {
  const std::byte* p = cast_ptr_to<std::byte>(&val);
  *(buf+0) = *(p+0);
  *(buf+1) = *(p+1);
  *(buf+2) = *(p+2);
  *(buf+3) = *(p+3);
  return 4u;
}

template <typename T>
std::size_t append_val_swap(std::byte* buf, const T& val, const size_tag<8u>*) noexcept {
  const std::byte* p = cast_ptr_to<std::byte>(&val);
  *(buf+0) = *(p+7);
  *(buf+1) = *(p+6);
  *(buf+2) = *(p+5);
  *(buf+3) = *(p+4);
  *(buf+4) = *(p+3);
  *(buf+5) = *(p+2);
  *(buf+6) = *(p+1);
  *(buf+7) = *(p+0);
  return 8u;
}

template <typename T>
std::size_t append_val_noswap(std::byte* buf, const T& val, const size_tag<8u>*) noexcept {
  const std::byte* p = cast_ptr_to<std::byte>(&val);
  *(buf+0) = *(p+0);
  *(buf+1) = *(p+1);
  *(buf+2) = *(p+2);
  *(buf+3) = *(p+3);
  *(buf+4) = *(p+4);
  *(buf+5) = *(p+5);
  *(buf+6) = *(p+6);
  *(buf+7) = *(p+7);
  return 8u;
}


} // end namespace detail

// C++ 20 will contain std::endian, which allows full compile time endian detection;
// until then, the endian detection and branching will be runtime, although it
// can be computed at global initialization instead of each time it is called

inline bool detect_big_endian () noexcept {
  const std::uint32_t tmp {0xDDCCBBAA};
  return *(cast_ptr_to<std::byte>(&tmp) + 3) == static_cast<std::byte>(0xAA);
}

// should be computed at global initialization time

const bool big_endian = detect_big_endian();

/**
 * @brief Extract a value in network byte order (big endian) from a @c std::byte buffer 
 * into a fundamental integral type in native endianness, swapping bytes as needed.
 *
 * This function template dispatches on specific sizes. If an unsupported size is attempted
 * to be swapped, a compile time error is generated.
 *
 * @param buf Buffer of @c std::bytes containing an object of type T in network byte order.
 *
 * @return A value in native endian order.
 *
 * @pre The buffer must contain at least @c sizeof(T) bytes.
 *
 * @note Floating point swapping is not supported, only integral types. Swapping floating
 * point types can easily result in NaN representations, which can generate hardware traps,
 * either causing runtime crashes or silently changing bits within the floating point number.
 * In particular, returning a byte swapped floating point type by value will result in
 * bad things happening.
 *
 */
template <typename T>
T extract_val(const std::byte* buf) noexcept {

  static_assert(sizeof(T) == 1u || sizeof(T) == 2u || sizeof(T) == 4u || sizeof(T) == 8u,
    "Size for value extraction is not supported.");
  static_assert(std::is_integral<T>::value, "Value extraction is only supported for integral types.");

  return big_endian ? detail::extract_val_noswap<T>(buf, static_cast< const detail::size_tag<sizeof(T)>* >(nullptr)) :
                      detail::extract_val_swap<T>(buf, static_cast< const detail::size_tag<sizeof(T)>* >(nullptr));
}

/**
 * @brief Append a fundamental integral value to a @c std::byte buffer, swapping into network
 * endian order (big endian) as needed.
 *
 * This function template dispatches on specific sizes. If an unsupported size is attempted
 * to be swapped, a static error is generated.
 *
 * @param buf Buffer of @c std::bytes already allocated to hold the bytes of the value.
 *
 * @param val Value in native endian order to append to buf.
 *
 * @return Number of bytes copied into the @c std::byte buffer.
 *
 * @pre The buffer must already be allocated to hold at least @c sizeof(T) bytes.
 *
 * @note See note above about floating point values, which are not supported.
 *
 */
template <typename T>
std::size_t append_val(std::byte* buf, const T& val) noexcept {

  static_assert(sizeof(T) == 1u || sizeof(T) == 2u || sizeof(T) == 4u || sizeof(T) == 8u,
    "Size for value appending is not supported.");
  static_assert(std::is_integral<T>::value, "Value appending is only supported for integral types.");

  return big_endian ? detail::append_val_noswap(buf, val, static_cast< const detail::size_tag<sizeof(T)>* >(nullptr)) :
                      detail::append_val_swap(buf, val, static_cast< const detail::size_tag<sizeof(T)>* >(nullptr));
}

/**
 * @brief Encode an unsigned integer into a variable length buffer of bytes using the MSB (most
 * significant bit) algorithm.
 *
 * Given an integer, store the value in 1 or 2 or 3 or more bytes depending on the
 * value. If small (under 128) the value is stored in one byte. If the value is greater
 * than 127, the most significant bit in the first byte is set to 1 and the value is stored
 * in two bytes. This logic is repeated as necessary.
 *
 * This algorithm optimizes space when most of the values are small. If most of the values are
 * large, this algorithm is inefficient, needing more buffer space for the encoded integers than
 * if fixed size integer buffers were used.
 *
 * The output of this function is (by definition) in little-endian order (which is opposite
 * to the rest of the @c append and @c extract functions). However, this does not matter since
 * there is no byte swapping performed, and encoding and decoding will occur to the native 
 * endianness of the platform.
 * 
 * @note Unsigned types are not supported.
 *
 * @param val The input value. Any standard unsigned integer type is allowed.
 *
 * @param output A pointer to a preallocated array of @c std::bytes big enough for the output.
 * A safe minimum size is 5 bytes for 32 bit integers, 10 bytes for 64 bit integers and 3 bytes
 * for 16 bit integers.
 *
 * @return The number of bytes written to the output array.
 * 
 * @pre The output buffer must already be allocated large enough to hold the result.
 *
 */

template<typename T>
std::size_t append_var_int(std::byte* output, T val) {

    static_assert(std::is_unsigned<T>::value, "Only unsigned integer types are supported.");

    std::uint8_t* output_ptr = cast_ptr_to<std::uint8_t>(output);
    std::size_t output_size = 0;
    
    // While more than 7 bits of data are left, occupy the last output byte
    // and set the next byte flag
    while (val > 127) {

        output_ptr[output_size] = (static_cast<std::uint8_t> (val & 127)) | 128;
        //Remove the seven bits we just wrote
        val >>= 7;
        ++output_size;
    }
    output_ptr[output_size++] = static_cast<std::uint8_t> (val) & 127;
    return output_size;
}
/**
 * @brief Given a buffer of @c std::bytes that hold a variable sized integer, decode
 * them into an unsigned integer.
 *
 * For consistency with the @c append_var_int function, only unsigned integers are
 * supported for the output type of this function.
 *
 * @param input A variable-length encoded integer stored in a buffer of @c std::bytes.
 *
 * @param input_size Number of bytes representing the integer.
 *
 * @return The value in native unsigned integer format.
 * 
 */
template<typename T>
T extract_var_int(const std::byte* input, std::size_t input_size) {

    static_assert(std::is_unsigned<T>::value, "Only unsigned integer types are supported.");

    const std::uint8_t* input_ptr = cast_ptr_to<std::uint8_t> (input);
    
    T ret = 0;
    for (std::size_t i = 0; i < input_size; ++i) {
        ret |= (input_ptr[i] & 127) << (7 * i);
        //If the next-byte flag is set
        if(!(input_ptr[i] & 128)) {
            break;
        }
    }
    return ret;
}

} // end namespace

#endif

