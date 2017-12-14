#ifndef CMSSW_PARSE_H
#define CMSSW_PARSE_H

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>

void print_usage(const char* first_arg) {

    printf("\nUsage: %s INPUTS [INPUTS ...] OUTPUT\n\n", first_arg);
    printf("INPUTS   Must be valid panda files.\n");
    printf("OUTPUT   Is the name of the output file that is used in monojet analysis\n");
    printf("         The file must not exist prior to running %s\n\n", first_arg);

}

/**
   This function checks that the input files exist, and that the output file does not exist.
   A missing input file leads to exit code 2.
   An existing output file leads to exit code 3.
 */

int check_files_then_send(int argc, char** argv, int (*desired_main)(int, char**)) {

  // Check input files

  struct stat buffer;

  for (int i_file = 1; i_file < argc; i_file++) {
    int stat_code = stat(argv[i_file], &buffer);

    if (stat_code != 0 && i_file != (argc - 1)) {
      print_usage(argv[0]);
      printf("Could not find file located at %s\n", argv[i_file]);
      printf("Please try again.\n");
      return 2;
    }
    else if (stat_code == 0 && i_file == (argc - 1)) {
      print_usage(argv[0]);
      printf("File %s already exists.\n", argv[i_file]);
      printf("Please select a different file to write to.\n");
      return 3;
    }
  }

  return desired_main(argc, argv);

}

/**
   This function takes the argc and argv you get from a command line input,
   and checks for CMSSW-style inputFiles and outputFile parameters.
   It only does this if argv[1] is a fake python file name
   (which cmsRun would usually use, for example).

   After parsing and adjusting the input into something that actually matches the
   print_usage message above, this function calls the desired_main function.
 */

int parse_then_send(int argc, char** argv, int (*desired_main)(int, char**)) {

  // Check arguments

  const char* dummyinput = "inputFiles";
  const char* dummyoutput = "outputFile";

  if (argc < 3) {
    print_usage(argv[0]);
    printf("Alternatively, you can use this goofy cmsRun-style syntax:\n\n");
    printf("    %s fake.py %s=INPUTS,INPUTS[,INPUTS...] %s=OUTPUT\n\n",
           argv[0], dummyinput, dummyoutput);
    printf("Be sure to put some fake python file name there,\n");
    printf("if that's what you want. Otherwise I won't parse.\n\n");
    return 1;
  }

  if (strcmp(argv[1] + strlen(argv[1]) - 3, ".py") == 0) {
    printf("Detected fake python file %s at the beginning.\n", argv[1]);
    printf("Parsing inputs for CMSSW style.\n\n");

    // Check that the input format matches what we expect for CMSSW style
    assert(argc == 4);
    assert(argv[2][strlen(dummyinput)] == '=');
    assert(argv[3][strlen(dummyoutput)] == '=');

    int parsed_argc = 1;
    const int MAX_ARGS = 2048;
    char* parsed_argv[MAX_ARGS];
    parsed_argv[0] = argv[0];

    // Place null characters in the proper places in the input
    int num_chars = strlen(argv[2]);
    bool just_found_sep = true;
    for (int i_char = strlen(dummyinput) + 1; i_char < num_chars; i_char++) {
      if (just_found_sep) {
        parsed_argv[parsed_argc++] = argv[2] + i_char;
        just_found_sep = false;
        assert(parsed_argc < MAX_ARGS);   // Will add one more arg at the end, so equal is no good
      }

      if (argv[2][i_char] == ',') {
        argv[2][i_char] = '\0';           // Null character to end last string
        just_found_sep = true;            // Be prepared to add next
      }
    }

    // Set the output file
    parsed_argv[parsed_argc++] = argv[3] + strlen(dummyoutput) + 1;

    return check_files_then_send(parsed_argc, parsed_argv, desired_main);
  }

  // Otherwise, check arguments

  return check_files_then_send(argc, argv, desired_main);

}

#endif
