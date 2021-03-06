#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/StreamCopier.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Condition.h"
#include "Poco/Mutex.h"
#include <functional>
#include <iostream>
#include <cstdlib>
#include <thread>


typedef Poco::Net::HTTPServerRequest Request;
typedef Poco::Net::HTTPServerResponse Response;


class HTTPServer: public Poco::Util::ServerApplication
{
public:
    typedef std::function<void(Request&, Response&)> HandleRequest;

    HTTPServer(const HandleRequest & inHandleRequest) : mHandleRequest(inHandleRequest)
    {
    }

private:
    int main(const std::vector<std::string>& )
    {
        Poco::ThreadPool::defaultPool().addCapacity(16);
        Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
        pParams->setMaxQueued(100);
        pParams->setMaxThreads(Poco::ThreadPool::defaultPool().capacity());

        // set-up a server socket
        Poco::Net::ServerSocket svs(8080);


        struct RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
        {
            RequestHandlerFactory(const HandleRequest & inHandleRequest) : mHandleRequest(inHandleRequest) {}

            Poco::Net::HTTPRequestHandler* createRequestHandler(const Request&)
            {
                struct RequestHandler: public Poco::Net::HTTPRequestHandler
                {
                    RequestHandler(const HandleRequest & inHandleRequest) : mHandleRequest(inHandleRequest) {}

                    void handleRequest(Request& request, Response& response)
                    {
                        mHandleRequest(request, response);
                    }

                    HandleRequest mHandleRequest;
                };
                return new RequestHandler(mHandleRequest);
            }

            HandleRequest mHandleRequest;
        };

        // set-up a HTTPServer instance
        Poco::Net::HTTPServer srv(new RequestHandlerFactory(mHandleRequest), svs, pParams);

        // start the HTTPServer
        srv.start();

        // wait for CTRL-C or kill
        waitForTerminationRequest();

        // Stop the HTTPServer
        srv.stop();

        return Application::EXIT_OK;
    }

    HandleRequest mHandleRequest;
};


struct Dispatcher : HTTPServer
{
    Dispatcher() :
        HTTPServer(std::bind(&Dispatcher::handleRequest,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2)),
        mSolicitor()
    {
    }

    void solicit(Request&, Response& response)
    {
        Poco::ScopedLock<Poco::Mutex> lock(mRequestMutex);
        mSolicitor = true;
        std::shared_ptr<void> unset(nullptr, [this](void*){ mSolicitor = false; });
        mRequestCondition.wait(mRequestMutex);
        response.send() << mRequest;
    }

    void result(Request& request, Response& response)
    {
        Poco::ScopedLock<Poco::Mutex> lock(mResultMutex);
        mResult = toString(request.stream());
        mResultCondition.signal();
        response.send() << std::endl;
    }

    void client(Request& request, Response& response)
    {
        Poco::ScopedLock<Poco::Mutex> clientLock(mClientMutex);
        {
            Poco::ScopedLock<Poco::Mutex> requestLock(mRequestMutex);
            if (!mSolicitor)
            {
                response.send() << "No solicitor!" << std::endl;
                return;
            }
            mRequest = toString(request.stream());
            mRequestCondition.signal();
        }

        {
            Poco::ScopedLock<Poco::Mutex> resultLock(mResultMutex);
            mResultCondition.wait(mResultMutex);
            response.send() << mResult;
        }
    }

    static std::string toString(std::istream& stream)
    {
         return std::string(std::istreambuf_iterator<char>(stream),
                            std::istreambuf_iterator<char>());
    }

    void handleRequest(Request& request, Response& response)
    {
        const std::string & uri = request.getURI();
        if (!uri.find("/solicit"))
        {
            solicit(request, response);
        }
        else if (!uri.find("/result"))
        {
            result(request, response);
        }
        else
        {
            client(request, response);
        }
    }

    Poco::Mutex mClientMutex;

    Poco::Condition mResultCondition;
    Poco::Mutex mResultMutex;
    std::string mResult;

    Poco::Condition mRequestCondition;
    Poco::Mutex mRequestMutex;
    std::string mRequest;

    bool mSolicitor;
};



int main(int argc, char** argv)
{
    Dispatcher app;
    return app.run(argc, argv);
}
