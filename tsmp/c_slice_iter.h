#ifndef CSLICE_ITER_H
#define CSLICE_ITER_H

#include <iostream>
#include <valarray>
#include <complex>
#include "allinclude.h"



using namespace std;

/*!
 * @class csliceIter provides the functionality of  matrix acces in the form [i][j]
 * Idea B. Stroustrup
 * const version of sliceIter - see slice_iter for documentation
 *
 */

// forward declarations to allow friend declarations:
template<class T> class csliceIter;
template <class T> ostream& operator << (ostream& os, const csliceIter<T>& );
template<class T> bool operator == ( const csliceIter<T>&, const csliceIter<T>& );
template<class T> bool operator != ( const csliceIter<T>&, const csliceIter<T>& );
template<class T> bool operator < ( const csliceIter<T>&, const csliceIter<T>& );
template<class T> long int  operator - ( const csliceIter<T>&, const csliceIter<T>& );
template<class T> long int  operator + ( const csliceIter<T>&, const csliceIter<T>& );
template<class T> csliceIter<T>  operator + ( const csliceIter<T>&, const size_t& );
template<class T> csliceIter<T>  operator - ( const csliceIter<T>&, const size_t& );

#ifdef  QDEBUG_H
template <class T> QDebug operator << (QDebug dbg, const sliceIter<T>& );

#endif



template<class T>
class csliceIter
{
    valarray<T>* v;
    slice s;
    size_t curr; // index of current element
    const T& ref ( size_t i ) const
    {
      return ( *v ) [s.start() + i*s.stride() ];
    }
  public:
    csliceIter ( valarray<T>* vv, slice ss ) : v ( vv ), s ( ss ), curr ( 0 ) {}

    csliceIter end() const
    {

      csliceIter t = *this;
      t.curr = s.size(); // index of one plus last element
      return t;
    }

    size_t size() const
    {
      return s.size();
    }

    csliceIter begin()
    {
      csliceIter t = *this;
      t.curr = 0;
      return t;
    }



    csliceIter& operator ++ ()
    {
      curr++;
      return *this;
    }
    csliceIter  operator ++ ( int )
    {
      csliceIter t = *this;
      curr++;
      return t;
    }

    friend csliceIter  operator + ( const csliceIter<T>& p, const size_t& st )
    {

      csliceIter t = p;
      t.curr += st;
      return t;
    }

    friend csliceIter  operator - ( const csliceIter<T>& p, const size_t& st )
    {

      csliceIter t = p;
      t.curr -= st;
      return t;
    }


    const T& operator [] ( size_t i ) const
    {
      return ref ( i );
    }
    /*
            const T& operator ()(size_t i) const {
            return ref(i);
        }

    */
    const T& operator * ()         const
    {
      return ref ( curr );
    }

    friend bool operator == <> ( const csliceIter& p, const csliceIter& q );
    friend bool operator != <> ( const csliceIter& p, const csliceIter& q );
    friend bool operator <  <> ( const csliceIter& p, const csliceIter& q );
    friend long int     operator -  <> ( const csliceIter& p, const csliceIter& q );
    friend long int     operator +  <> ( const csliceIter& p, const csliceIter& q );
    friend ostream& operator << <>(ostream& os, const csliceIter& si);
    
#ifdef  QDEBUG_H
    friend QDebug operator << <>(QDebug dbg, const sliceIter<T>& si);

#endif
    
    

};


template <class T> ostream& operator << (ostream& os, const csliceIter<T>& si) {
  
  size_t i = 0;
  while (i < si.size()) os << si.ref(i++) << " ";
  return os;
}

template<class T>
bool operator == ( const csliceIter<T>& p, const csliceIter<T>& q )
{
  return p.curr == q.curr
         && p.s.stride() == q.s.stride()
         && p.s.start() == q.s.start();
}

template<class T>
bool operator != ( const csliceIter<T>& p, const csliceIter<T>& q )
{
  return ! ( p == q );
}

template<class T>
bool operator< ( const csliceIter<T>& p, const csliceIter<T>& q )
{
  return p.curr < q.curr
         && p.s.stride() == q.s.stride()
         && p.s.start()  == q.s.start();
}


template<class T>  long int operator - ( const csliceIter<T>& p, const csliceIter<T>& q )
{

  return p.curr - q.curr;
}


template<class T>  long int operator + ( const csliceIter<T>& p, const csliceIter<T>& q )
{

  return p.curr + q.curr;
}


//****************************** USING QT ****************************
#ifdef  QDEBUG_H

template <class T> QDebug operator << (QDebug dbg, const csliceIter<T>& si) {
  size_t i = 0;
  while (i < si.size()) dbg.nospace() << si.ref(i++) << " ";
  return dbg.space();
};

#endif



#endif
