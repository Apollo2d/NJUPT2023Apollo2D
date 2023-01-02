/* -*-C++-*-
 *Header:
 *File: object.h (for C++ & cc)
 *Author: Noda Itsuki
 *Date: 1995/02/21
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */


/*
 *ModifyHistory
 *
 *EndModifyHistory
 */


#ifndef RCSSSERVER_OBJECT_H
#define RCSSSERVER_OBJECT_H

#include <string>
#include <memory>
#include <iostream>
#include <cmath>

/*
 *===================================================================
 *Part: Plain Vector
 *===================================================================
 */
class PVector {
public:
    double x;
    double y;

    explicit
    PVector( const double & xx = 0.0,
             const double & yy = 0.0 )
        : x( xx )
        , y( yy )
      { }

    const PVector & assign( const double & xx,
                            const double & yy )
      {
          x = xx;
          y = yy;
          return *this;
      }

    PVector	operator-() const
      {
          return PVector( -x, -y );
      }

    const PVector & operator+() const
      {
          return *this;
      }

    const PVector & operator+=( const PVector & v )
      {
          x += v.x;
          y += v.y;
          return *this;
      }

    const PVector & operator-=( const PVector & v )
      {
          x -= v.x;
          y -= v.y;
          return *this;
      }

    const PVector & operator*=( const double & a )
      {
          x *= a;
          y *= a;
          return *this;
      }

    const PVector & operator/=( const double & a )
      {
          x /= a;
          y /= a;
          return *this;
      }

    double r2() const
      {
          return x*x + y*y;
      }

    double r() const
      {
          return std::sqrt( x*x + y*y );
      }

    double th() const
      {
          return ( (x == 0.0) && ( y == 0.0 )
                   ? 0.0
                   : std::atan2( y, x ) );
      }

    const PVector & normalize( const double l = 1.0 )
      {
          *this *= ( l / std::max( r(), rcsc::EPS ) );
          return *this;
      }

    double distance2( const PVector & orig ) const
      {
          return ( PVector( *this ) -= orig ).r2();
      }

    double distance( const PVector & orig ) const
      {
          return ( PVector( *this ) -= orig ).r();
      }

    const PVector & rotate( const double ang )
      {
          double c = std::cos( ang );
          double s = std::sin( ang );
          return assign( this->x * c - this->y * s,
                         this->x * s + this->y * c );
      }

    bool between( const PVector & begin,
                  const PVector & end ) const;

    static
    PVector fromPolar( const double & r,
                       const double & ang )
      {
          return PVector( r * std::cos( ang ),
                          r * std::sin( ang ) );
      }

};

inline
bool
operator==( const PVector & lhs,
            const PVector & rhs )
{
    return ( lhs.x == rhs.x && lhs.y == rhs.y );
}

inline
bool
operator!=( const PVector & lhs,
            const PVector & rhs )
{
    return !( lhs == rhs );
}

inline
PVector
operator+( const PVector & lhs,
           const PVector & rhs )
{
    return PVector( lhs ) += rhs;
}

inline
PVector
operator-( const PVector & lhs,
           const PVector & rhs )
{
    return PVector( lhs ) -= rhs;
}


inline
std::ostream &
operator<<( std::ostream & o,
            const PVector & v )
{
    return o << "#V[" << v.x << "," << v.y << "]";
}



// inline
// double
// PVector::vangle( const PVector & target,
//                  const PVector & origin ) const
// {
//     return ( origin - *this ).angle( target - *this );
// }

// inline
// double
// PVector::vangle( const PVector & target,
//                  const double & origin ) const
// {
//     return normalize_angle( ( target - *this ).angle() - origin );
// }




/*
 *===================================================================
 *Part: Area
 *===================================================================
 */
class RArea {
private:
    double M_left;
    double M_right;
    double M_top;
    double M_bottom;

    // not used
    RArea() = delete;
public:
    RArea( const double & l,
           const double & r,
           const double & t,
           const double & b )
        : M_left( l ),
          M_right( r ),
          M_top( t ),
          M_bottom( b )
      { }

    RArea( const PVector & center,
           const PVector & size )
        : M_left( center.x - size.x*0.5 ),
          M_right( center.x + size.x*0.5 ),
          M_top( center.y - size.y*0.5 ),
          M_bottom( center.y + size.y*0.5 )
      { }

    const double & left() const { return M_left; }
    const double & right() const { return M_right; }
    const double & top() const { return M_top; }
    const double & bottom() const { return M_bottom; }

    bool inArea( const PVector & p ) const
      {
          return ( p.x >= left() )
              && ( p.x <= right() )
              && ( p.y >= top() )
              && ( p.y <= bottom() );
      }

    /*! find nearest horizontal line */
    PVector nearestHEdge( const PVector & p ) const;

		/*! find nearest vertical line */
    PVector nearestVEdge( const PVector & p ) const;

    PVector nearestEdge( const PVector & p ) const;

    PVector randomize() const;

    std::ostream & print( std::ostream & o ) const;
};

#endif // _H_OBJECT
