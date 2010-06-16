#include "RequestHandler.h"
#include "FileUtils.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Util/Application.h"
#include "Poco/StreamCopier.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <sstream>


using namespace Poco::Data;


namespace HSServer
{

    MissingArgumentException::MissingArgumentException(const std::string & inMessage) :
        std::runtime_error(inMessage)
    {
    }

    
    RequestHandler::RequestHandler(RequestMethod inRequestMethod, const std::string & inLocation, const std::string & inResponseContentType) :
        mSession(SessionFactory::instance().create("SQLite", "HighScores.db")),
        mRequestMethod(inRequestMethod),
        mLocation(inLocation),
        mResponseContentType(inResponseContentType)
    {
        // Create the table if it doesn't already exist
        mSession << "CREATE TABLE IF NOT EXISTS HighScores(Id INTEGER PRIMARY KEY, Name VARCHAR(20), Score INTEGER(5))", now;
    }


    void RequestHandler::GetArgs(const std::string & inURI, Args & outArgs)
    {
        if (inURI.empty())
        {
            return;
        }

        size_t offset = inURI.find('?');
        if (offset != std::string::npos)
        {
            // Search the substring that starts one character beyone the '?' mark.
            offset++;
        }
        else
        {
            // If no '?' was found then the entire string is considered to be a parameter list.
            offset = 0;
        }

        std::string argString = inURI.substr(offset, inURI.size() - offset);
        Poco::StringTokenizer tokenizer(argString, "&", Poco::StringTokenizer::TOK_IGNORE_EMPTY |
                                                        Poco::StringTokenizer::TOK_TRIM);

        Poco::StringTokenizer::Iterator it = tokenizer.begin(), end = tokenizer.end();
        for (; it != end; ++it)
        {
            const std::string & pair = *it;
            Poco::StringTokenizer t(pair, "=", Poco::StringTokenizer::TOK_IGNORE_EMPTY |
                                               Poco::StringTokenizer::TOK_TRIM);
            if (t.count() != 2)
            {
                continue;
            }
            outArgs.insert(std::make_pair(t[0], t[1]));
        }
    }


    const std::string & RequestHandler::GetArg(const Args & inArgs, const std::string & inArg)
    {
        Args::const_iterator it = inArgs.find(inArg);
        if (it != inArgs.end())
        {
            return it->second;
        }
        throw MissingArgumentException("Missing argument: " + inArg);
    }


    const std::string & RequestHandler::getResponseContentType() const
    {
        return mResponseContentType;
    }

