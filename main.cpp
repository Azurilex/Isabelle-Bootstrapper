/*
 *	main.cpp
 *	Isabelle Bootstrapper
 *
 *	Created by Azurilex on 04/01/2020
 *	Copyright © 2020 Azurilex. All rights reserved
 *	Checks for updates and downloads files required for Isabelle's functionality
 */

#define NOMINMAX
#define CURL_STATICLIB
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "curl/curl.h"
#include "termcolor.hpp"
#include <boost/timer.hpp>
#include <boost/filesystem.hpp>
#include <direct.h>
#include <VMProtectSDK.h>


#ifdef _DEBUG
inline bool debug = true; //debug?
#pragma comment(lib, "curl/libcurl_a_debug.lib")
#else
inline bool debug = false; //debug?
#pragma comment(lib, "curl/libcurl_a.lib")
#endif

#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "advapi32.lib")

inline bool check = false; //global checks
inline bool check2 = false; //global checks

std::string path() {
	char buffer[MAX_PATH];
	GetModuleFileName(nullptr, buffer, MAX_PATH); //gets directory
	auto f = std::string(buffer); //to string
	return f.substr(0, f.find_last_of("\\/")); //truncate
}

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE *stream) //write data function for file explorer (stackoverflow)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

size_t writecallback(void* contents, size_t size, size_t nmemb, void* userp) //writecallback for text buffer (stackoverflow)
{
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

bool download(const char* name, const char* url)
{
    CURL* curl = curl_easy_init();
    if (curl)
    {
        FILE* fp = fopen(name, "wb"); //location of where file will be saved to
        curl_easy_setopt(curl, CURLOPT_URL, url); //url for request
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); //pass data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //writes to location of fp
        if(debug) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //if program is being ran in debug, verbose log will output
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl); //clean up
        fclose(fp);
        return true; //returns true if everything worked out
    }
    return false; //returns false if something bad happened :(
}

