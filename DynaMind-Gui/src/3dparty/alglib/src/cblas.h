/*************************************************************************
Copyright (c) 2005-2007, Sergey Bochkanov (ALGLIB project).

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

#ifndef _cblas_h
#define _cblas_h

#include "ap.h"
#include "ialglib.h"

void complexmatrixvectormultiply(const ap::complex_2d_array& a,
     int i1,
     int i2,
     int j1,
     int j2,
     bool transa,
     bool conja,
     const ap::complex_1d_array& x,
     int ix1,
     int ix2,
     ap::complex alpha,
     ap::complex_1d_array& y,
     int iy1,
     int iy2,
     ap::complex beta,
     ap::complex_1d_array& t);


#endif

