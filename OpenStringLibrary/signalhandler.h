#ifndef __SIGNAL_HANDLER_H_
#define __SIGNAL_HANDLER_H_
#include <stdexcept>
using std::runtime_error

class cSignalException : public runtime_error {
	public:
		cSignalException(const std::string& _message): std::runtime_error(_message) {}
};

class cSignalHandler {
	protected:
		static bool signalstatus;
	public:
		cSignalHandler(int signum) {
			setupSignalHandler(signum);
		}
		~cSignalHandler() {}
		static bool gotSignal() { return signalstatus; }
   		static void setSignal(bool _signalstatus) { signalstatus=_signalstatus; }
		void setupSignalHandler(int signum) {
			if(signal(signum, cSignalHandler::signalHandler)==SIG_ERR) {
				throw SignalException("Error setting up signal handler");
			}
		}

		static void signalHandler(int _ignored) { signalstatus=true; }

};
#endif
          
