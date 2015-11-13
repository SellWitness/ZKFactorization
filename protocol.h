
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <exception>
#include <string>

class ProtocolException: public std::exception {
	std::string msg;
	public:
	ProtocolException(const std::string &msg):msg(msg){
	}
	virtual const char* what() const throw(){
		return msg.c_str();
	}
};

// Define protocol and classes A and B.
// For the protocol to be used in cut and choose
// B needs to define bool field valid.

template<typename A, typename B>
class Protocol {
	public:
	virtual void init(A *, B *) = 0;
	virtual void exec(A *, B *) = 0;
	virtual void open(A *, B *) = 0;
};

#endif
