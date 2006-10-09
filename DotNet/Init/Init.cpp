
#include "Init.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

using namespace CadKit::Init::Glue;

Init::Init()
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
}
