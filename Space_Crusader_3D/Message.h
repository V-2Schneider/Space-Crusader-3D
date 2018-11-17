#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H

template <class T>
class Message {
private:
	int to;
	int from;
	int type;
	T data;

	Message(int to, int from, int type, T data)
	{
		this.to = to;
		this.from = from;
		this.type = type;
		this.data = data;
	}
};

#endif // !MESSAGE_H
