/*
  File:       Stubs.h
 
  Contains:   Useful classes and functions for profile creation utilities.
 
  Version:    V1
 
  Copyright:  © see below
*/

/*
 * The ICC Software License, Version 0.1
 *
 *
 * Copyright (c) 2003-2006 The International Color Consortium. All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        The International Color Consortium (www.color.org)"
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "ICC" and "The International Color Consortium" must
 *    not be used to imply that the ICC organization endorses or
 *    promotes products derived from this software without prior
 *    written permission. For written permission, please see
 *    <http://www.color.org/>.
 *
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNATIONAL COLOR CONSORTIUM OR
 * ITS CONTRIBUTING MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the The International Color Consortium. 
 *
 *
 * Membership in the ICC is encouraged when this software is used for
 * commercial purposes. 
 *
 *  
 * For more information on The International Color Consortium, please
 * see <http://www.color.org/>.
 *  
 * 
 */

#ifndef __DEFINED_STUBS_H__
#define __DEFINED_STUBS_H__

#include <iosfwd>

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif

// stub for relevant parts of LP LLC color/CIEXYZ.{h, cpp}
class CIEXYZ
{
public:
  CIEXYZ() : X_(0), Y_(0), Z_(0) {}
  CIEXYZ(double X, double Y, double Z) : X_(X), Y_(Y), Z_(Z) {}

  double X() const { return X_; }
  double Y() const { return Y_; }
  double Z() const { return Z_; }

  friend
  std::ostream&
  operator<<(std::ostream& stream, const CIEXYZ& val);
  
private:
  double X_;
  double Y_;
  double Z_;
};

// stub for relevant parts of LP LLC color/DeviceRGB.{h, cpp}
class DeviceRGB
{
public:
  DeviceRGB(double r, double g, double b) : r_(r), g_(g), b_(b) {}

  virtual
  ~DeviceRGB() {}

  double R() const { return r_; }
  double G() const { return g_; }
  double B() const { return b_; }
  
protected:
  double r_;
  double g_;
  double b_;
};

// stub for relevant parts of LP LLC color/DPX.{h, cpp}
class DPX : public DeviceRGB
{
public:
  DPX() : DeviceRGB(0, 0, 0) {}
  DPX(double r, double g, double b) : DeviceRGB(r, g, b) {}
  
  friend
  std::ostream& operator<<(std::ostream& stream, const DPX& val);

  // use one of these to force desired sort order in assoc. containers of DPX
  bool
  operator<(const DPX& p) const
  {
    return (r_ != p.r_) ? (r_ < p.r_) : ((g_ != p.g_) ? (g_ < p.g_) : (b_ < p.b_));
  }
};

class Vector3d
{
public:
  Vector3d() : X_(0), Y_(0), Z_(0) {};
  Vector3d(double X, double Y, double Z) : X_(X), Y_(Y), Z_(Z) {}
  
  double
  X() const { return X_; }
  
  double
  Y() const { return Y_; }
  
  double
  Z() const { return Z_; }
  
private:
  double X_;
  double Y_;
  double Z_;
};

class Matrix3d
{
public:
  Matrix3d();
  Matrix3d
  ( double m00, double m01, double m02,
    double m10, double m11, double m12,
    double m20, double m21, double m22);
  
  //  // copy ctor and assignment operator
  //  Matrix3d(const Matrix3d& other);
  //  
  //  Matrix3d&
  //  operator=(const Matrix3d& rhs);
  
  Matrix3d
  operator*(double s) const;  
  
  double
  Determinant() const;
  
  Matrix3d
  Inverse() const;

  Vector3d
  operator*(const Vector3d& v) const;
  
private:
  double m00_;
  double m01_;
  double m02_;
  double m10_;
  double m11_;
  double m12_;
  double m20_;
  double m21_;
  double m22_;
};

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif
