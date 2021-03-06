#include "Config.h"
#include "RequestHandler.h"
#include "SQLRequestGenericHandler.h"
#include "RequestHandlerFactory.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/ThreadPool.h"
#include <boost/lexical_cast.hpp>
#include <iostream>


using Poco::Util::Option;
using Poco::Util::OptionSet;


namespace HSServer {


// Declaration only.
Poco::Logger & GetLogger();


class HighScoreServer : public Poco::Util::ServerApplication
{
public:
    HighScoreServer() :
        mFactory(0),
        mPortNumber(9090)
    {
    }

protected:
    static int GetPortFromArgs(const std::vector<std::string>& args, int inDefault)
    {
        for (size_t idx = 0; (idx + 1) < args.size(); ++idx)
        {
            if (args[idx] == "-p" || args[idx] == "--port")
            {
                return boost::lexical_cast<int>(args[idx + 1]);
            }
        }
        return inDefault;
    }

    void handleOption(const std::string& name, const std::string& value)
    {
        ServerApplication::handleOption(name, value);

        if (name == "port")
        {
            try
            {
                mPortNumber = boost::lexical_cast<unsigned int>(value);
            }
            catch (const boost::bad_lexical_cast &)
            {
                throw std::runtime_error("Invalid port number: " + value);
            }
        }
    }

    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
            Option("port", "p", "Port number for incoming HTTP requests")
                .required(false)
                .repeatable(false)
                .argument("port"));
    }

    int main(const std::vector<std::string>& args)
    {
        Poco::Data::SQLite::Connector::registerConnector();


        int maxQueued  = config().getInt("HighScoreServer.maxQueued", 100);

        // Only allow one thread because the database using simple locking
        int maxThreads = config().getInt("HighScoreServer.maxThreads", 1);
        Poco::ThreadPool::defaultPool().addCapacity(maxThreads);

        Poco::Net::HTTPServerParams * params = new Poco::Net::HTTPServerParams;
        params->setMaxQueued(maxQueued);
        params->setMaxThreads(maxThreads);

        Poco::Net::ServerSocket serverSocket(mPortNumber);
        std::cout << "Listening to port " << mPortNumber << std::endl;

        mFactory = new RequestHandlerFactory;
        RegisterRequestHandlers(*mFactory);

        // Ownership of the factory is passed to the HTTP server.
        Poco::Net::HTTPServer httpServer(mFactory, serverSocket, params);
        httpServer.start();
        waitForTerminationRequest();
        httpServer.stop();
        return Poco::Util::Application::EXIT_OK;
    }

private:

    RequestHandlerFactory * mFactory;
    int mPortNumber;
};


} // namespace HSServer


using namespace HSServer;


int main(int argc, char** argv)
{
    int result = 0;
    try
    {
        HSServer::HighScoreServer app;
        result = app.run(argc, argv);
    }
    catch (const std::exception & exc)
    {
    #ifdef _WIN32
        ::MessageBoxA(0, exc.what(), "High Score Server", MB_OK);
    #else
        std::cout << exc.what() << std::endl;
    #endif
    }
    return result;
}


