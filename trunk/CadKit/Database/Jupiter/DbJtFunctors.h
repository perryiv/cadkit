
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtFunctors: Functors used in this library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_FUNCTORS_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_FUNCTORS_H_


namespace CadKit
{


///////////////////////////////////////////////////////////////////////////////
//
//  DbJtFunctor. Base class.
//
///////////////////////////////////////////////////////////////////////////////

template <class E, class I1, class I2> class DbJtFunctor
{
public:

  DbJtFunctor ( E entity, I1 *use, I2 *pass );
  virtual ~DbJtFunctor();

  virtual bool operator()() = 0;

protected:

  E _entity;
  SlRefPtr<I1> _use;
  SlRefPtr<I2> _pass;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class E, class I1, class I2> inline DbJtFunctor::DbJtFunctor ( E entity, I1 *use, I2 *pass ) : 
  _entity ( entity ),
  _use ( use ),
  _pass ( pass )
{
  SL_ASSERT ( _use.isValid() );
  SL_ASSERT ( _pass.isValid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbJtNotifyStartFunctor.
//
///////////////////////////////////////////////////////////////////////////////

template <class E, class I1, class I2> class DbJtNotifyStartFunctor : public DbJtFunctor<E,I1,I2>
{
public:

  DbJtNotifyStartFunctor ( E entity, I1 *use, I2 *pass ) : DbJtFunctor ( E, I1, I2 ){}
  virtual bool operator()() { _use->startNotify ( _entity, _pass ); }
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbJtNotifyEndFunctor.
//
///////////////////////////////////////////////////////////////////////////////

template <class E, class I1, class I2> class DbJtNotifyEndFunctor : public DbJtFunctor<E,I1,I2>
{
public:

  DbJtNotifyEndFunctor ( E entity, I1 *use, I2 *pass ) : DbJtFunctor ( E, I1, I2 ){}
  virtual bool operator()() { _use->endNotify ( _entity, _pass ); }
};

Inline virtual functions... does it matter?
Use these functors in DbJtDatabase::_startAssembly() and friends.

}; // namespace CadKit


#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_FUNCTORS_H_
