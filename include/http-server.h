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

/**
 * @file http-server.h
 * @brief This is the HTTP server header file for this project.
 * @author Alexander O'Connor
 * @date 30 December 2021
 */

#ifndef PIEAR_SERVER_HTTP_SERVER_H
#define PIEAR_SERVER_HTTP_SERVER_H

#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"

/**
 * @brief This runs the webserver.
 * @details On port 8080, this webserver is a getter/setter for the labels and cpm
 * @param cpm pointer to the cpm
 * @param audio_stream_count the number of audio streams
 * @param curr_hash this is the current hash of all labels, so that it is of consistent length.
 * This should be sent with the audio so the app knows if it needs to ask for the latest names.
 */
void mainloop_http_server(int *cpm, int audio_stream_count, char *curr_hash);



/**
 * Handles Label set
 */
class changeLabelHandler : public Poco::Net::HTTPRequestHandler {

    /**
     * Handles requests made to change the label of an audio source
     *
     * @param request Poco::Net::HTTPServerRequest
     * @param response Poco::Net::HTTPServerResponse
     */
    virtual void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
public:
    int index_to_change;     //!< Index of item to change
    char *name_to_change_to; //!< New name
};

/**
 * Handles Label get
 */
class getLabelHandler : public Poco::Net::HTTPRequestHandler {
    /**
     * Handles requests made to get the label of all audio sources
     *
     * @param request Poco::Net::HTTPServerRequest
     * @param response Poco::Net::HTTPServerResponse
     */
    virtual void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);

public:
    int *labels; //!< Audio stream labels
    //! @todo Implement actual label management system
};

/**
 * Handles CPM set
 */
class changeCPMlHandler : public Poco::Net::HTTPRequestHandler {

    /**
     * Handles requests made to change the cpm
     *
     * @param request Poco::Net::HTTPServerRequest
     * @param response Poco::Net::HTTPServerResponse
     */
    virtual void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
public:
    int cpm_to_change_to; //!< The new cpm
};

/**
 * Handles CPM get
 */
class getCPMHandler : public Poco::Net::HTTPRequestHandler {
    /**
     * Handles requests made to get the CPM
     *
     * @param request Poco::Net::HTTPServerRequest
     * @param response Poco::Net::HTTPServerResponse
     */
    virtual void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);

public:
    int *curr_cpm; //!< Pointer to CPM
};

class error : public Poco::Net::HTTPRequestHandler {
    /**
     * Returns HTTP_BAD_REQUEST
     *
     * @param request Poco::Net::HTTPServerRequest
     * @param response Poco::Net::HTTPServerResponse
     */
    virtual void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
};

/**
 * The Poco::Net::HTTPRequestHandlerFactory for the HTTP server.
 */
class factory : public Poco::Net::HTTPRequestHandlerFactory {
     /**
      * @brief Determines the Poco::Net::HTTPRequestHandler for the request.
      *
      * **POST:**\n
      * `/labels/<id:int>`   Changes the label of item at `id`\n
      * `/cpm`               Changes the cpm\n
      * **GET:**\n
      * `/labels`           Returns labels of all items, seperated by `\n`\n
      * @param request Poco::Net::HTTPServerRequest
      * @return Poco::NET:HTTPRequestHandler*
      */
    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(Poco::Net::HTTPServerRequest &request);

public:
    int *labels; //!< Audio stream labels
};

#endif //PIEAR_SERVER_HTTP_SERVER_H
