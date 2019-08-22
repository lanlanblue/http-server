#include <sys/socket.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

using namespace std;

string genTable(string URL) {
    string table;
    DIR * dir = opendir(URL.c_str());
    if (dir == NULL) {
        cout << "Can't find directory/file: " << URL << endl; 
        return table;
    }
    string item;
    int index = 1;
    struct dirent *entry;
    struct stat statbuf;
    while ((entry = readdir(dir)) != NULL) {
        lstat((URL+ "/" + string(entry->d_name)).c_str(), &statbuf);
        if (string(entry->d_name) == "." || string(entry->d_name) == "..") {
            continue;
        }
        item =
            R"(<tr>)" "\n"
            R"(  <th scope="row">)";
        item += to_string(index) + "</th>\n";
        string linkPath = URL + "/" + string(entry->d_name);
        size_t found = linkPath.find("root_dir");
        linkPath.replace(found, 8, "");
        
        if (S_ISDIR(statbuf.st_mode)) {
            item += "<td><span class=\"glyphicon glyphicon-folder-open\" style=\"margin-left: 5px;margin-right: 10px\"></span><a href=\"" + linkPath + "\">" + string(entry->d_name) + "</a></td>\n";
        } else {
            item += "<td><span class=\"glyphicon glyphicon-file\" style=\"margin-left: 5px;margin-right: 10px\"></span><a href=\"" + linkPath + "\">" + string(entry->d_name) + "</a></td>\n";
        }
        item += "<td>" + to_string(statbuf.st_size) + "</td>\n";
        char buff[30];
        strftime(buff, 30, "%Y-%m-%d %H:%M:%S", localtime(&statbuf.st_mtime));
        item += "<td>" + string(buff) + "</td>\n";
        item += "</tr>\n";
        index++;
        table += item;
        item = "";
    }
    closedir(dir);
    return table;
}

int main () {
    //1. Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cout << "cannot create socket" << endl;
        return 0;
    }
    //2. Identify(Name) a socket
    //struct sockaddr_in 
    //{ 
    //    __uint8_t         sin_len; 
    //    sa_family_t       sin_family; 
    //    in_port_t         sin_port; 
    //    struct in_addr    sin_addr; 
    //    char              sin_zero[8]; 
    //};
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    const int port = 8080;

    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    /* htonl converts a long integer (e.g. address) to a network representation */
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    /* htons converts a short integer (e.g. port) to a network representation */ 
    address.sin_port = htons(port);

    int reuseaddr = 1;
    socklen_t reuseaddr_len = sizeof(reuseaddr);
    //for reusing address
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, reuseaddr_len);
    if (bind(server_fd, (struct sockaddr*)& address, sizeof(address)) < 0) {
        cout << "bind failed" << endl;
        return 0;
    }
    //3. On the server, wait for an incoming connection
    if (listen(server_fd, 3) < 0) {
        cout << "In listen" << endl;
        exit(EXIT_FAILURE);
    }

    while (1) {
        cout << "+++++++ Waiting for new connection +++++" << endl;
        int new_socket = accept(server_fd, (struct sockaddr*)& address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            cout << "In accept" << endl;
            exit(EXIT_FAILURE);
        }
        //4. Send & receive messages
        char buffer[1024] = {0};
        int valread = read(new_socket, buffer, 1024);
        //print out new request
        cout << buffer << endl;
        if (valread < 0) {
            cout << "No bytes are there to read" << endl;
        }

        //check from root directory
        string response;
        stringstream ss(buffer);
        string method;
        string URL;
        string mainPage = "root_dir/index.html";
        ss >> method;
        ss >> URL;
        if (URL == "/") {
            URL = "root_dir";
        } else {
            URL = "root_dir" + URL;
        }
        string fileName = URL.substr(URL.find_last_of("/")+1);
        cout << "Method: " << method << endl;
        cout << "URL: " << URL << endl;
        ifstream f(mainPage);
        if (f) {
            //200 OK
            //open URL file
            string tableStr((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
            response = 
                R"(HTTP/1.1 200 OK)" "\n"
                R"(Date: Mon, 27 Jul 2009 12:28:53 GMT)" "\n"
                R"(Server: Apache/2.2.14 (Win32))" "\n"
                R"(Last-Modified: Wed, 22 Jul 2009 19:15:56 GM)" "\n";
            //check if URL is a dir
            struct stat statbuf;
            lstat(URL.c_str(), &statbuf);
            if (S_ISDIR(statbuf.st_mode)) {
                //if it is folder, reply subdirectory info
                string table = genTable(URL);
                //modify HTML
                //inser after <tbody>
                size_t found = tableStr.find("<tbody>");
                if (found != string::npos) {
                    tableStr.insert(int(found)+7, table);
                }
                response += R"(Content-Length:)";
                response += to_string(tableStr.length()) + "\n";
                response +=
                    R"(Content-Type: text/html)" "\n"
                    R"(Connection: Closed)" "\n\n" + tableStr;
                cout << response << endl;
            } else {
                //if it is file, reply download response

                    ifstream f(URL);
                    string downloadStr((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
                response += 
                    R"(Content-Description: File Transfer)" "\n"
                    R"(Content-Type: application/octet-stream)" "\n"
                    R"(Content-Disposition: attachment;filename=")"                 + fileName + "\"\n"
                    R"(Content-Transfer-Encoding: binary)" "\n"
                    R"(Connection: Keep-Alive)" "\n"
                    R"(Expires: 0)" "\n"
                    R"(Cache-Control: must-revalidate, post-check=0, pre-check=0)" "\n"
                    R"(Pragma: public)" "\n"
                    R"(Content-Length: )" + to_string(downloadStr.length()) + "\n\n";
                response += downloadStr;

            }
        }
        write(new_socket, response.c_str(), response.length());
        cout << "Response sent!" << endl;
        //5. Close the socket
        close(new_socket);
    }
    return 0;
}
