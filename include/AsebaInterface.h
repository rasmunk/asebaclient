//
// Created by Rasmus Munk on 04/11/2017.
//

#ifndef DBUS_TEST_ASEBAINTERFACE_H
#define DBUS_TEST_ASEBAINTERFACE_H

#include <Qthread>
#include <memory>
#include <dashel/dashel.h>
#include <aseba/common/msg/NodesManager.h>
#include <asebaclient/support/AeslProgram.h>
#include <asebaclient/support/Message.h>

namespace ENU
{

	class AsebaInterface : public QThread, public Dashel::Hub, public Aseba::NodesManager
	{
		Q_OBJECT

	public:
		AsebaInterface(const QString& ip, const QString& port);
		~AsebaInterface() { this->stop(); }
		void sink(std::unique_ptr<ENU::Http::AeslProgram> aeslProgram);

		// NodesManager
		void sendMessage(const Aseba::Message& message) override;
		void uploadAeslProgram(unsigned nodeId);
		void nodeDescriptionReceived(unsigned nodeId) override;

		// Call DashelHub
		virtual void stop();

		void sendEvent(const std::string& event);
		void setVariable(const std::string& variable, const int& value);
		void getVariable(const std::string& variable);

	signals:
		// Signal to let the connecting object know that the interface has configure the Aseba Target
		void configured();
		void incommingUserMessage(const Message& message);

	protected:
		// QThread
		void run() override;

		// From Dashel::Hub
		void incomingData(Dashel::Stream* stream) override;
		void connectionClosed(Dashel::Stream* stream, bool abnormal) override;

		// NodesManager
		// TODO -> Optimize with collection of streams to support multiple controllers
		Dashel::Stream* stream;
		QString dashelParams;
		bool running;

	private:
		std::unique_ptr<ENU::Http::AeslProgram> _aeslProgram;
		void setupNode(unsigned nodeId);
	};
}


#endif //DBUS_TEST_ASEBAINTERFACE_H
