#ifndef SLICE_ITER_H
#define SLICE_ITER_H

#include <iostream>
#include <valarray>
#include <complex>
#include "allinclude.h"

using namespace std;

/*!
 * @class sliceIter provides the functionality of  matrix acces in the form [i][j]
 * Idea B. Stroustrup
 *
 *
 */

// forward declarations to allow friend declarations
template<class T> class sliceIter;
template <class T> ostream& operator << (ostream& os, const sliceIter<T>& );
template <class T> istream& operator >> (istream& is, sliceIter<T>& );
template<class T> bool operator == ( const sliceIter<T>&, const sliceIter<T>& );
template<class T> bool operator != ( const sliceIter<T>&, const sliceIter<T>& );
template<class T> bool operator < ( const sliceIter<T>&, const sliceIter<T>& );
template<class T> bool operator > ( const sliceIter<T>&, const sliceIter<T>& );
template<class T> long int  operator - ( const sliceIter<T>&, const sliceIter<T>& );
template<class T> long int  operator + ( const sliceIter<T>&, const sliceIter<T>& );
template<class T> sliceIter<T>  operator + ( const sliceIter<T>&, const size_t& );
template<class T> sliceIter<T>  operator - ( const sliceIter<T>&, const size_t& );

#ifdef  QDEBUG_H
template <class T> QDebug operator << (QDebug dbg, const sliceIter<T>& );

#endif

template<class T>
class sliceIter
{
    valarray<T>* v; /*!< data to be organized in slices -> Matrix */
    slice s;
    size_t curr;   /*!< index of current element */

    T& ref ( size_t i ) const
    {
      return ( *v ) [s.start() + i*s.stride() ];
    }

  public:

    sliceIter ( valarray<T>* vv, slice ss ) :v ( vv ), s ( ss ), curr ( 0 ) { }




    /**
         * STL adaptor
         * @return  size
     */
    size_t size() const
    {
      return s.size();
    }


    /**
         * ++ increment pre
         * @return next element
     */
    sliceIter& operator ++ ()
    {
      curr++;
      return *this;
    }

    /**
         * increment ++ (post)
         * @param
         * @return next element
     */
    sliceIter  operator ++ ( int )
    {
      sliceIter t = *this;
      curr++;
      return t;
    }


    /**
    * operator myIter = presentIter + 5<br>
    * this is a NON checked operation; please avoid
    * maybe use: if (plus < presentIter.size()) myIter = presentIter + plus <br>
    *
    * @param p sliceIter
    * @param st lenght or size
    * @return sliceIter - an iterator to the new position
     */
    friend sliceIter  operator + ( const sliceIter<T>& p, const size_t& st )
    {

      sliceIter t = p;
      t.curr += st;
      return t;
    }


    /**
    *
    * operator myIter = presentIter - 5
    * this is a NON checked operation; please avoid; reference  + operator
    *
    *
    * @param p sliceIter
    * @param st lenght or size
    * @return sliceIter - an iterator to the new position
     */
    friend sliceIter  operator - ( const sliceIter<T>& p, const size_t& st )
    {

      sliceIter t = p;
      t.curr -= st;
      return t;
    }




    /*
    sliceIter& operator + () {
    curr++;
    return *this;
    }
    */
    /**
    * index operator
    * @param i
    * @return
     */
    T& operator [] ( size_t i )
    {
      return ref ( i );
    }

    /**
    *
    * @return de-reference - get the "number" of current element
     */
    T& operator * ()
    {
      return ref ( curr );
    }


    /**
                                                                   *
  * @return return the index of the first element<br>
     */
    sliceIter begin()
    {
      sliceIter t = *this;
      t.curr = 0;
      return t;
    }

    /**
    *  STL adaptor
    * @return  index of last-plus-one element
     */
    sliceIter end() const
    {
      sliceIter t = *this;
      t.curr = s.size();
      return t;
    }


    friend bool operator == <> ( const sliceIter& p, const sliceIter& q );
    friend bool operator != <> ( const sliceIter& p, const sliceIter& q );
    friend bool operator <  <> ( const sliceIter& p, const sliceIter& q );
    friend bool operator >  <> ( const sliceIter& p, const sliceIter& q );
    friend long int     operator -  <> ( const sliceIter& p, const sliceIter& q );
    friend long int     operator +  <> ( const sliceIter& p, const sliceIter& q );
    friend ostream& operator << <>(ostream& os, const sliceIter& si);
    friend istream& operator >> <>(istream& is, sliceIter& si);
#ifdef  QDEBUG_H
    friend QDebug operator << <>(QDebug dbg, const sliceIter<T>& si);

#endif
    
    

};


/**
 * cout // cerr overload for slice iter; use cerr << slice
 * @param os 
 * @param si 
 * @return 
 */
template <class T> ostream& operator << (ostream& os, const sliceIter<T>& si) {
  size_t i = 0;
     while (i < si.size()) os << si.ref(i++) << " ";
 return os;
}

template <class T> istream& operator >> (istream& is,  sliceIter<T>& si) {
  size_t i = 0;
  while (i < si.size()) is >> si[i++];
  return is;
}


/**
*   == operator needed for STL
* @param p
* @param q
* @return
 */
template<class T>
bool operator == ( const sliceIter<T>& p, const sliceIter<T>& q )
{
  return p.curr == q.curr
         && p.s.stride() == q.s.stride()
         && p.s.start() == q.s.start();
}

/**
*  != operator needed for STL
* @param p
* @param q
* @return
 */


template<class T>
bool operator != ( const sliceIter<T>& p, const sliceIter<T>& q )
{
  return ! ( p == q );
}


/**
  *  < operator needed for STL
  * @param p
  * @param q
  * @return
 */

template<class T>
bool operator < ( const sliceIter<T>& p, const sliceIter<T>& q )
{
  return p.curr < q.curr
         && p.s.stride() == q.s.stride()
         && p.s.start() == q.s.start();
}

/**
  *  > , convinience, not needed for STL because could be generated by !<
  * @param p
  * @param q
  * @return
 */

template<class T>
bool operator > ( const sliceIter<T>& p, const sliceIter<T>& q )
{
  return p.curr > q.curr
         && p.s.stride() == q.s.stride()
         && p.s.start() == q.s.start();
}

template<class T>  long int operator - ( const sliceIter<T>& p, const sliceIter<T>& q )
{

  return p.curr - q.curr;
}


/**
 * 
 * @param p 
 * @param q 
 * @return 
 */
template<class T>  long int operator + ( const sliceIter<T>& p, const sliceIter<T>& q )
{

  return p.curr + q.curr;
}

//****************************** USING QT ****************************
#ifdef  QDEBUG_H

template <class T> QDebug operator << (QDebug dbg, const sliceIter<T>& si) {
      size_t i = 0;
      while (i < si.size()) dbg.nospace() << si.ref(i++) << " ";
  return dbg.space();
};

#endif

#endif
