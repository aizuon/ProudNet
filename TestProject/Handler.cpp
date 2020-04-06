#include "Handler.h"

using namespace Nettention::Proud;

Handler::Handler(NetClient* instance)
{
	Instance = instance;
}

NetClient& Handler::GetClient()
{
	return *Instance;
}
