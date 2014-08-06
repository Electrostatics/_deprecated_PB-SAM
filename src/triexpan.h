#ifndef _TRIEXPAN_H_
#define _TRIEXPAN_H_
#include "expansion.h"

//! CTriExpan class
/*!	This class is the base class for for gradients */
class CTriExpan
{
public:
	// Constructor
	CTriExpan( int p = 0, REAL scale=1.0 );  

	void rotate( const CQuat & Q, int p );
	void incRotate( const CQuat & Q );
	friend ostream & operator<<( ostream & out, const CTriExpan & exp );
	void outputComplex( REAL fact=1.0 ) const;
	
	// Inline functions
	const CExpan & operator[]( int c ) const
	{ assert( c >= 0 && c < 3 ); return m_M[c]; }
	CExpan & operator[]( int c )
	{ assert( c >= 0 && c < 3 ); return m_M[c]; }
	CTriExpan & operator+=( const CTriExpan & E )
	{ m_M[0]+=E.m_M[0]; m_M[1]+=E.m_M[1]; m_M[2]+=E.m_M[2]; return *this; }
	CTriExpan & operator-=( const CTriExpan & E )
	{ m_M[0]-=E.m_M[0]; m_M[1]-=E.m_M[1]; m_M[2]-=E.m_M[2]; return *this; }
	CTriExpan & operator*=( const REAL * C )
	{ m_M[0] *= C; m_M[1] *= C; m_M[2] *= C; return *this; }

	void setRange( int p )
	{ CRange R( 0,p ); setRange(R); }
	void setRange( CRange R )
	{ m_M[0].setRange( R ); m_M[1].setRange(R); m_M[2].setRange(R); }
	void setScale( REAL scale )
	{ m_M[0].setScale( scale ); m_M[1].setScale(scale); m_M[2].setScale(scale); }
	
	void reset( CRange R ) { m_M[0].reset(R); m_M[1].reset(R); m_M[2].reset(R); }
	void reset( int p = 0 ){ CRange R(0,p); reset(R); }
	void recip(  ) { m_M[0].recip(); m_M[1].recip(); m_M[2].recip(); }
	
	// Inline described below
	vector<REAL> getOneVector(  ) const ;
	void setFromOneVector( const vector<REAL> &V );
	static REAL computeDev( const CTriExpan & G1, const CTriExpan & G2 );
	
	friend CPnt inprod_unitScale( const CExpan & M1, const CTriExpan & M2 );
	friend CPnt inprod( const CMulExpan & M1, const CTriExpan & M2 );
	friend CPnt inprod( const CTriExpan & M1, const CLocalExpan & M2 );
	friend CPnt cross( const CTriExpan & G1, const CTriExpan & G2 );
	void copy( const CTriExpan & G, int p );
	void copy_p( const CTriExpan & G, int p );
	
	friend CTriExpan operator*( const CExpan E, const CPnt p );
	CTriExpan operator=( const CTriExpan & E );
	
	// Printing functions
	CRange getRange(  ) const
	{ return m_M[0].getRange(  ); }
	REAL getScale(  ) const
	{ return m_M[0].getScale(  ); }
	
protected:
	CExpan m_M[3];
};	// end class CTriExpan

/******************************************************************/
/******************************************************************/
/**
 * Function to return the vectors from each expansion in the triexpan
 concatenated into 1 long vector
 ******************************************************************/
inline vector<REAL> 
CTriExpan::getOneVector(  ) const
{
	vector<REAL> V;
	for( int c=0; c<3; c++ )
		V.insert( V.end( ), m_M[c].getVector().begin(), m_M[c].getVector().end());
	return V;
}	// end getOneVector

/******************************************************************/
/******************************************************************/
/**
 * Take one long vector and set the vectors in the expansions to
 1/3 of the input vector
 ******************************************************************/
inline void
CTriExpan::setFromOneVector( const vector<REAL> &V )
{
	const REAL* pV = &( V[0] );
	int len = V.size(  )/3;
	
	for( int c=0; c<3; c++, pV += len  )
		m_M[c].setVector(  pV, len  );
}	// end setFromOneVector

/******************************************************************/
/******************************************************************/
/**
 * Compute the deviation of the difference between 2 trio expansions
 ******************************************************************/
inline REAL
CTriExpan::computeDev( const CTriExpan & G1, const CTriExpan & G2 )
{
	return 
	( 
	 fabs ( CExpan::computeDev(G1.m_M[0], G2.m_M[0] ) ) +
	 fabs ( CExpan::computeDev(G1.m_M[1], G2.m_M[1] ) ) +
	 fabs ( CExpan::computeDev(G1.m_M[2], G2.m_M[2] ))
	 );
}	// end computeDev

/******************************************************************/
/******************************************************************/
/**
 * Compute the inner product of 1 expansion times each component of 
 a tri expansion with a scale of 1.0
 ******************************************************************/
inline CPnt
inprod_unitScale( const CExpan & M1, const CTriExpan & M2 )
{
	assert( M1.getRange( ).p2() == M2.getRange().p2());
	return CPnt( inprod_unitScale(M1, M2[0] ), inprod_unitScale(M1, M2[1]), 
							inprod_unitScale( M1, M2[2] ));
}	// end inprod_unitScale

/******************************************************************/
/******************************************************************/
/**
 * Compute the inner product of 1 expansion times each component of
 a tri expansion with a scale given by the tri expansion
 ******************************************************************/
