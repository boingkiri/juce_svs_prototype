/*
  ==============================================================================

    Client.h
    Created: 2 Jun 2022 10:31:56am
    Author:  vdjfelrl11

  ==============================================================================
*/

#pragma once

#include <cpr/cpr.h>
#include <JuceHeader.h>

class Client
{
public:

    Client();

    void setUrl(std::string);
    void sendRequest();

    // Just test function
    int network_test();

private:
    cpr::Url url;
    cpr::Response response;
};
