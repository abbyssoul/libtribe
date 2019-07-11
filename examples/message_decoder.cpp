/*
*  Copyright 2018 Ivan Ryabov
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/

#include <tribe/parser.hpp>

#include <getopt.h>


using namespace Solace;
using namespace tribe;


/// Print app usage
int usage(const char* progname) {
    std::cout << "Usage: " << progname
              << "[-m <size>] "
              << "[-p <version>] "
              << "[-h] "
              << " [FILE]..."
              << std::endl;

    std::cout << "Parse gossip protocol message and display it in a human-readable form\n\n"
              << "Options: \n"
              << " -m <size> - Use maximum buffer size for a message [Default: " << kMaxMesssageSize << "]\n"
              << " -p <version> - Use specific protocol version [Default: " << Parser::PROTOCOL_VERSION << "]\n"
              << " -h - Display help and exit\n"
              << std::endl;

    return EXIT_SUCCESS;
}


/**
 * A simple example of decoding a 9P message from a file / stdin and printing it in a human readable format.
 */
int main(int argc, char* const* argv) {
    size_type maxMessageSize = kMaxMesssageSize;
    StringView requiredVersion = Parser::PROTOCOL_VERSION;

    int c;
    while ((c = getopt(argc, argv, "m:p:h")) != -1)
        switch (c) {
        case 'm': {
            int requestedSize = atoi(optarg);
            if (requestedSize <= 0) {
                std::cerr << "Option - "<< optopt << " requires positive interger value.\n";
                return EXIT_FAILURE;
            }
            maxMessageSize = requestedSize;
        } break;
        case 'p':
            requiredVersion = StringView{optarg};
            break;
        case 'h':
            return usage(argv[0]);
        default:
            return EXIT_FAILURE;
    }

    Parser proc{maxMessageSize, requiredVersion};
    MemoryManager memManager{proc.maxPossibleMessageSize()};
    auto buffer = memManager.allocate(proc.maxPossibleMessageSize());

    if (optind < argc) {
        for (int i = optind; i < argc; ++i) {
            std::ifstream input(argv[i]);
            if (!input) {
                std::cerr << "Failed to open file: " << std::quoted(argv[i]) << std::endl;
                return EXIT_FAILURE;
            }

            readAndPrintMessage(input, buffer, proc);
        }
    } else {
        readAndPrintMessage(std::cin, buffer, proc);
    }

    return EXIT_SUCCESS;
}
