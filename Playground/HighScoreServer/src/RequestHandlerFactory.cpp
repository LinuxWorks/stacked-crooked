#include "RequestHandlerFactory.h"
#include "ContentType.h"
#include "RequestHandler.h"
#include "SQLRequestGenericHandler.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"


namespace HSServer
{

    std::string GetLocation(const Poco::Net::HTTPServerRequest & inRequest)
    {
        const std::string & uri = inRequest.getURI();
        if (uri.empty())
        {
            throw std::runtime_error("URI is empty.");
        }
        return uri.substr(1, uri.find("?"));
    }


    std::string GetLocationWithoutExtension(const Poco::Net::HTTPServerRequest & inRequest)
    {
        std::string location = GetLocation(inRequest);
        return location.substr(0, location.find("."));
    }


    /**
     * Returns the "file type" of a url.
     *
     * For example:
     *   /hs.html => html
     *   /hs.xml => xml
     */
    std::string GetFileType(const std::string & inLocation)
    {
        std::string::size_type offset = inLocation.find(".");
        if (offset == std::string::npos || offset == inLocation.size() - 1)
        {
            return "";
        }
        return inLocation.substr(offset + 1);
    }


    bool ConvertFileTypeToContentType(const std::string & inText, ContentType & outContentType)
    {
        typedef std::map<std::string, ContentType> Mapping;
        static Mapping fMapping;
        if (fMapping.empty())
        {
            fMapping["html"] = ContentType_TextHTML;
            fMapping["xml"] = ContentType_ApplicationXML;
            fMapping["txt"] = ContentType_TextPlain;
        }

        Mapping::iterator it = fMapping.find(inText);
        if (it != fMapping.end())
        {
            outContentType = it->second;
            return true;
        }
        return false;
    }


    /**
     * The request usually contains multiple content types. We select one
     * according the the following priority list:
     *   - text/html
     *   - application/xml
     *   - text/plain
     */
    ContentType GetContentType(const Poco::Net::HTTPServerRequest & inRequest)
    {
        ContentType result = ContentType_Unknown;
        if (ConvertFileTypeToContentType(GetFileType(GetLocation(inRequest)), result))
        {
            return result;
        }

        const std::string & acceptHeader = inRequest.get("Accept");

        if (acceptHeader.find("text/html") != std::string::npos)
        {
            return ContentType_TextHTML;
        }
        else if (acceptHeader.find("application/xml") != std::string::npos)
        {
            return ContentType_ApplicationXML;
        }
        else if (acceptHeader.find("text/plain") != std::string::npos)
        {
            return ContentType_TextPlain;
        }
        throw std::runtime_error("Unsupported content type: " + acceptHeader);
    }


    ResourceId GetResourceId(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return ResourceManager::Instance().getResourceId(GetLocationWithoutExtension(inRequest));
    }


    Method GetMethod(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return ParseMethod(inRequest.getMethod());
    }


    RequestHandlerId GetRequestHandlerId(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return RequestHandlerId(GetResourceId(inRequest),
                                GetMethod(inRequest),
                                GetContentType(inRequest));
    }


    Poco::Net::HTTPRequestHandler *
    RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& inRequest)
    {
        // Log the request
        Poco::Logger & fLogger = Poco::Util::Application::instance().logger();

        try
        {
            if (inRequest.getURI() == "/favicon.ico")
            {
                std::string dump;
                const_cast<Poco::Net::HTTPServerRequest&>(inRequest).stream() >> dump;
                throw new HTMLErrorResponse("Favicon is not supported.");
            }
            fLogger.information("---");
            fLogger.information("Request with uri: " + inRequest.getURI());
            fLogger.information("Request id: " + ToString(GetRequestHandlerId(inRequest)));

            FactoryFunctions::iterator it = mFactoryFunctions.find(GetRequestHandlerId(inRequest));
            if (it != mFactoryFunctions.end())
            {
                const FactoryFunction & ff(it->second);
                return ff();
            }
            else
            {
                fLogger.warning("No handler found");

                // HACK (?)
                // For some mysterious reason the next request sometimes
                // contains the current request body. Therefore we flush
                // it here.
                std::string dump;
                const_cast<Poco::Net::HTTPServerRequest&>(inRequest).stream() >> dump;
            }
        }
        catch (const std::exception & inException)
        {
            fLogger.error(inException.what());
        }
        return new HTMLErrorResponse("No handler for " + inRequest.getURI());
    }

} // namespace HSServer
