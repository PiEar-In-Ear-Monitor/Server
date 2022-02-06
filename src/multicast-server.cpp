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

#include "multicast-server.h"
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/MulticastSocket.h>
#include <Poco/Net/NetException.h>
#include <iostream>

void mainloop_multicast_server(int num_of_audio_streams) {
    Poco::Net::initializeNetwork();
    try {
        Poco::Net::SocketAddress address(Poco::Net::IPAddress(), 6666), sendto(Poco::Net::IPAddress(), 6666);
        Poco::Net::MulticastSocket socket(address);
        socket.connect(sendto);

        std::cout << "Sending Hello" << std::endl;
        socket.sendBytes("Hello", 5);
    }
    catch(const Poco::Net::NetException& ex) {
        std::cout << ex.displayText() << std::endl << std::flush;
    }
    Poco::Net::uninitializeNetwork();
}