/**
 * @file Header_Info.cpp
 * @brief Contains common information for all Dynamo programs
 */
// System libraries
#include <string>
#include <pwd.h> // getpwuid
#include <unistd.h> // getuid, gethostname

// System libraries (only for time query; might move that code somewhere else)
#include <iomanip>

#include "Header_Info.h"

// Constructor
HeaderInfo::HeaderInfo(std::string program_name, std::string code_version, std::string git_branch,
                       std::string git_hash, std::string user_name,
                       std::string os_version, std::string date, std::string time)
{
    // Copy flags locally
    this->local_flag_program_name = std::move(program_name);
    this->local_flag_code_version = std::move(code_version);
    this->local_flag_git_branch = std::move(git_branch);
    this->local_flag_git_hash = std::move(git_hash);
    this->local_flag_user_name = std::move(user_name);
    this->local_flag_os_version = std::move(os_version);
    this->local_flag_date = std::move(date);
    this->local_flag_time = std::move(time);
}

// Get hostname of current machine
std::string HeaderInfo::get_hostname()
{
    char hostname[1024];
    gethostname(hostname, 1024);
    return std::string(hostname);
}

// Get current username
std::string HeaderInfo::get_username()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid (uid);
    return pw ? std::string(pw->pw_name) : "";
}

// Get current date
std::string HeaderInfo::get_current_date()
{
    // Get current datetime
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream buf{};
    buf << std::put_time(&tm, "%F %T");
    auto datetime = buf.str(); // String
    return datetime;
}

// Build ASCII art banner
std::string HeaderInfo::get_banner()
{
    // Dynamo3 banner
    // http://www.network-science.de/ascii/  font: letters
    std::string banner_str{};
    banner_str += "VV     VV                              DDDDD     AAA        ++         ++    \n";
    banner_str += "VV     VV   eee  rr rr  nn nnn    eee  DD  DD   AAAAA       ++         ++    \n";
    banner_str += " VV   VV  ee   e rrr  r nnn  nn ee   e DD   DD AA   AA  ++++++++++ ++++++++++\n";
    banner_str += "  VV VV   eeeee  rr     nn   nn eeeee  DD   DD AAAAAAA      ++         ++    \n";
    banner_str += "   VVV     eeeee rr     nn   nn  eeeee DDDDDD  AA   AA      ++         ++     ";

    // Return banner
    return banner_str;
}

// Build sudo greeting
std::string HeaderInfo::get_sudo_greeting()
{
    // Sudo greeting str
    std::string sudo_greeting_str{};

    // If executing as sudo, show welcome message
    if (!getuid())
    {
        sudo_greeting_str += "Gokigen'yō, Goshujin-sama~\n";
    }

    // Return sudo greeting
    return sudo_greeting_str;
}

// Build compilation information
std::string HeaderInfo::get_compilation_info()
{
    // Compilation info message
    std::string compilation_info_str{};
    compilation_info_str += "VerneDA++ " + this->get_program_name() + "\n";
    compilation_info_str += "SW. version: " + this->get_code_version() + "\n";
    compilation_info_str += "Repo branch: " + this->get_git_branch() + "\n";
    compilation_info_str += "Commit hash: " + this->get_git_hash() + "\n";
    compilation_info_str += "Compiled by: " + this->get_user_name() + "\n";
    compilation_info_str += "Compiled on: " + this->get_date() + ", " + this->get_time() + "\n";
    compilation_info_str += "Compiled on: " + this->get_os_version() + "\n";

    // Return compilation info
    return compilation_info_str;
}

// Print compilation information
void HeaderInfo::print_compilation_info()
{
    (void) std::fprintf(stdout, "%s\n", this->get_compilation_info().c_str());
}

// Build ASCII banner and compilation info
// To be used as standard welcome banner for all mains
std::string HeaderInfo::get_header_info()
{
    // Local variables
    std::string header_info_str{};
    std::string sudo_greeting = HeaderInfo::get_sudo_greeting();

    // Build header banner
    header_info_str += "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n";
    header_info_str += HeaderInfo::get_banner() + "\n";
    header_info_str +=                                                            "\n";
    header_info_str += sudo_greeting.empty() ? "" : sudo_greeting +               "\n";
    header_info_str += this->get_compilation_info() +                             "\n";
    header_info_str +=  "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=";

    // Return entire header as a str
    return header_info_str;
}

// Print ASCII banner and compilation info
void HeaderInfo::print_header_info()
{
    (void) std::fprintf(stdout, "%s\n", this->get_header_info().c_str());
}