inline CPnt
inprod( const CMulExpan & M1, const CTriExpan & M2 )
{
	assert( M1.getRange( ).p2() == M2.getRange().p2());
	
	// Make more efficient later
	CLocalExpan tL[3];
	tL[0] = CLocalExpan( M2[0].getVector( ), M2.getRange(), M2.getScale()); 
	tL[1] = CLocalExpan( M2[1].getVector( ), M2.getRange(), M2.getScale()); 
	tL[2] = CLocalExpan( M2[2].getVector( ), M2.getRange(), M2.getScale()); 
	
	return CPnt( inprod(M1, tL[0] ), inprod(M1, tL[1]), 
							inprod( M1, tL[2] ));
}	// end inprod

/******************************************************************/
/******************************************************************/
/**
 * Compute the inner product of a local expansion times each 
 component of a tri expansion with a scale given by the tri expansion
 ******************************************************************/
inline CPnt
inprod( const CTriExpan & M1, const CLocalExpan & M2 )
{
	assert( M1.getRange( ).p2() == M2.getRange().p2());
	
	// Make more efficient later
	CMulExpan tM[3];
	tM[0] = CMulExpan( M1[0].getVector( ), M1.getRange(), M1.getScale()); 
	tM[1] = CMulExpan( M1[1].getVector( ), M1.getRange(), M1.getScale()); 
	tM[2] = CMulExpan( M1[2].getVector( ), M1.getRange(), M1.getScale()); 
	
	return CPnt( inprod(tM[0],M2 ), inprod(tM[1],M2), 
							inprod( tM[2],M2 ));
}	// end inprod

/******************************************************************/
/******************************************************************/
/**
 * Compute the cross product between two tri expansions
 ******************************************************************/
inline CPnt 
cross( const CTriExpan & G1, const CTriExpan & G2 )
{
	assert( G1.getRange( ).p2() == G2.getRange().p2());
	
	CMulExpan tM[3];
	tM[0] = CMulExpan( G1[0].getVector( ), G1.getRange(), G1.getScale()); 
	tM[1] = CMulExpan( G1[1].getVector( ), G1.getRange(), G1.getScale()); 
	tM[2] = CMulExpan( G1[2].getVector( ), G1.getRange(), G1.getScale()); 
	
	CLocalExpan tL[3];
	tL[0] = CLocalExpan( G2[0].getVector( ), G2.getRange(), G2.getScale()); 
	tL[1] = CLocalExpan( G2[1].getVector( ), G2.getRange(), G2.getScale()); 
	tL[2] = CLocalExpan( G2[2].getVector( ), G2.getRange(), G2.getScale()); 
	
	CPnt res( inprod(tM[1], tL[2] ) - inprod(tM[2],tL[1]),
					 inprod( tM[2], tL[0] ) - inprod(tM[0],tL[2]),
					 inprod( tM[0], tL[1] ) - inprod(tM[1],tL[0]));
	
	return res;
}	// end cross

/******************************************************************/
/******************************************************************/
/**
 * Copy the number of poles and the tri expansion to current obj
 ******************************************************************/
inline void 
CTriExpan::copy( const CTriExpan & G, int p )
{ 
	m_M[0].copy( G.m_M[0],p ); 
	m_M[1].copy( G.m_M[1],p ); 
	m_M[2].copy( G.m_M[2],p );
}	// end copy

/******************************************************************/
/******************************************************************/
/**
 * Copy the number of poles and the tri expansion to current obj
 ******************************************************************/
inline void 
CTriExpan::copy_p( const CTriExpan & G, int p )
{ 
	m_M[0].copy_p( G.m_M[0],p ); 
	m_M[1].copy_p( G.m_M[1],p ); 
	m_M[2].copy_p( G.m_M[2],p );
} /// end copy_p

/******************************************************************/
/******************************************************************/
/**  * Create a triexpansion by multiplying each part of cartesian
 coordinates p, by E an expansion object.
 ******************************************************************/
inline CTriExpan 
operator*( const CExpan E, const CPnt p )
{ 
	CTriExpan N;
	N.setScale( E.getScale( ));
	N.setRange( E.getRange( ).p2());
	
	N.m_M[0] = p.x(  )*E; 
	N.m_M[1] = p.y(  )*E; 
	N.m_M[2] = p.z(  )*E; 
	return N; 
} 	// operator*

/******************************************************************/
/******************************************************************/
/**  * The triexpansion = operator
 ******************************************************************/
inline CTriExpan 
CTriExpan::operator=( const CTriExpan & E )
{  
	setScale( E.getScale( ));
	setRange( E.getRange( ).p2());
	m_M[0] = E.m_M[0]; 
	m_M[1] = E.m_M[1]; 
	m_M[2] = E.m_M[2]; 
	return *this; 
} 	// operator=

/******************************************************************/
/******************************************************************/
/**  * CGradExpan: Not sure whether necessary but define anyway
 ******************************************************************/
class CGradExpan : public CTriExpan
{
public: 
	CGradExpan( int p = 0, int res = N_POLES ) : CTriExpan(p, res) {} 
	CGradExpan( const CPnt* g, /*const vector<CPnt> &g,*/ const vector<CPnt> &pos, 
						 int p, bool bKappa, REAL scale, bool bMultipole );
	
	CGradExpan( const CTriExpan & G ) : CTriExpan(G) {} 
	CGradExpan sphToCart( const CPnt * R );
};	// class CGradExpan : public CTriExpan

/******************************************************************/
/******************************************************************/
/** * Convert spherical derivatives to cartesian coords
 ******************************************************************/
inline CGradExpan
CGradExpan::sphToCart( const CPnt * R )
{
	CGradExpan G( getRange( ).p2());
	for ( int i = 0; i < 3; i++ )
	{
		G.m_M[0] += R[i].x(  )*m_M[i];
		G.m_M[1] += R[i].y(  )*m_M[i];
		G.m_M[2] += R[i].z(  )*m_M[i];
	}
	return G;
}	// sphToCart

#endif
