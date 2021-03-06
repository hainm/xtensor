/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef TEST_COMMON_HPP
#define TEST_COMMON_HPP

namespace xt
{
    template <class C = std::vector<std::size_t>>
    struct layout_result
    {
        using vector_type = std::vector<int>;
        using size_type = typename C::value_type;
        using shape_type = C;
        using strides_type = C;

        using assigner_type = std::vector<std::vector<vector_type>>;

        inline layout_result()
        {
            m_shape = { 3, 2, 4 };
            m_assigner.resize(m_shape[0]);
            for(std::size_t i = 0; i < m_shape[0]; ++i)
            {
                m_assigner[i].resize(m_shape[1]);
            }
            m_assigner[0][0] = { -1, 1, 2, 3 };
            m_assigner[0][1] = { 4, 5, 6, 7 };
            m_assigner[1][0] = { 8, 9, 10, 11 };
            m_assigner[1][1] = { 12, 13, 14, 15 };
            m_assigner[2][0] = { 16, 17, 18, 19 };
            m_assigner[2][1] = { 20, 21, 22, 23 };
        }

        shape_type m_shape;
        strides_type m_strides;
        strides_type m_backstrides;
        vector_type m_data;
        assigner_type m_assigner;

        inline size_type size() const { return m_data.size(); }
        inline const shape_type& shape() const { return m_shape; }
        inline const strides_type& strides() const { return m_strides; }
        inline const strides_type& backstrides() const { return m_backstrides; }
        inline const vector_type& data() const { return m_data; }
    };

    template <class C = std::vector<std::size_t>>
    struct row_major_result : layout_result<C>
    {
        inline row_major_result()
        {
            this->m_strides = { 8, 4, 1 };
            this->m_backstrides = {16, 4, 3};
            this->m_data = { -1, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                             10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                             20, 21, 22, 23 };
        }
    };

    template <class C = std::vector<std::size_t>>
    struct column_major_result : layout_result<C>
    {
        inline column_major_result()
        {
            this->m_strides = { 1, 3, 6 };
            this->m_backstrides = { 2, 3, 18 };
            this->m_data = { -1, 8, 16, 4, 12, 20,
                              1, 9, 17, 5, 13, 21,
                              2, 10, 18, 6, 14, 22,
                              3, 11, 19, 7, 15, 23 };
        }
    };

    template <class C = std::vector<std::size_t>>
    struct central_major_result : layout_result<C>
    {
        inline central_major_result()
        {
            this->m_strides = { 8, 1, 2 };
            this->m_backstrides = { 16, 1, 6};
            this->m_data = { -1, 4, 1, 5, 2, 6, 3, 7,
                              8, 12, 9, 13, 10, 14, 11, 15,
                             16, 20, 17, 21, 18, 22, 19, 23 };
        }
    };

    template <class C = std::vector<std::size_t>>
    struct unit_shape_result
    {
        using vector_type = std::vector<int>;
        using size_type = typename C::value_type;
        using shape_type = C;
        using strides_type = C;

        using assigner_type = std::vector<std::vector<vector_type>>;

        inline unit_shape_result()
        {
            m_shape = { 3, 1, 4 };
            m_strides = { 4, 0, 1 };
            m_backstrides = { 8, 0, 3 };
            m_data = { -1, 1, 2, 3, 8, 9, 10, 11, 16, 17, 18, 19 };
            m_assigner.resize(m_shape[0]);
            for(std::size_t i = 0; i < m_shape[0]; ++i)
            {
                m_assigner[i].resize(m_shape[1]);
            }
            m_assigner[0][0] = { -1, 1, 2, 3 };
            m_assigner[1][0] = { 8, 9, 10, 11 };
            m_assigner[2][0] = { 16, 17, 18, 19 };
        }

        shape_type m_shape;
        strides_type m_strides;
        strides_type m_backstrides;
        vector_type m_data;
        assigner_type m_assigner;