    const std::string & RequestHandler::getLocation() const
    {
        return mLocation;
    }

    
    void RequestHandler::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                       Poco::Net::HTTPServerResponse& inResponse)
    {
        Poco::Util::Application& app = Poco::Util::Application::instance();
        app.logger().information("Request from " + inRequest.clientAddress().toString());

        inResponse.setChunkedTransferEncoding(true);
        inResponse.setContentType(getResponseContentType());

        try
        {
            generateResponse(inRequest, inResponse);
        }
        catch (const std::exception & inException)
        {
            app.logger().critical(inException.what());
        }
    }    
    
    
    RequestHandler * DefaultRequestHandler::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new DefaultRequestHandler;
    }

    DefaultRequestHandler::DefaultRequestHandler() :
        RequestHandler(RequestMethod_Get, "", "text/html")
    {
    }


    void DefaultRequestHandler::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse)
    {
        std::ifstream html("html/index.html");
        Poco::StreamCopier::copyStream(html, inResponse.send());
    }


    ErrorRequestHandler::ErrorRequestHandler(const std::string & inErrorMessage) :
        RequestHandler(RequestMethod_Get, "", "text/html"),
        mErrorMessage(inErrorMessage)
    {
    }


    void ErrorRequestHandler::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse)
    {
        std::ostream & ostr(inResponse.send());
        ostr << "<html><body>";
        ostr << "<h1>Error</h1>";
        ostr << "<p>" + mErrorMessage + "</p>";
        ostr << "</body></html>";
    }
    
    
    RequestHandler * GetAllHighScores::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetAllHighScores;
    }
    
    
    GetAllHighScores::GetAllHighScores() :
        RequestHandler(RequestMethod(), GetLocation(), "text/html")
    {
    }


    void GetAllHighScores::getRows(const Poco::Data::RecordSet & inRecordSet, std::string & outRows)
    {
        for (size_t rowIdx = 0; rowIdx != inRecordSet.rowCount(); ++rowIdx)
        {   
            outRows += "<tr>";
            for (size_t colIdx = 0; colIdx != inRecordSet.columnCount(); ++colIdx)
            {                
                outRows += "<td>";
                std::string cellValue;
                inRecordSet.value(colIdx, rowIdx).convert(cellValue);
                outRows += cellValue;
                outRows += "</td>";
            }
            outRows += "</tr>\n";
        }
    }


    void GetAllHighScores::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse)
    {   
        std::ostream & ostr(inResponse.send());
        Statement select(mSession);
        select << "SELECT * FROM HighScores";
        select.execute();
        RecordSet rs(select);

        std::string html;
        HSServer::ReadEntireFile("html/getall.html", html);
        std::string rows;
        getRows(rs, rows);
        ostr << Poco::replace<std::string>(html, "{{ROWS}}", rows);
    }

    
    RequestHandler * GetAddHighScore::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new GetAddHighScore;
    }


    GetAddHighScore::GetAddHighScore() :
        RequestHandler(GetRequestMethod(), GetLocation(), "text/html")
    {
    }


    void GetAddHighScore::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse)
    {        
        std::ifstream htmlFile("html/add.html");
        Poco::StreamCopier::copyStream(htmlFile, inResponse.send());
    }

    
    RequestHandler * PostHighScore::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return new PostHighScore;
    }


    PostHighScore::PostHighScore() :
        RequestHandler(GetRequestMethod(), GetLocation(), "text/plain")
    {
    }


    void PostHighScore::generateResponse(Poco::Net::HTTPServerRequest& inRequest,
                                             Poco::Net::HTTPServerResponse& inResponse)
    {
        std::string body;
        inRequest.stream() >> body;

        Args args;
        RequestHandler::GetArgs(body, args);

        std::string name = RequestHandler::GetArg(args, "name");
        std::string score = RequestHandler::GetArg(args, "score");

        Statement insert(mSession);
        insert << "INSERT INTO HighScores VALUES(NULL, ?, ?)", use(name),
                                                               use(score);
        insert.execute();

        // Return an URL instead of a HTML page.
        // This is because the client is the JavaScript application in this case.
        inResponse.send() << "http://localhost/hs/commit-succeeded?name=" << name << "&score=" << score;        
    }
    
    
    RequestHandler * CommitHighScore::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        Args args;
        GetArgs(inRequest.getURI(), args);
        return new CommitHighScore(GetArg(args, "name"), GetArg(args, "score"));
    }


    CommitHighScore::CommitHighScore(const std::string & inName, const std::string & inScore) :
        RequestHandler(GetRequestMethod(), GetLocation(), "text/plain"),
        mName(inName),
        mScore(inScore)
    {
    }


    void CommitHighScore::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse)
    {        
        Statement insert(mSession);        
        insert << "INSERT INTO HighScores VALUES(NULL, ?, ?)", use(mName),
                                                               use(mScore);
        insert.execute();

        // Return an URL instead of a HTML page.
        // This is because the client is the JavaScript application in this case.
        inResponse.send() << "http://localhost/hs/commit-succeeded?name=" << mName << "&score=" << mScore;
    }
    
    
    RequestHandler * CommitSucceeded::Create(const Poco::Net::HTTPServerRequest & inRequest)
    {
        Args args;
        GetArgs(inRequest.getURI(), args);
        return new CommitSucceeded(GetArg(args, "name"), GetArg(args, "score"));
    }


    CommitSucceeded::CommitSucceeded(const std::string & inName, const std::string & inScore) :
        RequestHandler(GetRequestMethod(), GetLocation(), "text/html"),
        mName(inName),
        mScore(inScore)
    {
    }


    void CommitSucceeded::generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse)
    {
        std::ostream & ostr(inResponse.send());
        ostr << "<html>";
        ostr << "<body>";
        ostr << "<p>";
        ostr << "Succesfully added highscore for " << mName << " of " << mScore << ".";
        ostr << "</p>";
        ostr << "</body>";
        ostr << "</html>";
    }

} // namespace HSServer
