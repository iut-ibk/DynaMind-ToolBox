/*************************************************************************
Copyright 2008 by Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses

>>> END OF LICENSE >>>
*************************************************************************/

#include <stdafx.h>
#include "tsort.h"

void tagsort(ap::real_1d_array& a,
     int n,
     ap::integer_1d_array& p1,
     ap::integer_1d_array& p2)
{
    int i;
    int j;
    int k;
    int t;
    double tmp;
    int tmpi;
    ap::integer_1d_array pv;
    ap::integer_1d_array vp;
    int lv;
    int lp;
    int rv;
    int rp;

    
    //
    // Special cases
    //
    if( n<=0 )
    {
        return;
    }
    if( n==1 )
    {
        p1.setbounds(0, 0);
        p2.setbounds(0, 0);
        p1(0) = 0;
        p2(0) = 0;
        return;
    }
    
    //
    // General case, N>1: prepare permutations table P1
    //
    p1.setbounds(0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        p1(i) = i;
    }
    
    //
    // General case, N>1: sort, update P1
    //
    tagsortfasti(a, p1, n);
    
    //
    // General case, N>1: fill permutations table P2
    //
    // To fill P2 we maintain two arrays:
    // * PV, Position(Value). PV[i] contains position of I-th key at the moment
    // * VP, Value(Position). VP[i] contains key which has position I at the moment
    //
    // At each step we making permutation of two items:
    //   Left, which is given by position/value pair LP/LV
    //   and Right, which is given by RP/RV
    // and updating PV[] and VP[] correspondingly.
    //
    pv.setbounds(0, n-1);
    vp.setbounds(0, n-1);
    p2.setbounds(0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        pv(i) = i;
        vp(i) = i;
    }
    for(i = 0; i <= n-1; i++)
    {
        
        //
        // calculate LP, LV, RP, RV
        //
        lp = i;
        lv = vp(lp);
        rv = p1(i);
        rp = pv(rv);
        
        //
        // Fill P2
        //
        p2(i) = rp;
        
        //
        // update PV and VP
        //
        vp(lp) = rv;
        vp(rp) = lv;
        pv(lv) = rp;
        pv(rv) = lp;
    }
}


void tagsortfasti(ap::real_1d_array& a, ap::integer_1d_array& b, int n)
{
    int i;
    int j;
    int k;
    int t;
    double tmp;
    int tmpi;

    
    //
    // Special cases
    //
    if( n<=1 )
    {
        return;
    }
    
    //
    // General case, N>1: sort, update B
    //
    i = 2;
    do
    {
        t = i;
        while(t!=1)
        {
            k = t/2;
            if( ap::fp_greater_eq(a(k-1),a(t-1)) )
            {
                t = 1;
            }
            else
            {
                tmp = a(k-1);
                a(k-1) = a(t-1);
                a(t-1) = tmp;
                tmpi = b(k-1);
                b(k-1) = b(t-1);
                b(t-1) = tmpi;
                t = k;
            }
        }
        i = i+1;
    }
    while(i<=n);
    i = n-1;
    do
    {
        tmp = a(i);
        a(i) = a(0);
        a(0) = tmp;
        tmpi = b(i);
        b(i) = b(0);
        b(0) = tmpi;
        t = 1;
        while(t!=0)
        {
            k = 2*t;
            if( k>i )
            {
                t = 0;
            }
            else
            {
                if( k<i )
                {
                    if( ap::fp_greater(a(k),a(k-1)) )
                    {
                        k = k+1;
                    }
                }
                if( ap::fp_greater_eq(a(t-1),a(k-1)) )
                {
                    t = 0;
                }
                else
                {
                    tmp = a(k-1);
                    a(k-1) = a(t-1);
                    a(t-1) = tmp;
                    tmpi = b(k-1);
                    b(k-1) = b(t-1);
                    b(t-1) = tmpi;
                    t = k;
                }
            }
        }
        i = i-1;
    }
    while(i>=1);
}


void tagsortfastr(ap::real_1d_array& a, ap::real_1d_array& b, int n)
{
    int i;
    int j;
    int k;
    int t;
    double tmp;
    double tmpr;

    
    //
    // Special cases
    //
    if( n<=1 )
    {
        return;
    }
    
    //
    // General case, N>1: sort, update B
    //
    i = 2;
    do
    {
        t = i;
        while(t!=1)
        {
            k = t/2;
            if( ap::fp_greater_eq(a(k-1),a(t-1)) )
            {
                t = 1;
            }
            else
            {
                tmp = a(k-1);
                a(k-1) = a(t-1);
                a(t-1) = tmp;
                tmpr = b(k-1);
                b(k-1) = b(t-1);
                b(t-1) = tmpr;
                t = k;
            }
        }
        i = i+1;
    }
    while(i<=n);
    i = n-1;
    do
    {
        tmp = a(i);
        a(i) = a(0);
        a(0) = tmp;
        tmpr = b(i);
        b(i) = b(0);
        b(0) = tmpr;
        t = 1;
        while(t!=0)
        {
            k = 2*t;
            if( k>i )
            {
                t = 0;
            }
            else
            {
                if( k<i )
                {
                    if( ap::fp_greater(a(k),a(k-1)) )
                    {
                        k = k+1;
                    }
                }
                if( ap::fp_greater_eq(a(t-1),a(k-1)) )
                {
                    t = 0;
                }
                else
                {
                    tmp = a(k-1);
                    a(k-1) = a(t-1);
                    a(t-1) = tmp;
                    tmpr = b(k-1);
                    b(k-1) = b(t-1);
                    b(t-1) = tmpr;
                    t = k;
                }
            }
        }
        i = i-1;
    }
    while(i>=1);
}


void tagsortfast(ap::real_1d_array& a, int n)
{
    int i;
    int j;
    int k;
    int t;
    double tmp;
    double tmpr;

    
    //
    // Special cases
    //
    if( n<=1 )
    {
        return;
    }
    
    //
    // General case, N>1: sort, update B
    //
    i = 2;
    do
    {
        t = i;
        while(t!=1)
        {
            k = t/2;
            if( ap::fp_greater_eq(a(k-1),a(t-1)) )
            {
                t = 1;
            }
            else
            {
                tmp = a(k-1);
                a(k-1) = a(t-1);
                a(t-1) = tmp;
                t = k;
            }
        }
        i = i+1;
    }
    while(i<=n);
    i = n-1;
    do
    {
        tmp = a(i);
        a(i) = a(0);
        a(0) = tmp;
        t = 1;
        while(t!=0)
        {
            k = 2*t;
            if( k>i )
            {
                t = 0;
            }
            else
            {
                if( k<i )
                {
                    if( ap::fp_greater(a(k),a(k-1)) )
                    {
                        k = k+1;
                    }
                }
                if( ap::fp_greater_eq(a(t-1),a(k-1)) )
                {
                    t = 0;
                }
                else
                {
                    tmp = a(k-1);
                    a(k-1) = a(t-1);
                    a(t-1) = tmp;
                    t = k;
                }
            }
        }
        i = i-1;
    }
    while(i>=1);
}




