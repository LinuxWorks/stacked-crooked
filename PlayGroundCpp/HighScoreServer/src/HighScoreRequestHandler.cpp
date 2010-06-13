#include "HighScoreRequestHandler.h"
#include "HTMLElements.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Util/Application.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>


using namespace Poco::Data;
using namespace HTML;


namespace HSServer
{
    
    typedef std::map<std::string, std::string> Args;

    void GetArgs(const std::string & inURI, Args & outArgs)
    {
        if (inURI.empty())
        {
            return;
        }

        if (inURI[0] != '/')
        {
            return;
        }

        std::string argString = inURI.substr(1, inURI.size() - 1);
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

    
    HighScoreRequestHandler::HighScoreRequestHandler() :
        mSession(SessionFactory::instance().create("SQLite", "HighScores.db"))
    {
        // Create the table if it doesn't already exist
        mSession << "CREATE TABLE IF NOT EXISTS HighScores(Id INTEGER PRIMARY KEY, Name VARCHAR(20), Score INTEGER(5))", now;
    }

    
    void HighScoreRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                                                Poco::Net::HTTPServerResponse& inResponse)
    {
        Poco::Util::Application& app = Poco::Util::Application::instance();
        app.logger().information("Request from " + inRequest.clientAddress().toString());

        inResponse.setChunkedTransferEncoding(true);
        inResponse.setContentType("text/html");

        std::ostream & outStream = inResponse.send();
        HTML::CurrentOutStream currentOutputStream(outStream);
        try
        {
            generateResponse(mSession, outStream);
        }
        catch (const std::exception & inException)
        {
            app.logger().critical(inException.what());
        }
    }    
    
    
    DefaultRequestHandler * DefaultRequestHandler::Create(const std::string & inURI)
    {
        return new DefaultRequestHandler;
    }


    void DefaultRequestHandler::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        Html html;
        Body body;
        H1 h1("High Score Server");
        P p("There is nothing here.");
        B b("I'm bold.");Br();
        I i("I'm scheef."); Br();
        U u("I'm underlined.");
        {
            P p1;
            B b1;
            I i1;
            U u1;
            Write("Nested everything.\n");
        }
    }
    
    
    GetAllHighScores * GetAllHighScores::Create(const std::string & inURI)
    {
        return new GetAllHighScores;
    }


    void GetAllHighScores::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        Html html;
        Body body;
        P p("High Score Table");
        Table table;        
        Statement select(inSession);
        select << "SELECT * FROM HighScores";
        select.execute();
        RecordSet rs(select);
        for (size_t rowIdx = 0; rowIdx != rs.rowCount(); ++rowIdx)
        {   
            Tr tr;
            for (size_t colIdx = 0; colIdx != rs.columnCount(); ++colIdx)
            {
                Td td;
            }
        }
    }
    
    
    AddHighScore * AddHighScore::Create(const std::string & inURI)
    {
        Args args;
        GetArgs(inURI, args);

        Args::iterator it = args.find("name");
        if (it == args.end())
        {
            throw std::runtime_error("Could not find 'name' argument in GET URL for AddHighScore method.");
        }

        std::string name = it->second;

        it = args.find("score");
        if (it == args.end())
        {
            // TODO: create class MissingArgument
            throw std::runtime_error("Could not find 'score' argument in GET URL for AddHighScore method.");
        }

        int score = 0;
        try
        {
            score = boost::lexical_cast<int>(it->second);
        }
        catch (const boost::bad_lexical_cast & )
        {
            throw std::runtime_error("Argument 'score' must be of type INTEGER.");
        }

        return new AddHighScore(HighScore(name, score));
    }


    AddHighScore::AddHighScore(const HighScore & inHighScore) :
        mHighScore(inHighScore)
    {
    }


    void AddHighScore::generateResponse(Poco::Data::Session & inSession, std::ostream & ostr)
    {
        HTMLElement html("html");
        HTMLElement body("body");

        Statement insert(inSession);

        insert << "INSERT INTO HighScores VALUES(NULL, ?, ?)", use(mHighScore.name()),
                                                               use(mHighScore.score());
        insert.execute();

        std::stringstream ss;
        ss << "Added High Score: " << mHighScore.name() << ": " << mHighScore.score();
        HTMLElement p("p", ss.str());
    }

} // namespace HSServer
