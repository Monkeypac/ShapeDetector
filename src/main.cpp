/**
 * @file main.cpp
 * Application entry points. Handles parameters and starts processing.
 */

#include <iostream>
#include "Log.h"
#include "Application.h"

std::string infos = 
    "Usage: ./ShapeDetector <image_path> [options]\n\n"
	"Options:\n"
	"\t-t[tolerance]\tSet custom tolerance. Default is 1.\n"
	"\t-o[path]\tSet custom output image. Default is map.xml.\n"
	"\t-p\tActivate creating a file representing the found shapes.\n"
	"\t-v\tActivate verbose mode.\n"
	"\t-D\tActivate debug mode.\n"
	"\t-T\tActivate time count.\n"
	;

extern int flags;

int main(int argc, char** argv)
{
	std::string path;
	std::string output = "map.xml";
	float tolerance = 1.f;

	if (argc <= 1)
	{
        std::cout << infos << std::endl;
		exit(1);
	}

	flags = 0;
	path = argv[1];
	for (int i = 2; i < argc; i++) {
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 't':	
				tolerance = atof(&argv[i][2]);
				break;
			case 'o':
				output = &argv[i][2];
				break;
			case 'D':
				flags |= DEBUG;
				Log::info("Option") << "Debug ON";
				break;
			case 'T':
				flags |= TIME;
				Log::info("Option") << "Time ON";
				break;
			case 'p':
				flags |= PRINT;
				Log::info("Option") << "Print ON";
				break;
			case 'v':
				flags |= VERBOSE;
				Log::info("Option") << "Verbose ON";
				break;
            case 'h':
                std::cout << std::endl << infos << std::endl;
                return 0;
                break;
            default:
                Log::error("Parameters") << "Unknown parameter: " << argv[i][1];
                exit(1);
                break;
			}
		}
	}

    if (flags & VERBOSE) {
        Log::info("Path") << path;
        Log::info("Output") << output;
        Log::info("Tolerance") << tolerance;
    }

	Application a(path);
    //a.process(tolerance, std::ref(output));
    a.process(tolerance, output);

	return 0;
}
