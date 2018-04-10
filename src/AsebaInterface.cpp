//
// Created by Rasmus Munk on 04/11/2017.
//

#include <include/AsebaInterface.h>
#include <aseba/compiler/compiler.h>
#include <aseba/common/utils/utils.h>
#include <QDebug>
#include "support/Message.h"

using namespace ENU::Http;

namespace ENU
{
	AsebaInterface::AsebaInterface(const QString& ip, const QString& port) :
		stream(nullptr),
		running(true),
		dashelParams(("tcp:" + ip + ";port=" + port))
	{
		try
		{
			stream = Dashel::Hub::connect(dashelParams.toStdString());
			std::cout << "Connected to: " << dashelParams.toStdString();
			pingNetwork();
			// Register the emit type
			qRegisterMetaType<Message>("Message");
			start();
		}
		catch (Dashel::DashelException& e)
		{
			std::cout << "Failed to connect to: " << dashelParams.toStdString();
			throw e;
		}
	}

	void AsebaInterface::sink(std::unique_ptr<AeslProgram> aeslProgram)
	{
		_aeslProgram = std::move(aeslProgram);
	}

	// register node in
	void AsebaInterface::setupNode(unsigned nodeId)
	{
		uploadAeslProgram(nodeId);
	}

	void AsebaInterface::stop()
	{
		running = false;
		Dashel::Hub::stop();
		quit();
	}

	void AsebaInterface::setVariable(const std::string& variable, const int& value)
	{
		bool ok;
		unsigned pos = getVariablePos(stoul(_aeslProgram->getEntry().nodeId),
			Aseba::UTF8ToWString(variable),
			&ok);
		if (ok)
		{
			Aseba::SetVariables::VariablesVector data;
			data.push_back(atoi(std::to_string(value).c_str()));
			Aseba::SetVariables setVariables(stoul(_aeslProgram->getEntry().nodeId), pos, data);
			sendMessage(setVariables);
		}
		else
		{
			std::cerr << "Couldn't find the variable: " << variable << endl;
		}
	}

	void AsebaInterface::getVariable(const std::string& variable)
	{
		bool pos_ok, size_ok;
		unsigned nodeId = stoul(_aeslProgram->getEntry().nodeId);
		unsigned pos = getVariablePos(nodeId, Aseba::UTF8ToWString(variable), &pos_ok);
		unsigned size = getVariableSize(nodeId, Aseba::UTF8ToWString(variable), &size_ok);
		if (pos_ok && size_ok)
		{
			Aseba::GetVariables getVariables(nodeId, pos, size);
			sendMessage(getVariables);
		}
		else
		{
			std::cerr << "Couldn't find the variable: " << variable << endl;
		}
	}

	// In QThread main function, we just make our Dashel hub switch listen for incoming data
	void AsebaInterface::run()
	{
		while (running)
		{
			Dashel::Hub::step(-1);
		}
	}

	void AsebaInterface::sendEvent(const std::string& event)
	{
        auto w_event = Aseba::UTF8ToWString(event);
	    auto events = _aeslProgram->getCommonDefinitions().events;
	    if (events.contains(w_event)) {
	        for (int i = 0; i < events.size(); ++i) {
	            if (events[i].name == w_event) {
	                // type is = index of the definition events
                    Aseba::UserMessage userMessage((uint16_t)i);
                    sendMessage(userMessage);
	            }
	        }
	    } else {
            std::wcerr << "Event: " << w_event << " is not available on node: "
                                           << Aseba::UTF8ToWString(_aeslProgram->getEntry().nodeName)
                                           << endl;
	    }
	}

	void AsebaInterface::incomingData(Dashel::Stream* stream)
	{
		Aseba::Message* message;
		try
		{
			message = Aseba::Message::receive(stream);
		}
		catch (Dashel::DashelException& e)
		{
			std::wcerr << "Error while reading message" << endl;
			delete message;
		}

		Aseba::NodesManager::processMessage(message);
		// if variables, print
		const Aseba::Variables* variables(dynamic_cast<Aseba::Variables*>(message));
		if (variables)
		{
			// as this is a shell, we just print the result of the variable
			// message as we assumes that it was linked to the last
			// GetVariables request, but in case a third-party is requesting
			// variables there might be many response before the one corresponding
			// to the query. In that case, the variables->start value
			// must be checked against the variable map
			std::wcerr << '\r';
			for (size_t i = 0; i < variables->variables.size(); ++i)
				std::wcerr << variables->variables[i] << " ";
		}

		// if user event, print
		const Aseba::UserMessage* userMessage(dynamic_cast<Aseba::UserMessage*>(message));
		if (userMessage)
		{
			Aseba::UserMessage incMessage(*userMessage);
			Aseba::NamedValue eventName = _aeslProgram->getCommonDefinitions().events[incMessage.type];
			Aseba::UserMessage::DataVector dataVector = incMessage.data;

			Message msg(eventName, dataVector);
			emit incommingUserMessage(msg);
		}

		delete message;
	}


	void AsebaInterface::connectionClosed(Dashel::Stream* stream, bool abnormal)
	{
		this->stream = nullptr;
	}

	void AsebaInterface::sendMessage(const Aseba::Message& message)
	{
		if (stream)
		{
			try
			{
				message.serialize(stream);
				stream->flush();
			}
			catch (Dashel::DashelException& e)
			{
				qDebug() << "Sending Message Failed: " << e.what() << endl;
			}
		}
	}

	void AsebaInterface::uploadAeslProgram(unsigned int node)
	{
		if (stream)
		{
			AeslProgram::NodeEntry nodeEntry = _aeslProgram->getEntry();
			if (nodeEntry.nodeId == std::to_string(node))
			{
				std::wistringstream is(Aseba::UTF8ToWString(nodeEntry.code));

				Aseba::Error error;
				Aseba::BytecodeVector bytecodeVector;
				unsigned allocatedVariablesCount;

				// Get Description
				bool ok;

				//compile
				Aseba::Compiler compiler;
				compiler.setTargetDescription(getDescription(node));
				compiler.setCommonDefinitions(&_aeslProgram->getCommonDefinitions());

				bool result = compiler.compile(is, bytecodeVector, allocatedVariablesCount, error);
				if (result)
				{
					// transfer bytecode
					Aseba::sendBytecode(stream, node, std::vector<uint16_t>(bytecodeVector.begin(),
                                                                            bytecodeVector.end()));
					Aseba::Run msg(node);
					sendMessage(msg);
					std::cout << "Node: " << nodeEntry.nodeId
                               << " name: " << nodeEntry.nodeName
                               << " was configured " << endl;
					// Signal that the interface has configured the robot with the Aesl Program
					emit configured();
				}
				else
				{
					std::wcerr << "complication for node " << Aseba::UTF8ToWString(nodeEntry.nodeName)
                               << " failed: " << error.toWString() << endl;
				}
			}
			else
			{
				std::wcerr << "Node: " << node << " dosen't exist" << endl;
			}
		}
	}

	void AsebaInterface::nodeDescriptionReceived(unsigned nodeId)
	{
		std::wcerr << '\r';
		std::wcerr << "Received description for node " << getNodeName(nodeId) << " " << endl;
		setupNode(nodeId);
	}
}