#pragma once

#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include <deque>

struct MessageB {
	char Adresat;
	//jeszcze muszê przemyœleæ, jakie tutaj dane!
};

class Message_Bus {
private:
	std::deque<int> Input;
public:
	void Start_Listening() {
		while (true)
		{
			
		}
	}
};

#endif