        inline size_type size() const { return m_data.size(); }
        inline const shape_type& shape() const { return m_shape; }
        inline const strides_type& strides() const { return m_strides; }
        inline const strides_type& backstrides() const { return m_backstrides; }
        inline const vector_type& data() const { return m_data; }
    };

    template <class V, class R>
    void compare_shape(V& vec, const R& result)
    {
        EXPECT_EQ(vec.shape(), result.shape());
        EXPECT_EQ(vec.strides(), result.strides());
        EXPECT_EQ(vec.backstrides(), result.backstrides());
        EXPECT_EQ(vec.size(), result.size());
    }

    template <class V, class C = std::vector<std::size_t>>
    void test_reshape(V& vec)
    {
        {
            SCOPED_TRACE("row_major reshape");
            row_major_result<C> rm;
            vec.reshape(rm.m_shape, layout::row_major);
            compare_shape(vec, rm);
        }

        {
            SCOPED_TRACE("column_major reshape");
            column_major_result<C> cm;
            vec.reshape(cm.m_shape, layout::column_major);
            compare_shape(vec, cm);
        }

        {
            SCOPED_TRACE("central_major reshape");
            central_major_result<C> cem;
            vec.reshape(cem.m_shape, cem.m_strides);
            compare_shape(vec, cem);
        }

        {
            SCOPED_TRACE("unit_shape reshape");
            unit_shape_result<C> usr;
            vec.reshape(usr.m_shape, layout::row_major);
            compare_shape(vec, usr);
        }
    }

    template <class V, class C = std::vector<std::size_t>>
    void test_transpose(V& vec)
    {
        using shape_type = typename V::shape_type;
        using strides_type = typename V::strides_type;
        {
            SCOPED_TRACE("transpose");
            shape_type shape_new = vec.shape();
            vec.transpose();
            std::reverse(shape_new.begin(), shape_new.end());
            EXPECT_EQ(vec.shape(), shape_new);
        }

        {
            SCOPED_TRACE("transpose with data");
            row_major_result<C> rm;
            vec.reshape(rm.shape(), layout::row_major);

            assign_array(vec, rm.m_assigner);
            EXPECT_EQ(vec.data(), rm.m_data);

            auto vec_copy = vec;

            shape_type shape_new(rm.shape());
            vec.transpose();
            std::reverse(shape_new.begin(), shape_new.end());
            EXPECT_EQ(vec.shape(), shape_new);
            EXPECT_EQ(vec.data(), rm.m_data);

            strides_type new_strides = {rm.m_strides[2],
                                        rm.m_strides[1],
                                        rm.m_strides[0]};
            EXPECT_EQ(vec.strides(), new_strides);

            strides_type new_backstrides = {rm.m_backstrides[2],
                                            rm.m_backstrides[1],
                                            rm.m_backstrides[0]};
            EXPECT_EQ(vec.backstrides(), new_backstrides);

            EXPECT_EQ(vec_copy(0, 0, 0), vec(0, 0, 0));
            EXPECT_EQ(vec_copy(0, 1, 0), vec(0, 1, 0));
            EXPECT_EQ(vec_copy(1, 1, 0), vec(0, 1, 1));
            EXPECT_EQ(vec_copy(1, 1, 2), vec(2, 1, 1));
        }

        {
            SCOPED_TRACE("transpose with permutation");
            row_major_result<C> rm;
            vec.reshape(rm.shape(), layout::row_major);

            assign_array(vec, rm.m_assigner);
            EXPECT_EQ(vec.data(), rm.m_data);

            auto vec_copy = vec;

            shape_type a = vec.shape();
            vec.transpose({1, 0, 2});
            shape_type shape_new = {a[1], a[0], a[2]};
            EXPECT_EQ(vec.shape(), shape_new);
            EXPECT_EQ(vec.data(), rm.m_data);

            strides_type new_strides = {rm.m_strides[1], 
                                        rm.m_strides[0], 
                                        rm.m_strides[2]};
            EXPECT_EQ(vec.strides(), new_strides);

            strides_type new_backstrides = {rm.m_backstrides[1], 
                                            rm.m_backstrides[0], 
                                            rm.m_backstrides[2]};
            EXPECT_EQ(vec.backstrides(), new_backstrides);

            EXPECT_EQ(vec_copy(0, 0, 0), vec(0, 0, 0));
            EXPECT_EQ(vec_copy(0, 1, 0), vec(1, 0, 0));
            EXPECT_EQ(vec_copy(1, 1, 0), vec(1, 1, 0));
            EXPECT_EQ(vec_copy(1, 1, 2), vec(1, 1, 2));
        }

        {
            SCOPED_TRACE("transpose permutation throws");
            row_major_result<C> rm;
            vec.reshape(rm.shape(), layout::row_major);

            EXPECT_THROW(vec.transpose({1, 1, 0}, check_policy::full()), transpose_error);
            EXPECT_THROW(vec.transpose({1, 0, 2, 3}, check_policy::full()), transpose_error);
            EXPECT_THROW(vec.transpose({1, 2}, check_policy::full()), transpose_error);
            EXPECT_THROW(vec.transpose({3, 0, 1}, check_policy::full()), transpose_error);
        }
    }

