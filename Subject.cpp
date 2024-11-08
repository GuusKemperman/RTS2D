#include "precomp.h"
#include "Subject.h"
#include "Observer.h"

void Subject::AddObserver(Observer* observer)
{
	observers.push_back(observer);
}

void Subject::RemoveObserver(Observer* observer)
{
	observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::Notify()
{
	for (Observer* observer : observers)
	{
		observer->ObservationMade();
	}
}
