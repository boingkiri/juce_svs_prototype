/*
  ==============================================================================

    Client.cpp
    Created: 2 Jun 2022 10:31:56am
    Author:  vdjfelrl11

  ==============================================================================
*/

#include "Client.h"

//using namespace std;

//int main(int argc, char** argv)
Client::Client()
{

}

void Client::setUrl(std::string url_string)
{
    url = cpr::Url{ url_string };
}

int Client::network_test()
{
    cpr::Response r = cpr::Get(
        cpr::Url{ "https://www.naver.com" }
    );
    DBG(r.status_code);
    DBG(r.text);

    return 0;
}
void Client::sendRequest()
{
    response = cpr::Get(url);

    DBG("REQUEST");
    //DBG(response.url);
    DBG(response.status_code);
    DBG(response.text);
    DBG("END");
    //return 0;
}