    template <class V1, class V2>
    void assign_array(V1& dst, const V2& src)
    {
        for(std::size_t i = 0; i < dst.shape()[0]; ++i)
        {
            for(std::size_t j = 0; j < dst.shape()[1]; ++j)
            {
                for(std::size_t k = 0; k < dst.shape()[2]; ++k)
                {
                    dst(i, j, k) = src[i][j][k];
                }
            }
        }
    }

    template <class V, class C = std::vector<std::size_t>>
    void test_access(V& vec)
    {
        {
            SCOPED_TRACE("row_major access");
            row_major_result<C> rm;
            vec.reshape(rm.m_shape, layout::row_major);
            assign_array(vec, rm.m_assigner);
            EXPECT_EQ(vec.data(), rm.m_data);
        }

        {
            SCOPED_TRACE("column_major access");
            column_major_result<C> cm;
            vec.reshape(cm.m_shape, layout::column_major);
            assign_array(vec, cm.m_assigner);
            EXPECT_EQ(vec.data(), cm.m_data);
        }

        {
            SCOPED_TRACE("central_major access");
            central_major_result<C> cem;
            vec.reshape(cem.m_shape, cem.m_strides);
            assign_array(vec, cem.m_assigner);
            EXPECT_EQ(vec.data(), cem.m_data);
        }

        {
            SCOPED_TRACE("unit_shape access");
            unit_shape_result<C> usr;
            vec.reshape(usr.m_shape, layout::row_major);
            assign_array(vec, usr.m_assigner);
            EXPECT_EQ(vec.data(), usr.m_data);
        }
    }

    template <class V1, class V2>
    void indexed_assign_array(V1& dst, const V2& src)
    {
        xindex index(dst.dimension());
        for (std::size_t i = 0; i < dst.shape()[0]; ++i)
        {
            index[0] = i;
            for (std::size_t j = 0; j < dst.shape()[1]; ++j)
            {
                index[1] = j;
                for (std::size_t k = 0; k < dst.shape()[2]; ++k)
                {
                    index[2] = k;
                    dst[index] = src[i][j][k];
                }
            }
        }
    }

