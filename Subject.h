#pragma once

class Observer;

class Subject
{
public:
	virtual ~Subject() {};

	void AddObserver(Observer* observer);
	void RemoveObserver(Observer* observer);
	void Notify();
private:
	vector<Observer*> observers;
};

