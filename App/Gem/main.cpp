#include "System.h"

int main(void) {
	System* pSystem;

	pSystem = new System;
	if (!pSystem) {
		return 0;
	}

	if (!pSystem->Initialize()) {
		delete pSystem;
		return 0;
	}

	pSystem->Run();
	pSystem->Shutdown();

	delete pSystem;
	return 0;
}