    template <class V, class C = std::vector<std::size_t>>
    void test_indexed_access(V& vec)
    {
        {
            SCOPED_TRACE("row_major access");
            row_major_result<C> rm;
            vec.reshape(rm.m_shape, layout::row_major);
            indexed_assign_array(vec, rm.m_assigner);
            EXPECT_EQ(vec.data(), rm.m_data);
        }

        {
            SCOPED_TRACE("column_major access");
            column_major_result<C> cm;
            vec.reshape(cm.m_shape, layout::column_major);
            indexed_assign_array(vec, cm.m_assigner);
            EXPECT_EQ(vec.data(), cm.m_data);
        }

        {
            SCOPED_TRACE("central_major access");
            central_major_result<C> cem;
            vec.reshape(cem.m_shape, cem.m_strides);
            indexed_assign_array(vec, cem.m_assigner);
            EXPECT_EQ(vec.data(), cem.m_data);
        }

        {
            SCOPED_TRACE("unit_shape access");
            unit_shape_result<C> usr;
            vec.reshape(usr.m_shape, layout::row_major);
            indexed_assign_array(vec, usr.m_assigner);
            EXPECT_EQ(vec.data(), usr.m_data);
        }
    }

    template <class V>
    void test_broadcast(V& vec)
    {
        using shape_type = typename V::shape_type;

        shape_type s = { 3, 1, 4, 2 };
        vec.reshape(s);

        {
            SCOPED_TRACE("same shape");
            shape_type s1 = s;
            bool res = vec.broadcast_shape(s1);
            EXPECT_EQ(s1, s);
            EXPECT_TRUE(res);
        }

        {
            SCOPED_TRACE("different shape");
            shape_type s2 = { 3, 5, 1, 2 };
            shape_type s2r = {3, 5, 4, 2};
            bool res = vec.broadcast_shape(s2);
            EXPECT_EQ(s2, s2r);
            EXPECT_FALSE(res);

        }

        {
            SCOPED_TRACE("incompatible shapes");
            shape_type s4 = { 2, 1, 3, 2 };
            bool wit = false;
            try
            {
                vec.broadcast_shape(s4);
            }
            catch(broadcast_error&)
            {
                wit = true;
            }
            EXPECT_TRUE(wit);
        }
    }

    template <class V>
    void test_broadcast2(V& vec)
    {
        using shape_type = typename V::shape_type;

        shape_type s = { 3, 1, 4, 2 };
        vec.reshape(s);

        {
            SCOPED_TRACE("different dimensions");
            shape_type s3 = {5, 3, 1, 4, 2};
            shape_type s3r = s3;
            bool res = vec.broadcast_shape(s3);
            EXPECT_EQ(s3, s3r);
            EXPECT_FALSE(res);
        }
    }

    template <class V, class C = std::vector<std::size_t>>
    void test_storage_iterator(V& vec)
    {
        {
            SCOPED_TRACE("row_major storage iterator");
            row_major_result<C> rm;
            vec.reshape(rm.m_shape, layout::row_major);
            std::copy(rm.data().cbegin(), rm.data().cend(), vec.storage_begin());
            EXPECT_EQ(rm.data(), vec.data());
            EXPECT_EQ(vec.storage_end(), vec.data().end());
        }

        {
            SCOPED_TRACE("column_major storage iterator");
            column_major_result<C> cm;
            vec.reshape(cm.m_shape, layout::column_major);
            std::copy(cm.data().cbegin(), cm.data().cend(), vec.storage_begin());
            EXPECT_EQ(cm.data(), vec.data());
            EXPECT_EQ(vec.storage_end(), vec.data().end());
        }

        {
            SCOPED_TRACE("central_major storage iterator");
            central_major_result<C> cem;
            vec.reshape(cem.m_shape, cem.m_strides);
            std::copy(cem.data().cbegin(), cem.data().cend(), vec.storage_begin());
            EXPECT_EQ(cem.data(), vec.data());
            EXPECT_EQ(vec.storage_end(), vec.data().end());
        }

        {
            SCOPED_TRACE("unit_shape storage iterator");
            unit_shape_result<C> usr;
            vec.reshape(usr.m_shape, layout::row_major);
            std::copy(usr.data().cbegin(), usr.data().cend(), vec.storage_begin());
            EXPECT_EQ(usr.data(), vec.data());
            EXPECT_EQ(vec.storage_end(), vec.data().end());
        }
    }
}

#endif

