/*
 * =====================================================================================
 *
 *       Filename:  ObjectPtrGuard.hpp
 *
 *    Description:  guard template
 *
 *        Version:  1.0
 *        Created:  04/17/2015 09:20:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_BASELIB_OBJECT_PTR_GUARD_H_
#define _GFX_BASELIB_OBJECT_PTR_GUARD_H_

#include <stdio.h>
#include "mydef.h"

template < typename T >
struct policy_ptr_delete
{
    void destroy(T* &ptr)
    {
        SAFE_DELETE(ptr);
    }
};

template < typename T >
struct policy_array_delete
{
    void destroy(T* &ptr)
    {
        SAFE_DELETE_ARRAY(ptr);
    }
};

template < typename T >
struct policy_array_free
{
    void destroy(T* &ptr)
    {
        if (NULL != ptr)
        {
            free(ptr);
            ptr = NULL;
        }
    }
};

template < typename T >
struct policy_ptr_release
{
    void destroy(T* &ptr)
    {
        if (NULL != ptr)
        {
            ptr->release();
            ptr = NULL;
        }
    }
};

template <>
struct policy_ptr_delete<FILE>
{
    void destroy(FILE* &ptr)
    {
        if (NULL != ptr)
        {
            fclose(ptr);
            ptr = NULL;
        }
    }
};

template < typename T >
struct policy_null_delete
{
    void destroy(T* &ptr)
    {
        UNUSED(ptr);
    }
};

//对象指针的守护模板类
template<typename T, typename DestroyPolicy = policy_ptr_delete<T> >
class TObjectPtrGuard : public DestroyPolicy
{
public:
    explicit TObjectPtrGuard(T* ptr)
        : m_ptr(ptr)
        , m_bRetain(ptr!=NULL)
    {
    }
    TObjectPtrGuard(const TObjectPtrGuard<T, DestroyPolicy>& rhs)
        : m_ptr(rhs.get())
        , m_bRetain(rhs.m_bRetain)
    {
        rhs.ReleaseOwnership();
    }
    TObjectPtrGuard<T, DestroyPolicy>& operator=(const TObjectPtrGuard<T, DestroyPolicy>& rhs)
    {
        if (this != &rhs)
        {
            if (m_ptr != rhs.get() && m_bRetain)
            {
                this->destroy(m_ptr);
            }
            m_ptr = rhs.get();
            m_bRetain = rhs.m_bRetain;
            rhs.ReleaseOwnership();
        }
        return (*this);
    }
    TObjectPtrGuard<T, DestroyPolicy>& operator=(T *ptr)
    {
        if (m_ptr != ptr)
        {
            if (m_bRetain)
            {
                this->destroy(m_ptr);
            }
            m_ptr = ptr;
            m_bRetain = true;
        }
        return (*this);
    }
    ~TObjectPtrGuard()
    {
        if (m_bRetain)
        {
            m_bRetain = false;
            this->destroy(m_ptr);
        }
    }
    
public:
    void release()
    {
        if (m_bRetain)
        {
            this->ReleaseOwnership();
            this->destroy(m_ptr);
            m_bRetain = false;
        }
    }

    T* get() const          { return m_ptr; }
    operator T*() const     { return m_ptr; }
    T& operator*() const    { return *m_ptr; }
    T* operator->() const   { return m_ptr; }
    bool operator!() const  { return (NULL==m_ptr); }
    operator bool() const   { return (NULL!=m_ptr); }

protected:
    void ReleaseOwnership() { m_bRetain = false; }

private:
    T*              m_ptr;          //指针
    bool            m_bRetain;      //是否拥有该指针的权限
};

typedef TObjectPtrGuard<FILE>                                   CAutoFile;

// 采用new 分配内存, delete 释放内存
typedef TObjectPtrGuard<byte_t, policy_array_delete<byte_t> >   CAutoArray;
typedef TObjectPtrGuard<char, policy_array_delete<char> >       CAutoBuffer;

// 采用 calloc/malloc 分配内存，free释放内存
typedef TObjectPtrGuard<byte_t, policy_array_free<byte_t> >     CAutoArrayEx;

#endif  //_GFX_BASELIB_OBJECT_PTR_GUARD_H_



