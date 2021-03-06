require 'rubygems'
require 'fileutils'
require 'mongrel'
require 'popen4'
require 'pp'


$local = false


$host = $local ? "localhost" : "stacked-crooked.com"
$port = $local ? "4000" : 80



class SimpleHandler < Mongrel::HttpHandler
    def process(request, response)
        response.start(200) do |head,out|
            head["Content-Type"] = "text/html"
            case get_location(request)
            when ""
                FileUtils.copy_stream(File.new("cmd.html"), out)
            when "compile"

                # WRITE MAIN
                File.open("main.cpp", 'w') { |f| f.write(request.body.string) }

                # COMPILE
                status = POpen4::popen4("ccache g++ -o test -std=c++0x main.cpp >output 2>&1") do |stdout, stderr, stdin, pid|
                    stdin.close()
                end

                if status == 0
                    out << "Compilation succeeded.\n\n"
                else
                    out << "Compilation failed with the following errors:\n\n"
                end
                FileUtils.copy_stream(File.new("output"), out)
            when "favicon.ico"
                # Don't respond to favicon..
            else
                puts "Don't know how to respond to the request to #{location}."
            end
        end
    end


    # Returns the location. E.g: if the URL is "http://localhost.com/compile" then "ccompile" will be returned.
    def get_location(request)
        return request.params["REQUEST_PATH"][1..-1].gsub(/%20/, " ").gsub(/%22/, "\"")
    end

    def return_compilation_result(out, payload)
    end

end


puts "Listening to #{$host}:#{$port}"
h = Mongrel::HttpServer.new($host, $port)
h.register("/", SimpleHandler.new)
h.run.join
