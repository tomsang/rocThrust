// MIT License
//
// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Google Test
#include <gtest/gtest.h>

#include <thrust/iterator/zip_iterator.h>
#include <thrust/sort.h>

// HIP API
#if THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC
#include <hip/hip_runtime_api.h>
#include <hip/hip_runtime.h>

#define HIP_CHECK(condition) ASSERT_EQ(condition, hipSuccess)
#endif // THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC

#include "test_utils.hpp"
#include "test_assertions.hpp"

template<
        class InputType
>
struct Params
{
    using input_type = InputType;
};

template<class Params>
class ZipIteratorStableSortTests : public ::testing::Test
{
public:
    using input_type = typename Params::input_type;
};

typedef ::testing::Types<
    Params<int8_t>,
    Params<int16_t>,
    Params<int32_t>
> TestParams;

TYPED_TEST_CASE(ZipIteratorStableSortTests, TestParams);

#if THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC

TYPED_TEST(ZipIteratorStableSortTests, TestZipIteratorStableSort)
{
    using namespace thrust;
    using T = typename TestFixture::input_type;

    const std::vector<size_t> sizes = get_sizes();

    for(auto size : sizes)
    {
        thrust::host_vector<T> h1 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        thrust::host_vector<T> h2 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

        device_vector<T> d1 = h1;
        device_vector<T> d2 = h2;

        // sort on host
        stable_sort(
                make_zip_iterator(make_tuple(h1.begin(), h2.begin())),
                make_zip_iterator(make_tuple(h1.end(), h2.end())));

        // sort on device
        stable_sort(
                make_zip_iterator(make_tuple(d1.begin(), d2.begin())),
                make_zip_iterator(make_tuple(d1.end(), d2.end())));

        ASSERT_EQ_QUIET(h1, d1);
        ASSERT_EQ_QUIET(h2, d2);
    }
}

#endif // THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC