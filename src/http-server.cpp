//******************************************************************************
// Copyright © 2021  Alexander O'Connor
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//******************************************************************************

#include "http-server.h"
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>


void changeLabelHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("text/html");

    response.send() << "Success";
}

void getLabelHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("text/html");
    std::ostream& out = response.send();
    for (int i = 0; i < *(this->labels); ++i) {
        out << *labels << '\n';
    }
    out.flush();
}

void changeCPMlHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("text/html");

}

void error::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
}

Poco::Net::HTTPRequestHandler* factory::createRequestHandler(Poco::Net::HTTPServerRequest &request) {
    if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
        std::basic_string URI = request.getURI();
        if (URI.starts_with("/labels/")) {
            URI.erase(0, 8);
            int label_index;
            try {
                label_index = std::stoi(URI);
            } catch (std::invalid_argument e) {
                std::basic_string error_string = "URI: ";
                error_string.append(" didn't have int: ");
                error_string.append(request.getURI());
                perror(error_string.c_str());
                return new error;
            } catch (std::out_of_range) {
                std::basic_string error_string = "URI: ";
                error_string.append(request.getURI());
                error_string.append(" has int that is too large: ");
                perror(error_string.c_str());
                return new error;
            }
            changeLabelHandler *handler = new changeLabelHandler;
            handler->index_to_change = label_index;
            std::istream &i = request.stream();
            int len = request.getContentLength();
            handler->name_to_change_to = new char[len];
            i.read(handler->name_to_change_to, len);
            return handler;
        } else if (URI.starts_with("/cpm")) {
            changeCPMlHandler *handler = new changeCPMlHandler;

            std::istream &i = request.stream();
            int len = request.getContentLength();
            char *to_convert = new char[len];
            i.read(to_convert, len);
            try {
                handler->cpm_to_change_to = std::stoi(to_convert);
            } catch (std::invalid_argument e) {
                std::basic_string error_string = "URI: ";
                error_string.append(" didn't have int: ");
                error_string.append(request.getURI());
                perror(error_string.c_str());
                return new error;
            } catch (std::out_of_range) {
                std::basic_string error_string = "URI: ";
                error_string.append(request.getURI());
                error_string.append(" has int that is too large: ");
                perror(error_string.c_str());
                return new error;
            }
            return handler;
        } else {
            return new error;
        }
    } else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
        if (request.getURI().starts_with("/labels")) {
            getLabelHandler *handler = new getLabelHandler;
            handler->labels = this->labels;
            return handler;
        } else if (request.getURI().starts_with("/cpm")) {
            return new getLabelHandler;
        } else {
            return new error;
        }
    }
    return new error;
}

void mainloop_http_server(int *cpm, int audio_stream_count, char *curr_hash) {

};