int main()
{
    VMProtectBeginUltra("Main");
    std::string readbuffer; //buffer for version file that's on host
    CURL* curl = curl_easy_init(); //initialize libcurl

    std::string appdata = getenv("APPDATA"); //Gets %appdata% directory. this function isn't compatible with windows xp.
    appdata += "\\Isabelle"; // "%appdata%\Isabelle"
	
    if (boost::filesystem::exists(appdata)) { //check if folder inside appdata already exists
        std::string version = appdata + "\\version.txt"; // "%appdata%\Isabelle\version.txt"
        if (boost::filesystem::exists(version)) //check if version file exists
        {
            if (curl)
            {
                std::cout << "[" << termcolor::yellow << "*" << termcolor::white << "] Checking version..."; //interface purposes
                curl_easy_setopt(curl, CURLOPT_URL, VMProtectDecryptStringA("https://REDACTEDt")); //provides url for request
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback); //pass data
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readbuffer); //writes to buffer
                CURLcode res = curl_easy_perform(curl);
                curl_easy_cleanup(curl); //clean-up curl :)

                if (!readbuffer.empty()) //checks to make sure string isn't empty
                {
                    std::string outputstring;
                    std::string buff;
                    std::ifstream input(version); //gets text inside version file
                    while (std::getline(input, buff))
                        outputstring += (buff); //writes data from buffer

                    readbuffer.erase(std::remove(readbuffer.begin(), readbuffer.end(), '\n'), readbuffer.end()); //removes empty line from host string
                    readbuffer.erase(std::remove(readbuffer.begin(), readbuffer.end(), '\r'), readbuffer.end());

                    if (readbuffer == outputstring) //check if curl buffer (host's "version.txt" file is the same as the local "version.txt" file
                    {
                    	if (!boost::filesystem::exists("Isabella.dll")) //make sure all files exist before exiting, if missing it'll download it
                    	{
                            std::cout << termcolor::yellow << " Missing files found! Downloading..." << termcolor::white << std::endl;
                            boost::timer two; //timer :)
                            std::cout << "[" << termcolor::yellow << "*" << termcolor::white << "] Downloading \"Isabella.dll\"... " << termcolor::green;
                            std::string dlls = path() + "\\Isabella.dll"; // %DIRECTORY%\Isabelle.dll
                            if (dlls.size() > FILENAME_MAX) return 0; // makes sure it's not over 260 chars
                            char* dllurl = const_cast<char*>(VMProtectDecryptStringA("https://REDACTED")); //url for file, used for curl
                            const char* dllfile = dlls.c_str();
                            if (!download(dllfile, dllurl)) std::cout << termcolor::red << "ERROR DOWNLOADING ISABELLA.DLL" << termcolor::white << std::endl; //downloads file
                            else std::cout << "Done! " << termcolor::white << "(" << two.elapsed() << "s)" << std::endl;
                    		//checks for the rest of the code (interface purposes)
                            check = true;
                            check2 = true;
                    	}
                        if (!boost::filesystem::exists("Changelog.txt")) //make sure all files exist before exiting, if missing it'll download it
                        {
                        	if (check2) std::cout << termcolor::yellow << "Missing files found! Downloading..." << termcolor::white << std::endl; //check for interface purposes
                            else std::cout << termcolor::yellow << " Missing files found! Downloading..." << termcolor::white << std::endl;
                            boost::timer one; //timer :)
                            std::cout << "[" << termcolor::yellow << "*" << termcolor::white << "] Downloading \"changelog.txt\"... " << termcolor::green;
                            std::string s = path() + "\\Changelog.txt"; // %DIRECTORY%\Changelog.txt
                            if (s.size() > FILENAME_MAX) return 0; // makes sure it's not over 260 chars
                            char* clurl = const_cast<char*>(VMProtectDecryptStringA("https://REDACTED")); //url for file, used for curl
                            const char* clfile = s.c_str();
                            if (!download(clfile, clurl)) std::cout << termcolor::red << "ERROR DOWNLOADING CHANGELOG.TXT" << termcolor::white << std::endl; //downloads file
                            else std::cout << "Done! " << termcolor::white << "(" << one.elapsed() << "s)" << std::endl;
                        	//checks for the rest of the code
                            check = true;
                        }
                        if (!check) std::cout << termcolor::green << " Already up-to-date!" << termcolor::white << std::endl; //all files exist, bootstrapper can close
                        system("PAUSE");
                        return 0;
                    }

                    std::cout << termcolor::yellow << " Outdated!" << termcolor::white << std::endl; //outdated, will proceed to the rest of main()
                }
                else
                {
                    std::cout << "ERROR CHECKING VERSION. PRESS ENTER TO CONTINUE..." << std::endl; //unresolved
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
        } //if version file doesn't exist, it'll go to the rest of main() and download everything along with version
    }
    else // %appdata%\Isabelle not found, making directory now -> will continue to rest of main() to download files
    {
        _mkdir(appdata.c_str());
    }

    boost::timer one; //timer :)
    std::cout << "[" << termcolor::yellow << "*" << termcolor::white << "] Downloading \"changelog.txt\"... " << termcolor::green;
    std::string s = path() + "\\Changelog.txt"; // %DIRECTORY%\Changelog.txt
    if (s.size() > FILENAME_MAX) return 0; //makes sure it's not over 260 chars
    char* clurl = const_cast<char*>(VMProtectDecryptStringA("https://REDACTED")); //url for file, used for curl
    const char* clfile = s.c_str();
    if (!download(clfile, clurl)) std::cout << termcolor::red << "ERROR DOWNLOADING CHANGELOG.TXT" << termcolor::white << std::endl; //downloads file
    else std::cout << "Done! " << termcolor::white << "(" << one.elapsed() << "s)" << std::endl;

    boost::timer two; //timer :)
    std::cout << "[" << termcolor::yellow << "*" << termcolor::white << "] Downloading \"Isabella.dll\"... " << termcolor::green;
    std::string dlls = path() + "\\Isabella.dll"; // %DIRECTORY%\Isabella.dll
    if (dlls.size() > FILENAME_MAX) return 0; //makes sure it's not over 260 chars
    char* dllurl = const_cast<char*>(VMProtectDecryptStringA("https://REDACTEDl")); //url for file, used for curl
    const char* dllfile = dlls.c_str();
    if (!download(dllfile, dllurl)) std::cout << termcolor::red << "ERROR DOWNLOADING ISABELLA.DLL" << termcolor::white << std::endl; //downloads file
    else std::cout << "Done! " << termcolor::white << "(" << two.elapsed() << "s)" << std::endl;

    boost::timer three; //timer :)
    std::cout << "[" << termcolor::yellow << "*" << termcolor::white << "] Updating version identifier... " << termcolor::green;
    std::string ver = appdata + "\\version.txt"; // %appdata%\Isabelle\version.txt
    if (ver.size() > FILENAME_MAX) return 0; //makes sure it's not over 260 chars
    char* verurl = const_cast<char*>(VMProtectDecryptStringA("https://REDACTED")); //url for file, used for curl
    const char* verfile = ver.c_str();
    if (!download(verfile, verurl)) std::cout << termcolor::red << "ERROR DOWNLOADING ISABELLA.DLL" << termcolor::white << std::endl; //downloads new version file
    else std::cout << "Done! " << termcolor::white << "(" << three.elapsed() << "s)" << std::endl;
    
    system("PAUSE"); //bootstrapper is done, it can now close.
    VMProtectEnd();
    return 0;
}
