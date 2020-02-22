// RestapiHttpServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/system/system_error.hpp>

#include "Server.h"

int main()
{
    const unsigned short PORT = 3333;
    const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

    try 
    {
        Server srv;

        unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;

        if (thread_pool_size == 0)
            thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
         
        srv.start(PORT, thread_pool_size);

        std::cout << "press any key to exit" << std::endl;
        std::cin.get();

        srv.stop();
    }
    catch (boost::system::system_error &e) 
    {
        std::cout << "Error occured! Error code = "	<< e.code() << ". Message: " << e.what();
    }

    return 0;
}

