// RtstapiHttpServer_unittest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <gtest\gtest.h>

#include "CurlWrapper.h"


TEST(RequestTest, RequestUsers)
{
    std::string response;

    CurlWrapper aCurlWrapper;
    EXPECT_TRUE(aCurlWrapper.request("users", response));
    std::cout << response << std::endl;

    EXPECT_FALSE(response.empty());
}

TEST(RequestTest, RequestTodos)
{
    std::string response;

    CurlWrapper aCurlWrapper;
    EXPECT_TRUE(aCurlWrapper.request("todos", response));
    std::cout << response << std::endl;

    EXPECT_FALSE(response.empty());
}

TEST(RequestTest, RequestHobbies)
{
    std::string response;

    CurlWrapper aCurlWrapper;
    EXPECT_TRUE(aCurlWrapper.request("hobbies", response));
    std::cout << response << std::endl;

    EXPECT_FALSE(response.empty());
}

TEST(RequestTest, RequestSchool)
{
    std::string response;

    CurlWrapper aCurlWrapper;
    EXPECT_TRUE(aCurlWrapper.request("school", response));
    std::cout << response << std::endl;

    EXPECT_FALSE(response.empty());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();
    system("pause");
    return ret;
}

