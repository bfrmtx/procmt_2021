


#if ! defined (ffft_def_HEADER_INCLUDED)
#define	ffft_def_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace ffft
{



const double	PI		= 3.1415926535897932384626433832795;
const double	SQRT2	= 1.41421356237309514547462185873883;

#if defined (_MSC_VER)

	#define	ffft_FORCEINLINE	__forceinline

#else

	#define	ffft_FORCEINLINE	inline

#endif



}	// namespace ffft



#endif	// ffft_def_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
